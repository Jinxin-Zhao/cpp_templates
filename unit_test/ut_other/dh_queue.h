#ifndef _DH_QUEUE_H_
#define _DH_QUEUE_H_

#include <iostream>
#include <fstream>
#include <mutex>
#include <memory>
#include <string>
#include <deque>
#include <thread>
#include <stdlib.h>
#include <errno.h>
#include <memory.h>

class FilePersistentQueue {
public:
    explicit FilePersistentQueue(const std::string& filename)
        : m_filename(filename), m_readPos(0), m_writePos(0) {
        initialize();
    }

    ~FilePersistentQueue() {
        if (m_file.is_open()) {
            m_file.close();
        }
    }

    bool push(const void* data, size_t size) {
        std::lock_guard<std::mutex> lock(m_fileMutex);
        
        try {
            // 移动到写入位置
            m_file.seekp(m_writePos);
            if (!m_file) return false;

            // 写入消息头（包含消息长度和校验和）
            MessageHeader header;
            header.size = size;
            header.checksum = calculateChecksum(data, size);
            
            m_file.write(reinterpret_cast<const char*>(&header), sizeof(header));
            if (!m_file) return false;

            // 写入消息体
            m_file.write(reinterpret_cast<const char*>(data), size);
            if (!m_file) return false;

            m_file.flush();
            m_writePos = m_file.tellp();
            
            // 更新内存索引
            m_index.push_back({m_writePos, size});
            
            return true;
        } catch (...) {
            recoverFromError();
            return false;
        }
    }

    bool pop(void* data, size_t expectedSize) {
        std::lock_guard<std::mutex> lock(m_fileMutex);
        
        if (m_index.empty()) {
            return false;
        }

        try {
            const auto& entry = m_index.front();
            if (entry.size != expectedSize) {
                return false;
            }

            // 移动到读取位置
            m_file.seekg(entry.position - entry.size - sizeof(MessageHeader));
            if (!m_file) return false;

            // 读取消息头
            MessageHeader header;
            m_file.read(reinterpret_cast<char*>(&header), sizeof(header));
            if (!m_file || header.size != expectedSize) return false;

            // 读取消息体
            m_file.read(reinterpret_cast<char*>(data), header.size);
            if (!m_file) return false;

            // 验证校验和
            if (header.checksum != calculateChecksum(data, header.size)) {
                return false;
            }

            // 更新读取位置和索引
            m_readPos = m_file.tellg();
            m_index.pop_front();
            
            // 定期压缩文件
            if (m_index.size() > 0 && m_index.size() % 1000 == 0) {
                compactFile();
            }
            
            return true;
        } catch (...) {
            recoverFromError();
            return false;
        }
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(m_fileMutex);
        return m_index.size();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(m_fileMutex);
        return m_index.empty();
    }

private:
    struct MessageHeader {
        uint32_t size;
        uint32_t checksum;
    };

    struct QueueEntry {
        std::streampos position;
        size_t size;
    };

    std::string m_filename;
    mutable std::fstream m_file;
    std::streampos m_readPos;
    std::streampos m_writePos;
    std::deque<QueueEntry> m_index;
    mutable std::mutex m_fileMutex;

    void initialize() {
        // 打开或创建文件
        m_file.open(m_filename, 
                   std::ios::binary | std::ios::in | 
                   std::ios::out | std::ios::app);
        
        if (!m_file.is_open()) {
            // 文件不存在，创建新文件
            m_file.clear();
            m_file.open(m_filename, std::ios::binary | std::ios::out);
            m_file.close();
            m_file.open(m_filename, 
                       std::ios::binary | std::ios::in | 
                       std::ios::out);
        }

        // 重建索引
        rebuildIndex();
    }

    void rebuildIndex() {
        m_file.seekg(0, std::ios::beg);
        m_readPos = 0;
        m_index.clear();

        while (true) {
            MessageHeader header;
            m_file.read(reinterpret_cast<char*>(&header), sizeof(header));
            
            if (!m_file || m_file.eof()) break;

            std::streampos entryPos = m_file.tellg();
            m_index.push_back({entryPos + static_cast<std::streampos>(header.size), 
                              header.size});
            
            m_file.seekg(header.size, std::ios::cur);
            if (!m_file) break;
        }

        m_writePos = m_file.tellp();
    }

    uint32_t calculateChecksum(const void* data, size_t size) const {
        // 简单的校验和计算，可根据需要替换为CRC32等更健壮的算法
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);
        uint32_t checksum = 0;
        for (size_t i = 0; i < size; ++i) {
            checksum += bytes[i];
        }
        return checksum;
    }

    void compactFile() {
        if (m_index.empty()) {
            // 空队列，直接截断文件
            m_file.close();
            m_file.open(m_filename, std::ios::binary | std::ios::out | std::ios::trunc);
            m_file.close();
            m_file.open(m_filename, std::ios::binary | std::ios::in | std::ios::out);
            m_readPos = m_writePos = 0;
            return;
        }

        // 创建临时文件
        std::string tempFilename = m_filename + ".tmp";
        std::ofstream tempFile(tempFilename, std::ios::binary);
        
        if (!tempFile) return;

        // 复制剩余消息到临时文件
        std::streampos newPos = 0;
        std::deque<QueueEntry> newIndex;
        
        for (const auto& entry : m_index) {
            m_file.seekg(entry.position - entry.size - sizeof(MessageHeader));
            
            // 读取消息头和消息体
            MessageHeader header;
            m_file.read(reinterpret_cast<char*>(&header), sizeof(header));
            
            std::vector<char> buffer(header.size);
            m_file.read(buffer.data(), buffer.size());
            
            // 写入临时文件
            tempFile.write(reinterpret_cast<const char*>(&header), sizeof(header));
            tempFile.write(buffer.data(), buffer.size());
            
            // 更新索引
            newPos = tempFile.tellp();
            newIndex.push_back({newPos, header.size});
        }

        tempFile.close();
        m_file.close();

        // 替换文件
        std::remove(m_filename.c_str());
        std::rename(tempFilename.c_str(), m_filename.c_str());

        // 重新打开文件
        m_file.open(m_filename, std::ios::binary | std::ios::in | std::ios::out);
        m_index = std::move(newIndex);
        m_readPos = 0;
        m_writePos = newPos;
    }

    void recoverFromError() {
        try {
            m_file.clear(); // 清除错误状态
            rebuildIndex(); // 重建索引
        } catch (...) {
            // 严重错误，关闭文件
            if (m_file.is_open()) {
                m_file.close();
            }
        }
    }
};


//////////////////////////////////////////
#define __must_check
#define ARRAY_SIZE(ary) (sizeof((ary))/sizeof(*(ary)))
#ifdef __GNUC__
    #define typeof __typeof__
    #define smp_wmb __sync_synchronize
#endif

#define min(x,y) ((x)<(y)?(x):(y))
#define kfifo_size(fifo)	((fifo)->kfifo.mask + 1)

#define DEFAULTSIZE 1 << 3

struct _kfifo {
    unsigned int	in;
    unsigned int	out;
    unsigned int	mask;
    unsigned int	esize;
    void		*data;
};

//#define TRADEFIFOSIZE 1 << 16
constexpr static unsigned int TRADEFIFOSIZE = 1 << 16;  // 或 #define TRADEFIFOSIZE 1024

//template <typename T>
//struct SFIFOBuf {
//    static constexpr unsigned int size = DEFAULTSIZE;
//    struct _kfifo	kfifo;
//    T buf[((size < 2) || (size & (size - 1))) ? -1 : size];
//};

template <typename T, unsigned int Size = DEFAULTSIZE>
struct SFIFOBuf {
    static_assert(Size >= 2 && (Size & (Size - 1)) == 0,
        "Size must be a power of 2 and >= 2");
    struct _kfifo kfifo;
    T buf[Size]; // 直接使用模板参数 Size
};

template <typename T, unsigned int Size = DEFAULTSIZE>
class DHLockFreeQueue {
public:
    using DataType = T;
    using IdxType = unsigned int;
public:
    DHLockFreeQueue() {
       init();
    }

    ~DHLockFreeQueue() {
       delete m_fifobuf;
    }

    bool push(DataType & __val) {
        //return kfifo_in(&__val);
        while (true) {
            if (!kfifo_is_full()) {
                kfifo_in(&__val);
                printf("push value %d\n", (int)(__val.m));
                return true;
            }
            else {
               printf("thread_id %d, elem %d\n", std::this_thread::get_id(), (int)(__val.m));
            }
        }
        return false;
    }




    bool push(DataType && __val) {
        m_value = std::move(__val);
        return kfifo_in(&m_value);
    }

    bool pop(DataType & val) {
       return kfifo_out(&val);
    }

    IdxType size() {
        return kfifo_len();
    }

    IdxType kfifo_unused() {
        return (m_fifobuf->kfifo.mask + 1) - (m_fifobuf->kfifo.in - m_fifobuf->kfifo.out);
    }

private:
    void init() {
        m_fifobuf = new SFIFOBuf<DataType, Size>;
        struct _kfifo *_kfifo = &m_fifobuf->kfifo;
        _kfifo->in = 0;
        _kfifo->out = 0;
        _kfifo->mask = ARRAY_SIZE(m_fifobuf->buf) - 1;
        _kfifo->esize = sizeof(*m_fifobuf->buf);
        // printf("%s:%d esize %d\n", __FUNCTION__, __LINE__, _kfifo->esize);
        _kfifo->data = m_fifobuf->buf;
    }

    IdxType kfifo_avail() {
        unsigned int __avail = kfifo_size(m_fifobuf) - kfifo_len();
        return __avail;
    }

    IdxType kfifo_in(DataType * p, int size = 1) {
        unsigned long _n = size;
        struct _kfifo *_kfifo = &m_fifobuf->kfifo;
        return _kfifo_in(_kfifo, p, _n);
    }

    IdxType kfifo_out(DataType * p, int size = 1) {
        unsigned long __n = size;
        struct _kfifo *_kfifo = &m_fifobuf->kfifo;
        return _kfifo_out(_kfifo, p, __n);
    }

    bool kfifo_is_full() {
        return kfifo_len() > m_fifobuf->kfifo.mask;
    }

    bool push(DataType * elem) {
        while (true) {
            if (!kfifo_is_full()) {
                kfifo_in(elem);
                printf("push value %d\n", (int)(elem->m));
                return true;
            }
            else {
               printf("thread_id %d, elem %d\n", std::this_thread::get_id(), (int)(elem->m));
            }
        }
        return false;
    }

private:
        IdxType _kfifo_max_r(IdxType len, size_t recsize)
        {
            unsigned int max = (1 << (recsize << 3)) - 1;

            if (len > max)
                return max;
            return len;
        }

        IdxType _kfifo_in(struct _kfifo *fifo, const void *buf, IdxType len){
            unsigned int l = kfifo_unused();
            if (len > l)
                len = l;

            kfifo_copy_in(fifo, buf, len, fifo->in);
            fifo->in += len;
            return len;
        }

        IdxType _kfifo_in_r(struct _kfifo *fifo, const void *buf, IdxType len, size_t recsize) {
            if (len + recsize > kfifo_unused())
                return 0;

            _kfifo_poke_n(fifo, len, recsize);

            kfifo_copy_in(fifo, buf, len, fifo->in + recsize);
            fifo->in += len + recsize;
            return len;
        }

        IdxType _kfifo_out(struct _kfifo *fifo, void *buf, IdxType len = 1) {
            //printf("%s:%d, fifo->in %d, fifo->out %d, len %d\n", __FUNCTION__, __LINE__, fifo->in, fifo->out, len);
            len = _kfifo_out_peek(fifo, buf, len);
            fifo->out += len;
            //printf("%s:%d, fifo->out %d, len %d\n", __FUNCTION__, __LINE__, fifo->out, len);
            return len;
        }

        IdxType _kfifo_out_r(struct _kfifo *fifo, void *buf, IdxType len, size_t recsize) {
            unsigned int n;

            if (fifo->in == fifo->out)
                return 0;

            len = kfifo_out_copy_r(fifo, buf, len, recsize, &n);
            fifo->out += n + recsize;
            return len;
        }

        IdxType kfifo_len() {
            return m_fifobuf->kfifo.in - m_fifobuf->kfifo.out;
        }

private:
    unsigned int _kfifo_out_peek(struct _kfifo *fifo,
        void *buf, unsigned int len)
    {
        unsigned int l;

        l = fifo->in - fifo->out;
        if (len > l)
            len = l;

        kfifo_copy_out(fifo, buf, len, fifo->out);
        return len;
    }

    unsigned int _kfifo_out_peek_r(struct _kfifo *fifo, void *buf,
        unsigned int len, size_t recsize)
    {
        unsigned int n;

        if (fifo->in == fifo->out)
            return 0;

        return kfifo_out_copy_r(fifo, buf, len, recsize, &n);
    }

    static unsigned int kfifo_out_copy_r(struct _kfifo *fifo,
    void *buf, unsigned int len, size_t recsize, unsigned int *n)
    {
        *n = _kfifo_peek_n(fifo, recsize);

        if (len > *n)
            len = *n;

        kfifo_copy_out(fifo, buf, len, fifo->out + recsize);
        return len;
    }

    static void kfifo_copy_out(struct _kfifo *fifo, void *dst,
        unsigned int len, unsigned int off)
    {
        unsigned int size = fifo->mask + 1;
        unsigned int esize = fifo->esize;
        unsigned int l;

        off &= fifo->mask;
        if (esize != 1) {
            off *= esize;
            size *= esize;
            len *= esize;
        }
        l = min(len, size - off);
        //printf("%s:%d off %d, size %d, len %d, esize %d\n", __FUNCTION__, __LINE__, off, size, len, esize);
        memcpy(dst, (char *)fifo->data + off, l);
        memcpy((char *)dst + l, fifo->data, len - l);
        //auto d = (testS*)dst;
        //printf("%s:%d d->m %d, d->n %d\n", __FUNCTION__, __LINE__ ,d->m, d->n);
        /*
        * make sure that the data is copied before
        * incrementing the fifo->out index counter
        */
        smp_wmb();
        // c++ style
        //std::atomic_thread_fence(std::memory_order_seq_cst);
    }
    static unsigned int _kfifo_peek_n(struct _kfifo *fifo, size_t recsize)
    {
        unsigned int l;
        unsigned int mask = fifo->mask;
        unsigned char *data = (unsigned char*)(fifo->data);

        l = (data)[(fifo->out) & (mask)];
        if (--recsize)
            l |= (data)[(fifo->out + 1) & (mask)] << 8;

        return l;
    }

    static void kfifo_copy_in(struct _kfifo *fifo, const void *src,
        unsigned int len, unsigned int off) {
        unsigned int size = fifo->mask + 1;
        unsigned int esize = fifo->esize;
        unsigned int l;

        off &= fifo->mask;
        if (esize != 1) {
            off *= esize;
            size *= esize;
            len *= esize;
        }
        l = min(len, size - off);

        memcpy((char *)fifo->data + off, src, l);
        memcpy(fifo->data, (char *)src + l, len - l);
        /*
         * make sure that the data in the fifo is up to date before
         * incrementing the fifo->in index counter
         */
        smp_wmb();
    }

    /*
 * _kfifo_poke_n internal helper function for storeing the length of
 * the record into the fifo
 */
    static void _kfifo_poke_n(struct _kfifo *fifo, unsigned int n, size_t recsize)
    {
        unsigned int mask = fifo->mask;
        unsigned char *data = (unsigned char *)fifo->data;

        (data)[(fifo->in) & (mask)] = (unsigned char)(n);

        if (recsize > 1)
            (data)[(fifo->in + 1) & (mask)] = (unsigned char)(n >> 8);
    }

private:
    SFIFOBuf<DataType, Size> * m_fifobuf;
    DataType m_value;
};



////////////////////////////////////////////////////////// lock-free queue base on object pool ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
#include <atomic>
#include <memory>
#include <vector>
#include <mutex>

constexpr size_t DEFAULT_POOL_SIZE = 1 << 16;

template <typename T, size_t PoolSize = DEFAULT_POOL_SIZE>
class ObjectPool {
private:
    struct alignas(64) PoolNode {
        T data;
        std::atomic<PoolNode*> next;
    };

    std::atomic<PoolNode*> free_list{nullptr};
    std::vector<PoolNode*> allocated_nodes;
    std::vector<char*> allocated_blocks;
    std::mutex allocation_mutex;
    size_t initial_size;

public:
    explicit ObjectPool(bool is_spsc = true) : initial_size(PoolSize) {
        preallocate(PoolSize, true);
    }

    ~ObjectPool() {
        for (auto block : allocated_blocks) {
            delete[] block;
        }
    }

    void preallocate(size_t count, bool is_spsc = true) {
        if (count == 0) return;
        
        char* raw_memory = new char[count * sizeof(PoolNode)];
        PoolNode* nodes = reinterpret_cast<PoolNode*>(raw_memory);
        
        std::unique_lock<std::mutex> lock;
        if (!is_spsc) {
            lock = std::unique_lock<std::mutex>(allocation_mutex);
        }
        allocated_blocks.push_back(raw_memory);
        allocated_nodes.reserve(allocated_nodes.size() + count);
        
        for (size_t i = 0; i < count - 1; ++i) {
            new (&nodes[i]) PoolNode();
            nodes[i].next.store(&nodes[i+1], std::memory_order_relaxed);
            allocated_nodes.push_back(&nodes[i]);
        }
        
        new (&nodes[count-1]) PoolNode();
        nodes[count-1].next.store(free_list.load(std::memory_order_relaxed), 
                                std::memory_order_relaxed);
        allocated_nodes.push_back(&nodes[count-1]);
        
        // 将新分配的内存块链接到空闲列表的正确方式:
        // 1. 首先获取当前空闲列表头部
        PoolNode* old_head = free_list.load(std::memory_order_relaxed);
    
        // 2. 将新块的最后一个节点指向原空闲列表头部
        nodes[count-1].next.store(old_head, std::memory_order_relaxed);
    
        // 3. 将空闲列表头部指向新块的第一个节点
        // SPSC模式可以使用更宽松的内存序
        auto memory_order = is_spsc ? std::memory_order_relaxed : std::memory_order_release;
        free_list.store(&nodes[0], memory_order);

    }


    T* allocate(bool is_spsc = true) {
        // 尝试从空闲列表获取
        PoolNode* node = free_list.load(std::memory_order_acquire);
        while (node) {
            PoolNode* next = node->next.load(std::memory_order_relaxed);
            if (free_list.compare_exchange_weak(node, next, 
                                             std::memory_order_acq_rel)) {
                return &node->data;
            }
        }
        // 分配新节点并自动扩容
        //std::lock_guard<std::mutex> lock(allocation_mutex);

        std::unique_lock<std::mutex> lock;
        if (!is_spsc) {
            lock = std::unique_lock<std::mutex>(allocation_mutex);
        }
    
        // 计算新的扩容大小(当前已分配节点的2倍，至少为initial_size)
        size_t new_allocation_size = allocated_nodes.empty() ? 
                               initial_size : 
                               allocated_nodes.size() * 2;
    
        printf("%s:%d new size %d\n", __FUNCTION__, __LINE__,new_allocation_size);
        // 预分配新内存块
        preallocate(new_allocation_size);
    
        // 现在freelist已经有新节点了，再次尝试获取
        node = free_list.load(std::memory_order_acquire);
        while (true) {
            PoolNode* next = node->next.load(std::memory_order_relaxed);
            if (free_list.compare_exchange_weak(node, next,
                                          std::memory_order_acq_rel)) {
                return &node->data;
            }
        }
    }

    void deallocate(T* ptr) {
        if (!ptr) return;
        
        PoolNode* node = reinterpret_cast<PoolNode*>(ptr);
        PoolNode* old_head = free_list.load(std::memory_order_relaxed);
        node->next.store(old_head, std::memory_order_relaxed);
        
        PoolNode * expected = old_head;
        while (!free_list.compare_exchange_weak(expected, node,
                                              std::memory_order_release,
                                              std::memory_order_relaxed)) {
          node->next.store(expected, std::memory_order_relaxed);
        }
    }
};

template <typename T, bool SPSC_MODE = true>
class OptimizedLockFreeQueue {
public:
    using DataType = T;
    
    OptimizedLockFreeQueue(size_t initial_size = DEFAULT_POOL_SIZE) {
        init();
    }

    ~OptimizedLockFreeQueue() {
        Node* curr = head.load(std::memory_order_relaxed);
        while (curr) {
            Node* next = curr->next.load(std::memory_order_relaxed);
            if (curr != dummy_node) {
                pool.deallocate(curr);
            }
            curr = next;
        }
        pool.deallocate(dummy_node);
    }

    bool push(const DataType& val) {
        Node* new_node = pool.allocate();
        new (new_node) Node();
        new_node->data = val;
        new_node->next.store(nullptr, std::memory_order_relaxed);

        Node* old_tail = tail.load(std::memory_order_acquire);
        Node* null_ptr = nullptr;

        while (!old_tail->next.compare_exchange_weak(
            null_ptr, new_node, 
            std::memory_order_release,
            std::memory_order_relaxed)) {
            null_ptr = nullptr;
            old_tail = tail.load(std::memory_order_acquire);
        }

        tail.compare_exchange_strong(old_tail, new_node,
                                   std::memory_order_release);
        count.fetch_add(1, std::memory_order_relaxed);
        return true;
    }

    bool pop(DataType& val) {
        if constexpr (SPSC_MODE) {
            return pop_spsc(val);
        } else {
            std::lock_guard<std::mutex> lock(pop_mutex);
            return pop_locked(val);
        }
    }

    bool empty() const {
        return count.load(std::memory_order_relaxed) == 0;
    }

    size_t size() const {
        return count.load(std::memory_order_relaxed);
    }

    bool push_bulk(const DataType* values, size_t c) {
        if (c == 0) return true;
        
        std::vector<Node*> nodes(c);
        for (size_t i = 0; i < c; ++i) {
            nodes[i] = pool.allocate();
            new (nodes[i]) Node();
            nodes[i]->data = values[i];
            nodes[i]->next.store(nullptr, std::memory_order_relaxed);
        }
        
        for (size_t i = 0; i < c - 1; ++i) {
            nodes[i]->next.store(nodes[i+1], std::memory_order_relaxed);
        }
        
        Node* old_tail = tail.load(std::memory_order_acquire);
        Node* null_ptr = nullptr; 
        
        while (!old_tail->next.compare_exchange_weak(
            null_ptr, nodes[0], 
            std::memory_order_release,
            std::memory_order_relaxed)) {
            null_ptr = nullptr;
            old_tail = tail.load(std::memory_order_acquire);
        }
        
        tail.compare_exchange_strong(old_tail, nodes[c-1],
                                   std::memory_order_release);
        count.fetch_add(c, std::memory_order_relaxed);
        return true;
    }

private:
    struct alignas(64) Node {
        DataType data;
        std::atomic<Node*> next;
    };

    std::atomic<Node*> head;
    std::atomic<Node*> tail;
    std::atomic<size_t> count{0};
    ObjectPool<Node> pool;
    Node* dummy_node;
    std::mutex pop_mutex;

    bool pop_spsc(DataType& val) {
        Node* current_head = head.load(std::memory_order_relaxed);
        Node* next_node = current_head->next.load(std::memory_order_acquire);
        
        if (!next_node) return false; // 队列为空
        
        val = next_node->data;
        head.store(next_node, std::memory_order_release);
        
        // 将旧头节点放回内存池
        pool.deallocate(current_head);
        count.fetch_sub(1, std::memory_order_relaxed);
        return true;
    }

    bool pop_locked(DataType& val) {
        Node* old_head = head.load(std::memory_order_relaxed);
        Node* old_tail = tail.load(std::memory_order_relaxed);
        Node* next = old_head->next.load(std::memory_order_acquire);

        if (old_head == old_tail) {
            if (next == nullptr) return false; // 队列空
            
            // 帮助更新tail指针
            tail.compare_exchange_strong(old_tail, next,
                                       std::memory_order_release);
            return pop_locked(val); // 重试
        }

        val = next->data;
        head.store(next, std::memory_order_release);
        pool.deallocate(old_head);
        count.fetch_sub(1, std::memory_order_relaxed);
        return true;
    }

    void init() {
        dummy_node = pool.allocate();
        dummy_node->next.store(nullptr, std::memory_order_relaxed);
        head.store(dummy_node, std::memory_order_release);
        tail.store(dummy_node, std::memory_order_release);
    }
};



#endif _DH_QUEUE_H_
#include <iostream>
#include "../gtest/include/gtest/gtest.h"
#include "dh_queue.h"

struct CTest {
    int m{0};
    int n{0};
};

void Enqueue(OptimizedLockFreeQueue<CTest> * queue) {
    for (auto i = 0; i < 100; ++i) {
        CTest t;
        t.m = i;
        if (queue->push(t) == false) {
            std::cout << "enque error " << i <<  std::endl;
        }
    }
}

void Dequeue(OptimizedLockFreeQueue<CTest> * queue) {
    CTest t;
    while (auto p = queue->pop(t)) {
        std::cout << "deque " << t.m << std::endl;
    }
}

// TEST(TestOtherCase, test_queue) {
//     DHLockFreeQueue<CTest> * queue = new DHLockFreeQueue<CTest>();
//     std::thread t1(Enqueue, queue);
//     sleep(2);
//     std::thread t2(Dequeue, queue);

//     t1.join();
//     t2.join();
//     //FilePersistentQueue fq("queue.txt");
// }

// #include <iostream>
// #include <thread>
// #include <vector>

// struct TestData {
//     int id;
//     double value;
// };

// void producer(EnhancedLockFreeQueue<TestData>& queue, int id) {
//     for (int i = 0; i < 1000; ++i) {
//         TestData data{id * 1000 + i, 3.14 * i};
//         while (!queue.push(data)) {
//             std::this_thread::yield();
//         }
//     }
// }

// void consumer(EnhancedLockFreeQueue<TestData>& queue, std::atomic<int>& count) {
//     TestData data;
//     while (count.load() < 4000) {
//         if (queue.pop(data)) {
//             count.fetch_add(1);
//             // 模拟处理
//             std::this_thread::sleep_for(std::chrono::microseconds(10));
//         } else {
//             std::this_thread::yield();
//         }
//     }
// }

// int main() {
//     EnhancedLockFreeQueue<TestData> queue;
//     std::atomic<int> processed{0};

//     std::vector<std::thread> producers;
//     std::vector<std::thread> consumers;

//     // 4个生产者
//     for (int i = 0; i < 4; ++i) {
//         producers.emplace_back(producer, std::ref(queue), i);
//     }

//     // 2个消费者
//     for (int i = 0; i < 2; ++i) {
//         consumers.emplace_back(consumer, std::ref(queue), std::ref(processed));
//     }

//     for (auto& t : producers) t.join();
//     for (auto& t : consumers) t.join();

//     std::cout << "Total processed: " << processed.load() << std::endl;
//     std::cout << "Final queue size: " << queue.size() << std::endl;

//     return 0;
// }

void test_latency(OptimizedLockFreeQueue<CTest> * queue, int num_operations) {
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_operations; ++i) {
        CTest s;
        s.m = i;
        queue->push(s);
        CTest ss;
        bool f = queue->pop(ss);
        //printf("%s:%d out value %d\n", __FUNCTION__, __LINE__, ss.m);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "New Atomic Latency: " << duration.count() / num_operations * 1e6 << " us/op\n";
}

void multi_test_throughput(OptimizedLockFreeQueue<CTest> * queue, int num_threads, int num_operations) {
    std::vector<std::thread> threads;
    std::atomic<int> ops(0);

    auto start = std::chrono::high_resolution_clock::now();
    printf("start to loop\n");
    for (int i = 0; i < num_threads; ++i) {
        if (i % 2 != 0) {
            threads.emplace_back([&ops, queue, num_operations]() {
                CTest s;
                for (int j = 0; j < num_operations;) {
                    s.m = j;
                    if (queue->push(s)) {
                        //printf("write %d\n", j);
                        ops.fetch_add(1, std::memory_order_relaxed);
                        j++;
                    } else {
                        //printf("write error %d\n", j);
                    }
                    
                }
            });
        } else {
            threads.emplace_back([&ops, queue, num_operations]() {
                CTest s;
                for (int j = 0; j < num_operations;) {
                    if (queue->pop(s)) {
                        ops.fetch_add(1, std::memory_order_relaxed);
                        //printf("read %d\n", s.m);
                        j++;
                    } else {
                        //printf("read error %d\n", j);
                    }
                }
            });
        }
    }
    for (auto& t : threads) {
        t.join();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "new queue throughput: " << ops.load() / duration.count() << " ops/s\n";
}

constexpr int OPTIONS = 1000000;

int main (int argc, char** argv) {
    // testing::InitGoogleTest(&argc,argv);
    // return RUN_ALL_TESTS();

    //DHLockFreeQueue<CTest> * queue = new DHLockFreeQueue<CTest>();

    OptimizedLockFreeQueue<CTest> * queue = new OptimizedLockFreeQueue<CTest>();
    //
    test_latency(queue, OPTIONS);
    //
    multi_test_throughput(queue, 2, OPTIONS);

    // OptimizedLockFreeQueue<CTest> * queue = new OptimizedLockFreeQueue<CTest>();
    // std::thread t1(Enqueue, queue);
    // //sleep(2);
    // std::thread t2(Dequeue, queue);

    // t1.join();
    // t2.join();

    return 0;
}
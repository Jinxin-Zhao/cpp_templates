#ifndef _CHAPTER_19_H_
#define _CHAPTER_19_H_

// type traits
template <typename T>
struct AccumulationTraits;

// c++中只允许对在类中对一个整型和枚举类型的static const进行初始化
template<>
struct AccumulationTraits<char>{
    using AccT = int;
    // value traits
    static AccT const zero = 0;
};

template<>
struct AccumulationTraits<short> {
    using AccT = int;
    static AccT const zero = 0;
};

template<>
struct AccumulationTraits<int> {
    using AccT = int;
    static AccT const zero = 0;
};

template<>
struct AccumulationTraits<unsigned int> {
    using AccT = unsigned long;
    static AccT const zero = 0;
};

// constexpr好一点，可以对float和其他字面值类型进行类内初始化
template<>
struct AccumulationTraits<float> {
    using AccT = float;
    static constexpr float zero = 0.0f;
};

// 对于自定义的非字面值如
class BigInt{
public:
    BigInt(long long){}
};

template<>
struct AccumulationTraits<BigInt> {
    using AccT = BigInt;
    // error
    //static constexpr BigInt zero = BigInt{0};
    // solution: add inline keywords
    inline static BigInt const zero = BigInt{0};
};

template<typename T, typename AT = AccumulationTraits<T>>
auto accum(T * begin, T * end) {
    // using AccT = typename AccumulationTraits<T>::AccT;
    // AccT total =  AccumulationTraits<T>::zero;
    typename AT::AccT total = AT::zero;
    while (begin != end) {
        total += *begin;
        ++begin;
    }
    return total;
}

// traits policies

#endif


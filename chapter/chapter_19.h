#ifndef _CHAPTER_19_H_
#define _CHAPTER_19_H_
#include <vector>
#include <list>

// type traits
template <typename T>
struct AccumulationTraits;

// c++中只允许在类中对一个整型和枚举类型的static const进行初始化
template<>
struct AccumulationTraits<char>{
    using AccT = int;
    // value traits
    // static AccT const zero = 0;
    // rewrite with inline member functions for value traits, such a function can be decalred 'constexpr' if it returns a literal type
    static constexpr AccT zero() { return 0; }
};

template<>
struct AccumulationTraits<short> {
    using AccT = int;
    // static AccT const zero = 0;
    static constexpr AccT zero() { return 0; }
};

template<>
struct AccumulationTraits<int> {
    using AccT = int;
    // static AccT const zero = 0;
    static constexpr AccT zero() { return 0; }
};

template<>
struct AccumulationTraits<unsigned int> {
    using AccT = unsigned long;
    // static AccT const zero = 0;
    static constexpr AccT zero() { return 0; }
};

template<>
struct AccumulationTraits<float> {
    using AccT = float;
    // static constexpr float zero = 0.0f;
    static constexpr AccT zero() { return 0; }
};

// a user-defined arbitrary-precision 'BigInt' type might not be a literal type
// typically it has to allocate components on the heap
class BigInt{
public:
    BigInt(long long){
        // allocate some space on heap ...
    }
};

template<>
struct AccumulationTraits<BigInt> {
    using AccT = BigInt;
    // error
    //static constexpr BigInt zero = BigInt{0}; // Error: not a literal type

    // solution: add inline keywords
    // inline static BigInt const zero = BigInt{0}; // OK since C++17

    static BigInt zero() { return BigInt{0}; }
};

// template<typename T, typename AT = AccumulationTraits<T>>
// auto accum(T * begin, T * end) {
//     // using AccT = typename AccumulationTraits<T>::AccT;
//     // AccT total =  AccumulationTraits<T>::zero;
//     typename AT::AccT total = AT::zero();
//     while (begin != end) {
//         total += *begin;
//         ++begin;
//     }
//     return total;
// }


/** Traits versus Policies and Policy classes */

class SumPolicy {
public:
    template <typename T1, typename T2>
    static void accumulate(T1 & total, T2 const & value) {
        total += value;
    }
};

// by specifying a different policy to accumulate values, we can compute different things.
class MultPolicy {
public:
    template <typename T1, typename T2>
    static void accumulate(T1 & total, T2 const & value) {
        total *= value;
    }
};


template <typename T1, typename T2>
class SumPolicyTpp {
public:
    static void accumulate(T1 & total, T2 const & value) {
        total += value;
    }
};


// c++ standard lib takes initial value as a third (function call) arg
template<typename T, 
    typename Policy = SumPolicy,
    typename Traits = AccumulationTraits<T>>
auto accum(T const * begin, T const * end) {
    using AccT = typename Traits::AccT;
    AccT total = Traits::zero();
    while (begin != end) {
        Policy::accumulate(total, *begin);
        ++begin;
    }
    return total;
}

// 19.2.2 member templates versus template template parameters 
// advantage of accessing policy classes through template template
// parameters is that it makes it easier to have a policy class carry with it some state information (i.e., static data members)
template<typename T, 
    template<typename, typename> class Policy = SumPolicyTpp,
    typename Traits = AccumulationTraits<T>>
auto accum_tpp(T const * begin, T const * end) {
    using AccT = typename Traits::AccT;
    AccT total = Traits::zero();
    while (begin != end) {
        Policy<AccT, T>::accumulate(total, *begin);
        ++begin;
    }
    return total;
}

// 19.2.4 accumulation with general iterators
#include <iterator>

template <typename Iter>
auto accumIter(Iter start, Iter end) {
    using VT = typename std::iterator_traits<Iter>::value_type;
    VT total{};
    while (start != end) {
        total += * start;
        ++start;
    }
    return total;
}

// 19.3 type functions
template <typename T>
struct TypeSize {
    static std::size_t const value = sizeof(T);
};
// use: TypeSize<int>::value

template <typename T>
struct ElementT; // primary template

template <typename T>
struct ElementT<std::vector<T>> {  // partial specialization for vector
    using Type = T;
};

template <typename T>
struct ElementT<std::list<T>> {
    using Type = T;
};


template <typename T, std::size_t N>
struct ElementT<T[N]> {  // partial specialization for vector
    using Type = T;
};

template <typename T>
struct ElementT<T[]> {
    using Type = T;
};

// use
template <typename T>
void printElemType (T const & c) {
    std::cout << "Container of " << typeid(typename ElementT<T>::Type).name() << " elements. \n";
}

#endif
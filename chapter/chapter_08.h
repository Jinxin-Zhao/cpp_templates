#ifndef _CHAPTER_08_H_
#define _CHAPTER_08_H_

// 8.1 metaprogramming
// calculate in compile time

template <unsigned p, unsigned d>
struct DoIsPrime {
    static constexpr bool value = (p % d != 0) && DoIsPrime<p, d-1>::value;
};

template<unsigned p>
struct DoIsPrime<p, 2> {
    static constexpr bool value = (p % 2 != 0);
};

// primary template
template <unsigned p>
struct IsPrime {
    static constexpr bool value = DoIsPrime<p, p/2>::value;
};

template <>
struct IsPrime<0> {static constexpr bool value = false;};
template <>
struct IsPrime<1> {static constexpr bool value = false;};
template <>
struct IsPrime<2> {static constexpr bool value = true;};
template <>
struct IsPrime<3> {static constexpr bool value = true;};

// constexpr: c++11中constexpr限制词限了只能包含一句return，c++14后开始这个限制被移除
// 目前堆内存分配和异常抛出在constexpr函数内都不被支持


/////// 8.4 SFINAE
// 函数模板参数类型是T(&)[N]
template <typename T, unsigned N>
std::size_t f_len(T (&)[N]) {
    return N;
}

template <typename T>
typename T::size_type f_len(const T & t) {
    std::cout << "func name f_len(const T & t)  " << std::endl;
    return t.size();
}

template <typename ... T>
std::size_t f_len(T...) {
    return 0;
}

// 编译期if，利用SFINAE和std::enable_if来禁用或启用某个模板

#endif
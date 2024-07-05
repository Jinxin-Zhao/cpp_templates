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


// “我们SFINAE掉了一个函数”意思是我们通过模板在一些限制条件下产生无效代码，从而确保在这些条件下忽略掉该模板
// 如下面这个例子
class selfThread {
public:
    //确保在使用一个selfThread对象构造另一个selfThread时，下面的构造函数被SFINAE掉了，从而调用默认的copy构造函数
    template <typename F, typename... Args, typename=std::enable_if_t<!std::is_same_v<std::decay_t<F>, selfThread>>>
    explicit selfThread(F && f, Args&& ... args) {
    }
};

// 编译期if，利用SFINAE和std::enable_if来禁用或启用某个模板
template <typename T, typename... Types>
void print(const T & firstArg, Types const &... args) {
    std::cout << firstArg << '\n';
    // since c++17
    if constexpr(sizeof...(args) > 0) {
        // code only available if sizeof...(args)>0
        print(args...);
    } 
}


#endif
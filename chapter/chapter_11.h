#ifndef _CHAPTER_09_H_
#define _CHAPTER_09_H_

#include <iostream>
#include <vector>
#include <utility>
#include <functional>
// 11. template lib
// 在实现for_each过程中,会使用到模板
// template <typename Iter, typename Callable>
// void foreach(Iter current, Iter end, Callable op) {
//     while (current != end) {
//         op(*current);
//         ++current;
//     }
// }

void func(int i) {
    std::cout << "func() called for: " << i << std::endl;
}

class FuncObj {
public:
    void operator() (int i) const {
        std::cout << "FuncObj::op() called for: " << i << std::endl;
    }
};

class MyClass {
public:
    void memfunc(int i) const {
        std::cout << "MyClass::memfunc() called for " << i << std::endl;
    }
};

// 但是如果可调用的对象是一个成员函数，应该怎么实现
template <typename Iter, typename Callable, typename... Args>
void foreach(Iter current, Iter end, Callable op, Args const&... args) {
    while (current != end) {
        // std::invoke作用:
        // 1. 如果可调用对象是一个指向成员函数的指针，它会将args...中第一个参数当作this对象，Args...中其余参数则被当作常规参数传递给可调用对象。
        // 2. 否则所有参数都被直接传递给可调用对象。
        std::invoke(op, args..., *current);
        ++current;
    }
}

// 11.1.3 function wrap
template <typename Callable, typename... Args>
decltype(auto) call_inv(Callable && op, Args&&... args) {
    // return std::invoke(std::forward<Callable>(op), std::forward<Args>(args)...); 
    // or
    // decltype(auto) ret{std::invoke(std::forward<Callable>(op), std::forward<Args>(args)...)};
    // return ret;

    // 如果返回值是void，就不能像上述写法这么写，因为void是不完全类型，不能将ret初始化为decltype(auto)。
    if constexpr(std::is_same_v<std::invoke_result_t<Callable, Args>, void>) {
        std::invoke(std::forward<Callable>(op), std::forward<Args>(args)...);
        return;
    } else {
        decltype(auto) ret{std::invoke(std::forward<Callable>(op), std::forward<Args>(args)...)};
        return ret;
    }
}




#endif
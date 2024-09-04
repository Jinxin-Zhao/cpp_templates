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
    // if constexpr(std::is_same_v<std::invoke_result_t<Callable, Args>, void>) {
    //     //std::invoke(std::forward<Callable>(op), std::forward<Args>(args)...);
    //     return;
    // } else {
        decltype(auto) ret{std::invoke(std::forward<Callable>(op), std::forward<Args>(args)...)};
        return ret;
    // }
}

// 11.2.1 类型萃取
#include <type_traits>
template <typename T>
class CTypeTraits {
    // ensure that T is not void
    static_assert(!std::is_same_v<std::remove_cv_t<T>, void>, "invalid instantiation of class CTypeTraits for void type");
public:
    template <typename V>
    void f(V &&v) {
        if constexpr(std::is_reference_v<T>) {
            // if T is a reference type
        }
        if constexpr(std::is_convertible_v<std::decay_t<V>, T>) {
            // if T is convertible to T: int const && value = 1;
            std::add_rvalue_reference_t<int const> value = 1;
        }
        if constexpr(std::has_virtual_destructor_v<V>) {
            // if V has virtual destructor
        }
    }
};

// std::remove_const_t<int const&> : 结果是int const&,因为引用不是const
// 所以删除引用和删除const顺序很重要
// std::remove_const_t<std::remove_reference_t<int const &>> :   int
// std::remove_reference_t<std::remove_const_t<int const &>> :   int const
// 或者直接使用std::decay<int const &> : int

// add reference
//add_rvalue_reference_t<int const> : int const &&
//add_rvalue_reference_t<int const &>: // due to && collapse, => int const &

// copy asignable
// is_copy_assignable_v<int> // yields true (you can assgin an int to an int), 通常只会检查左值相关操作
// equal to is_assignable_v<int&, int&>

// 11.2.2 std::addressoff()
template <typename T>
void addr_f(T && x) {
    auto p = &x;
    auto q = std::addressof(x); // 返回一个对象或函数的准确地址
}


// 11.2.3
// std::declval可以被用作某一类型对象的的引用的占位符
template <typename T1, typename T2, typename RT = std::decay_t<decltype(true ? std::declval<T1>() : std::declval<T2>())>>
RT r_max(T1 a, T2 b) {
    return b < a ? a : b;
}

//
template <typename T>
void tmplParamIsReference(T) {
    std::cout << "T is reference: " << std::is_reference_v<T> << std::endl;
}

// 11.5 defer evaluation
template <typename T>
class Cont {
public:

    // error: std::is_move_constructible要求参数必须是完整类型，如果用Node调用就像 Cont<Node> next语句，此时Node还没定义完，属不完整类型
    // typename std::conditional<std::is_move_constructible<T>::value, T&&, T&>::type foo() {
    // }

    // 类型萃取依赖于模板参数D(默认值是T),并且编译器会一直等到foo被以完整类型(如Node)为参数调用时，
    // 才会对类型萃取部分进行计算(此时Node是一个完整类型,其只在定义时才是非完整类型)
    template <typename D = T>
    typename std::conditional<std::is_move_constructible<D>::value, T&&, T&>::type foo() {
    }
private:
    T * elem;
};

struct Node {
    std::string value;
    Cont<Node> next;
};

#endif
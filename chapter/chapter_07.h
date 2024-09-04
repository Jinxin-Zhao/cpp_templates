#ifndef _CHAPTER_07_H_
#define _CHAPTER_07_H_

#include <functional>  // for std::cref()
#include <string>
#include <iostream>
// 1.6.1节中建议在函数模板中应该优先使用按值传递，除非遇到以下情况：
// - 对象不允许被copy
// 参数被用于返回数据
// 参数以及其所有属性需要被模板转发到别的地方
// 可以获得明显的性能提升

// passed by value results in type decay
template <typename T>
void CNormalValueTemplateFunc(T arg){
}

// passed by reference
template <typename T>
void CReferenceTemplateFunc(T const & arg) {

}

// passed by non-const reference
template <typename T>
void CReferenceNonConstFunc(T & arg) {
    if (std::is_array<T>::value) {
        std::cout << "got array of " << std::extent<T>::value << " elems "<< std::endl;
    }
}

// solution
// template <typename T, typename = std::enable_if_t<!std::is_const<T>::value>>
// void CReferenceNonConstFunc(T & arg) {
//     if (std::is_array<T>::value) {
//         std::cout << "got array of " << std::extent<T>::value << " elems "<< std::endl;
//     }
// }
// 另一种写法
// template <typename T>
// requires !std::is_const_v<T>
// void CReferenceNonConstFunc(T & arg){}

// passed by double reference
template <typename T>
void passR(T && arg) {

}


// std::ref() & std::cref() 仅限于模板参数传递,如std::bind等
void printString(std::string const & s) {
    std::cout << s << '\n';
}

template <typename T>
void printT(T arg) {
    printString(arg); // might convert arg back to std::string
}

// 返回引用
// 当调用的时候传递的是lvalue，T deduced as 一个引用类型如int&
template <typename T>
T retR(T && p) {
    return T{2};
}
// int x;
// 当调用的时候retV<int &>(x);
template <typename T>
T retV(T p){

}

// 如何解决让函数不返回引用
template <typename T>
typename std::remove_reference<T>::type returnV(T p){}

// or 利用auto会导致类型退化的机制
template <typename T>
auto au_returnV(T p){}


#endif
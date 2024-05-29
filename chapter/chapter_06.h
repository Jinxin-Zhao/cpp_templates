#ifndef _CHAPTER_06_H_
#define _CHAPTER_06_H_

#include <utility>
#include <string>
#include <iostream>

// perfect forward
void g(int & x) {
    std::cout << "call g(int &x)" << std::endl;
}

void g(int const & x) {
    std::cout << "call g(int const & x)" << std::endl;
}

void g(int &&) {
    std::cout << "call g(int &&x)" << std::endl;
}

template <typename T>
void f(T && val) {
    g(std::forward<T>(val));
}


// special member function template
class CPerson {
public:
    explicit CPerson(std::string const & n) : name(n) {
        std::cout << "CPerson(const string &) " << std::endl;
    }
    explicit CPerson(std::string && n) : name(std::move(n)) {
        std::cout << "CPerson(string &&)" << std::endl;
    }
    CPerson(CPerson const & p) : name(p.name) {
        std::cout << "CPerson(CPerson const &) " << std::endl;
    }
    CPerson(CPerson && p) : name(std::move(p.name)) {
        std::cout << "CPerson(CPerson &&)" << std::endl;
    }
private:
    std::string name;
};

class CTPPerson {
public:
    template <typename STR>
    explicit CTPPerson(STR && n) : name(std::forward<STR>(n)) {
        std::cout << "TMPL constructor" << std::endl;
    }
    CTPPerson(CTPPerson const & p) : name(p.name) {
        std::cout << "CTPPerson(CTPPerson const &) " << std::endl;
    }
    CTPPerson(CTPPerson && p) : name(std::move(p.name)) {
        std::cout << "CTPPerson(CTPPerson &&)" << std::endl;
    }
private:
    std::string name;
};

// enable_if
// enable_if 会根据第一个模板参数编译期表达式决定其行为：
// 如果表达式结果为true，结果会返回一个类型
//       - 如果没有第二个模板参数，返回void
//       - 否则，返回类型是第二个参数类型
// 如果表达式false，则成员类型是未定义的，根据SFINAE规则，会导致包含std::enable_if<>表达式的函数模板被忽略

// template <typename T>
// std::enable_if_t<(sizeof(T) > 4), T> foo() {
//     return T();
// }
// 以上表达式演变成
template <typename T, typename = std::enable_if_t<(sizeof(T) > 4)>>
void foo(){
}
// 也可以用别名模板
template <typename T>
using EnableIfGreater4 = std::enable_if_t<(sizeof(T) > 4)>;

// use enable_if
template <typename T>
using EnableIfString = std::enable_if_t<std::is_convertible_v<T, std::string>>;

class FinalPerson {
public:
    template <typename STR, typename = EnableIfString<STR>>
    explicit FinalPerson(STR && n) : name(std::forward<STR>(n)) {
        std::cout << "TMPL-CONSTR" << std::endl;
    }
    FinalPerson(FinalPerson const & p) : name(p.name) {
        std::cout << "FinalPerson(FinalPerson const &) " << std::endl;
    }
    FinalPerson(FinalPerson && p) : name(std::move(p.name)) {
        std::cout << "FinalPerson(FinalPerson &&)" << std::endl;
    }
private:
    std::string name;
};

// forbid using some memeber function
class C {
public:
    // optimize
    C(){}
    // 下面的语句是为了更适用于模板拷贝构造
    C (C const volatile & x) = delete;
    template <typename T>
    C(T const & x) {
        std::cout << "class C tmp const copy constructor" << std::endl;
    }
};

template <typename T>
class CPlus {
public:
    // optimize
    CPlus(){}
    // 下面的语句是为了更适用于模板拷贝构造
    CPlus (CPlus const volatile & x) = delete;
    // 禁止通过int实例化出来的CPlus进行拷贝
    template <typename U, typename = std::enable_if_t<!std::is_integral<U>::value>>
    CPlus(CPlus<U> const & x) {
        std::cout << "class CPlus template const copy constructor" << std::endl;
    }
};

// use concept to replace enable_if
template <typename T>
concept ConvertibleToString = std::is_convertible_v<T, std::string>;

class CRequire {
public:
    // template <typename STR>
    // requires ConvertibleToString<STR>
    // CRequire(STR && n) : name(std::forward<STR>(n)) {
    //     std::cout << "CRequire Constructor: [" << name.c_str() << "]" << std::endl;
    // }

    // 也可以这么写，精简后
    template <ConvertibleToString STR>
    CRequire(STR && n) : name(std::forward<STR>(n)) {
        std::cout << "CRequire Constructor: [" << name.c_str() << "]" << std::endl;
    }

private:
    std::string name;
};



#endif
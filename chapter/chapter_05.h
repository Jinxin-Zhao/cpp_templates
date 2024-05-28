#ifndef _CHAPTER_05_H_
#define _CHAPTER_05_H_

#include <deque>
#include <cassert>
#include <memory>
#include <bitset>

// built-in type initialization
template <typename T>
void tp_type_initial() {
    T x{};
}
template <typename T>
void tp_type_initial(T p = T{}) {
    T x{};
}

////// naked array template ///////
template <typename T>
struct CNakedArrayTemplateClass;

template <typename T, std::size_t SZ>
struct CNakedArrayTemplateClass<T [SZ]> {
    static void print() {
        std::cout << "print() for T[" << SZ << "]" << std::endl;
    }
};

// partial spec. for references to arrays of known bounds
template <typename T, std::size_t SZ>
struct CNakedArrayTemplateClass<T (&)[SZ]> {
    static void print() {
        std::cout << "print() for T(&)[" << SZ << "]" << std::endl;
    }
};

// partial spec. for arrays of unknown bounds
template <typename T>
struct CNakedArrayTemplateClass<T []> {
    static void print() {
        std::cout << "print() for T[]" << std::endl;
    }
};

// partial spec. for references to arrays of unknown bounds
template <typename T>
struct CNakedArrayTemplateClass<T (&) []> {
    static void print() {
        std::cout << "print() for T(&)[]" << std::endl;
    }
};

template <typename T>
struct CNakedArrayTemplateClass<T *> {
    static void print() {
        std::cout << "point for T*" << std::endl;
    }
};


////// member template specialization ///////
class BoolString {
public:
    BoolString(std::string const & s) : value(s) {}
    template <typename T = std::string>
    T get() const {
        return value;
    }
private:
    std::string value;
};

template <>
inline bool BoolString::get<bool>() const {
    return value == "true" || value == "1" || value == "on";
}


////////////// 5.5.1 use template keywords //////////////
template <unsigned long N>
void printBitset(std::bitset<N> const & bs) {
    std::cout << "print bitset : "; 
    std::cout << bs.template to_string<char, std::char_traits<char>, std::allocator<char>>();
    std::cout << std::endl;
}

////////////// 5.5.2 generic lambda //////////////
auto lambda_func = [] (auto x, auto y) {
    return x + y;
};

////////////// 5.6  param template //////////////
// template definition of pi shouldn't be in function or module
template <typename T>
constexpr T pi{3.1415926535897932385};

/////////////// 用于数据成员的变量模板 ///////////////
template <typename T>
class MyClass {
public:
    static constexpr int max = 1000;
    static constexpr int value = 10;
};
template <typename T>
int myMax = MyClass<T>::max;

// application
auto i = myMax<char>;
// 代替下面的clause
//auto i = MyClass<char>::max;

///////////// type traits suffix_v /////////////////////
template <typename T>
constexpr int myClass_max_v = MyClass<T>::value;

//////////// template param template[模板参数] //////////////
/*
使用区别：
    chfStack<int, vector<int>> vChfStack;
    chfStack<int, vector> vChfStack; // 无须指定第二个模板参数类型
*/

//第二个模板参数被定义为一个类模板，相应的std::deque<T>也变成std::deque
// c++17之后才允许用typename取代第二个模板参数的class:
// template <typename Elem> typename Cont = std::deque
// deque有两个参数，第二个参数是默认参数allocator,这个Allocator也可以省略写成:
// tempalte <typename Elem, typename = std::allocator<Elem>>
template <typename T, 
template <typename Elem, typename Alloc = std::allocator<Elem>> class Cont = std::deque>
class chfStack {
private:
    Cont<T> elems;
public:
    void push(T const &);
    void pop();

    T const & top() const;
    bool empty() const {
        return elems.empty();
    }

    template <typename T2, template<typename Elem2, typename = std::allocator<Elem2>> class Cont2>
    chfStack<T, Cont> & operator= (chfStack<T2, Cont2> const &);

    // to get access to private member of any chfStack with elements of type T2
    template <typename, template <typename, typename> class>
    friend class chfStack;
};


template <typename T, template <typename, typename> class Cont>
    void chfStack<T, Cont>::push (T const & elem) {
        elems.push_back(elem);
    }

template <typename T, template <typename, typename> class Cont>
void chfStack<T, Cont>::pop() {
        assert(!elems.empty());
        elems.pop_back();
    }

template <typename T, template <typename, typename> class Cont>
T const & chfStack<T, Cont>::top () const {
        assert(!elems.empty());
        return elems.back();
    }

template <typename T, template <typename, typename> class Cont>
template <typename T2, template <typename, typename> class Cont2>
chfStack<T, Cont> & chfStack<T,Cont>::operator=(chfStack<T2, Cont2> const & op2) {
    elems.clear();
    elems.insert(elems.begin(), op2.elems.begin(), op2.elems.end());
    return *this;
}
#endif
#ifndef _CHAPTER_20_H_
#define _CHAPTER_20_H_

// ebco
class Empty{};
class TooEmpty : public Empty{};
class ThEmpty : public TooEmpty{};

class multiEmpty : public TooEmpty, public Empty, public ThEmpty{};

//
template <typename Base, typename Member>
class BaseMemberPair : public Base {
private:
    Member mem;
public:
    BaseMemberPair(Base const & b, Member const & m) : Base(b), mem(m){}

    Base const & base() const {
        return static_cast<Base const &>(*this);
    }

    Base & base() {
        return static_cast<Base&>(*this);
    }

    Member const & member() const {
        return this->mem;
    }
};

// not recomended
template <typename CustomClass>
class NROptimizable{
private:
    CustomClass info;
    void * storage;
};

// 可以使用另一种更实际的方法。其主要思想是通过使用EBCO将可能为空的类型参数与别的参数“合并”。
template <typename CustomClass>
class Optimizable {
private:
    BaseMemberPair<CustomClass, void*> info_and_storage;
};

// CRTP
template <typename Derived>
class CuriousBase {

};

class Curious : public CuriousBase<Curious> {

};

// using CRTP pattern
// 一个CRTP的简单应用是将其用于追踪从一个class类型实例化出了多少对象。这一功能也可
// 以通过在构造函数中递增一个static数据成员、并在析构函数中递减该数据成员来实现。
template <typename CountedType>
class ObjectCounter {
private:
    // 'inline' keyword for initializing member variable in class
    inline static std::size_t count = 0;
protected:
    ObjectCounter() {
        ++count;
    }
    ObjectCounter(const ObjectCounter<CountedType> &) {
        ++count;
    }
    ~ObjectCounter() {
        --count;
    }
public:
    static std::size_t live() {
        return count;
    }
};

template <typename T>
class SelfClass : public ObjectCounter<SelfClass<T>> {

};

// Barton-Nackman trick
// for "函数模板受限"

// 假设我们有一个需要为之定义operator==的类模板Array。一个可能的
// 方案是将该运算符定义为类模板的成员，但是由于其第一个参数（绑定到this指针上的参数）
// 和第二个参数的类型转换规则不同（为什么？一个是指针？一个是Arry类型？）
// 一般来说希望operator==两个参数类型是同一个类型
template <typename T>
class EmptyArray {
public:

};

template <typename T> 
bool operator== (const EmptyArray<T> & a, const EmptyArray<T> & b){

}

// 这样会带来一个问题：在当前作用域内不可以再声明其
// 它的operator ==模板，而其它的类模板却又很可能需要这样一个类似的模板。
// Barton和Nackman通过将operator==定义为class内部的一个友元函数解决这样的问题
template <typename T>
class Array {
    static bool areEqual(const Array<T> & a, const Array<T> & b);
public:
// 假设我们用float实例化了该Array类。作为实例化的结果，该友元运算符函数也会被连带声
// 明，但是请注意该函数本身并不是一个函数模板的实例。作为实例化过程的一个副产品，它
// 是一个被注入到 [全局作用] 域的常规非模板函数
// Barton和Nackman把这个称之为restricted template expansion
    friend bool operator== (const Array<T> & a, const Array<T> & b) {
        return areEqual(a, b);
    }
};

// 在函数的调用参数中，至少要有一个参数需要有一个包含了friend函数的关联类
class S {};

template <typename T>
class Wrapper {
private:
    T object;
public:
    Wrapper(T obj) : object(obj){} //implicit conversion from T to Wrapper<T>
    friend void foo(const Wrapper<T> &){}
};

// use
// S s;
// Wrapper<S> w(s);
// 此处的foo(w)调用是有效的，因为foo()是被定义于Wrapper<S>中的友元，而Wrapper<s>又
// 是与参数w有关的类。但是在foo(s)的调用中，friendfoo(Wrapper<S>const&)的声明并不可
// 见，这是因为定义了foo(Wrapper<S>const&)的类Wrapper<S>并没有和S类型的参数s关联起来
// foo(w);
// foo(s); // error

// implement operator!= with CRTP
template <typename Derived>
class EqualityCompatible {
public:
    friend bool operator!= (const Derived & d1, const Derived & d2) {
        return !(d1 == d2);
    }
};

class X : public EqualityCompatible<X> {
public:
    friend bool operator== (const X & x1, const X & x2) {
        return true;
    }
};

#endif
#ifndef _CRTP_H_
#define _CRTP_H_

/////CRTP:  simple example
template <typename Derived>
class CuriousBase{
};

template <typename T>
class CuriousTemplate : public CuriousBase<CuriousTemplate<T>>{
};

//////////////////////////////////////
template<typename CountedType>
class ObjectCounter
{
private:
    inline static std::size_t count=0;
protected:
    ObjectCounter()
    {
        ++count;
    }
    ObjectCounter(ObjectCounter<CountedType>&&)
    {
        ++count;
    }
    ~ObjectCounter()
    {
        --count;
    }
public:
    static std::size_t live()
    {
        return count;
    }
};

template<typename CharT>
class MyString:public ObjectCounter<MyString<CharT>>
{
};

/////the Barton-Nackman trick


template<typename T>
class Wrapper
{
private:
    T object;
public:
    Wrapper(T obj):object(obj){}
    friend void foo(Wrapper<T> const&){}
};

class S{
    //friend void foo(Wrapper<S> const&){}
};

////////
template<typename Derived>
class EqualityComparable
{
public:
    friend bool operator!=(Derived const& x1,Derived const x2)
    {
        return !(x1==x2);
    }
};
class X : public EqualityComparable<X>
{
public:
    friend bool operator==(X const& x1,X const& x2)
    {
        //比较并返回结果
        return true;
    }
};


//pack Expansion
template <typename... Mixins>
class Point : public Mixins... {
public:
    double x,y,z;
    Point() : Mixins()..., x(0.0),y(0.0){}
    Point(double x, double y) : Mixins()..., x(x), y(y) {}

    //mixins is a function parameter pack
    Point(Mixins... mixins) : Mixins()... {}

    template<typename Visitor>
    void visitMixins(Visitor visitor) {
        visitor(static_cast<Mixins&>(*this)...);
    }
};

struct Color { char red, green, blue; };
struct Label { std::string name; };

//pack Expansion in a template parameter list
template <typename... Ts>
struct Values {
    template<Ts... Vs>
    struct Holder{};
};

int i;
Values<char , int , int *>::Holder<'a',17,&i> valueHolder;
///////////////


//nested pack Expansion
template <typename F, typename... Types>
void forwardCopy(F f, Types const &... values){
    f(Types(values)...);
}
//expand to :
template <typename F, typename T1, typename T2, typename T3>
void forwardCopy(F f, T1 const &v1, T2 const & v2, T3 const & v3){
    f( T1(v1), T2(v2), T3(v3));
}

template<typename... OuterTypes>
class Nested {
    template<typename... InnerTypes>
    void f(InnerTypes const&... innerTypes){
        g(OuterTypes(InnerTypes(innerTypes)...)...);
    }
};

//Fold Expression
bool and_all() {return true;}
template <typename T>
bool and_all(T cond) {return cond;}

template <typename T,typename... Ts>
bool and_all(T cond, Ts... conds){
    return cond && and_all(conds...);
}

//Curious Mixins
//template <typename... Mixins>
//class Point : public Mixins... {
//public:
//    double x,y;
//    Point() : Mixins()..., x(0.0),y(0.0){}
//    Point(double x, double y) : Mixins()..., x(x), y(y) {}
//};

//template <template<typename>... Mixins>
//class CPoint : public Mixins<CPoint>... {
//public:
//    double x,y;
//    CPoint() : Mixins<CPoint>()..., x(0.0),y(0.0) {}
//
//};

#endif
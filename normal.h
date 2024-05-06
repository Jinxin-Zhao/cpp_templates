#include "public.h"

//shortcut for member templates
//class somecompilerName{
//    somecompilerName(){}
//    template <typename T1, typename T2>
//    auto operator() (T1 x, T2 y) const {
//        return x + y;
//    }
//
//    [] (auto x, auto y){
//        return x + y;
//    }
//};

/*default call arguments of function template*/
//T{} is 0 for built-in types
template <typename T>
void fill(T const & = T{}){

}

//or
//template <typename T>
//void fill(T const & = T()){}


template <typename T>
class test_t{
public:
    test_t(){}

    void p() {
        m_var.p();
    }

    T m_var;
};

template <typename T>
class test_sub : public test_t<test_sub<T>>{
public:
    typedef  test_t<test_sub<T>> base_type;

    void p(){std::cout << "sub\n";}

    test_sub(){}
};

template <typename T>
class test_pub : public test_t<test_pub<T>>{
public:
    typedef  test_t<test_pub<T>> base_type;

    void p(){std::cout << "pub\n";}

    test_pub(){}
};

template <typename T>
class QQQQ{
    using Type = typename T::type;
};

QQQQ<int> * pp = 0;

template <typename... Types>
class CTuple {
public:
    static constexpr std::size_t length = sizeof...(Types);
};
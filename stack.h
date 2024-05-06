#include "public.h"

template<typename T,
        template <typename Elem,typename Alloc = std::allocator<Elem> >
        class Cont = std::deque>
class Stack{
private:
    Cont<T> elems;

public:
    void push(const T &);
    void pop();
    const T & top() const;
    bool empty() const{
        return elems.empty();
    }

    //assign stack of elements of type T2
    template <typename T2,
            template <typename Elem2,typename = std::allocator<Elem2>>
            class Cont2>
    Stack<T,Cont> &operator= (Stack<T2,Cont2> const &);

    //to get access to private members of Stack<T2>
    template <typename,
            template <typename,typename>
            class>
    friend class Stack;
};

template <typename T,
        template <typename,typename>
        class Cont>
void Stack<T,Cont>::push(const T & elem) {
    elems.push_back(elem);
}


template <typename  T,
        template <typename,typename>
        class Cont>
void Stack<T,Cont>::pop(){
    assert(!elems.empty());
    elems.pop_back();  //remove last element
}

template  <typename T,
        template <typename,typename>
        class Cont>
const  T & Stack<T,Cont>::top() const {
    assert(!empty());
    return elems.back();
}

template <typename T, template <typename ,typename> class Cont>
template <typename T2,template <typename,typename> class Cont2>
Stack<T,Cont> & Stack<T,Cont>::operator=(Stack<T2, Cont2> const & op2) {
    elems.clear();
    elems.insert(elems.begin(),op2.elems.begin(),op2.elems.end());
    return *this;
}

template <typename T>
void outR (T & arg){
    static_assert(!std::is_const<T>::value,"out parameter of foo<T>(T&) is const");

    std::cout<<"arg = "<<arg<<std::endl;

}

void modify(std::string & ss){
    ss[2] = 'E';
}

template <typename T>
void printT(T arg){
    modify(arg);
}
#include "stack.h"
#include "typelist.h"
#include "CRTP.h"
#include <vector>
#include <queue>

#include "normal.h"

template <typename T,
        typename = typename std::enable_if<(sizeof(T) > 4)>::type >   //since c++11
                //or "typename = std::enable_if_t<(sizeof(T) > 4)>>"  //since c++14
        void foo(){

}
/*
 which expands to
 template <typename T,
            typename = void>
            void foo(){
            }
*/


//if sizeof(T)>4 is true,the return type of foo1 is T(the second argument)
template <typename T>
std::enable_if_t<(sizeof(T) > 4),T>
        foo1(){
            return T();
        }

//alias
template <typename T>
using EnableIfSizeGreater4 = std::enable_if_t<(sizeof(T) > 4)>;

template <typename T,
        typename = EnableIfSizeGreater4<T>>
                void foo3(){
                        std::cout << "sizeof(T) > 4" << std::endl;
                }

int main(){
    auto size_tuple = CTuple<short,int,long,double>::length;
    std::cout << "size of CTuple is " << size_tuple << std::endl;

    // 
    Point<Color,Label> p;
    Point<Color,Label> p_p({0x7F,0,0x7F},{"center"});

    Front<SignedIntegralType> uc = '5';

    ////
    std::string s;
    std::ref(s);
    Stack<int> iStack;
    Stack<float> fStack;
    iStack.push(1);
    iStack.push(2);
    std::cout<<"iStack.top(): "<<iStack.top()<<std::endl;

    fStack.push(3.3);

    fStack = iStack;
    fStack.push(4.4);
    std::cout<<"fStack.top(): "<<fStack.top()<<std::endl;

    int ab = 99;
    outR<int>(ab);

    //std::string s = "Hello";
    //printT(std::ref(s));

    //printf("s is : [%s]\n",s.c_str());

    S s_t;
    Wrapper<S> w(s_t);
    foo(w);  //Ok

    return 0;
}
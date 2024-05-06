#ifndef __TYPE_LIST_H
#define __TYPE_LIST_H


template <typename... Elements>
class Typelist{

};

using SignedIntegralType = Typelist<signed char,short,int,long,long long>;

template <typename List>
class FrontT;

template <typename Head,typename... Tail>
class FrontT<Typelist<Head,Tail...>>{
public:
    using Type = Head;
};

//get the first type of typelist
template <typename List>
using Front = typename FrontT<List>::Type;

///////
template <typename List>
class PopFrontT;

template <typename Head,typename... Tail>
class PopFrontT<Typelist<Head,Tail...>>{
public:
    using Type = Typelist<Tail...>;
};

template <typename List>
using PopFront = typename PopFrontT<List>::Type ;

///////
template <typename List,typename NewElement>
class PushFrontT;

template <typename... Elements,typename NewElement>
class PushFrontT<Typelist<Elements...>,NewElement>{
public:
    using Type = Typelist<NewElement,Elements...>;
};

template <typename List,typename NewElement>
using PushFront = typename PushFrontT<List,NewElement>::Type ;

//recursive case
template <typename List,unsigned N>
class NthElementT : public NthElementT<PopFront<List>,N-1>{
};

//basis case:
template <typename List>
class NthElementT<List,0> : public FrontT<List>{
};

template <typename List,unsigned N>
using NthElement = typename NthElementT<List,N>::Type ;


template <class T>
class CupBoad{
    enum wood : unsigned char;
};

#endif
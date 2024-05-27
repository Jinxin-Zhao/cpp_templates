#ifndef _CHAPTER_05_H_
#define _CHAPTER_05_H_

// 用于数据成员的变量模板
template <typename T>
class MyClass {
public:
    static constexpr int max = 1000;
};
template <typename T>
int myMax = MyClass<T>::max;

// application
auto i = myMax<char>;
// 代替下面的clause
//auto i = MyClass<char>::max;




#endif
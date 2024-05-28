#include <iostream>
#include "../gtest/include/gtest/gtest.h"
#include "../../chapter/chapter_05.h"

// x array definition
int x[] = {3,5,6};

template <typename T1, typename T2, typename T3>
void testNakedArrayTemplate(int a1[7], int a2[], int (&a3)[42], int (x0)[], T1 x1, T2 & x2, T3 && x3) {
    CNakedArrayTemplateClass<decltype(a1)>::print(); // uses CNakedArrayTemplateClass<T*>
    CNakedArrayTemplateClass<decltype(a2)>::print(); // uses CNakedArrayTemplateClass<T*> a1, a2 退化成指针
    CNakedArrayTemplateClass<decltype(a3)>::print(); // uses CNakedArrayTemplateClass<T(&)[SZ]>
    CNakedArrayTemplateClass<decltype(x0)>::print(); // uses CNakedArrayTemplateClass<T(&)[]>
    CNakedArrayTemplateClass<decltype(x1)>::print(); // uses CNakedArrayTemplateClass<T*>
    CNakedArrayTemplateClass<decltype(x2)>::print(); // uses CNakedArrayTemplateClass<T(&)[]>
    CNakedArrayTemplateClass<decltype(x3)>::print(); // uses CNakedArrayTemplateClass<T(&)[]> // 万能引用， 引用折叠
}

TEST(TestChapterFive, array_template) {
    int a[42];
    CNakedArrayTemplateClass<decltype(a)>::print(); // uses CNakedArrayTemplateClass<T[SZ]>
    extern int x[]; // forward declare array
    CNakedArrayTemplateClass<decltype(x)>::print(); // uses CNakedArrayTemplateClass<T[]>
    testNakedArrayTemplate(a, a, a, x, x, x, x);
}

TEST(TestChapterFive, member_template) {
    std::cout << std::boolalpha;
    BoolString s("hello");
    std::cout << s.get() << std::endl;
    std::cout << s.get<bool>() << std::endl;
    BoolString s_2("on");
    std::cout << s_2.get<bool>() << std::endl;
}

TEST(TestChapterFive, template_keywords) {
    std::bitset<4> bs{"0011"};
    printBitset(bs);
}

TEST(TestChapterFive, generic_lambda) {
    auto res = lambda_func(4.5, 9.8);
    std::cout << "result of lambda function " << res << std::endl;
}

TEST(TestChapterFive, template_param) {
    std::cout << "unit test chapter 05"<< std::endl;
    auto t_max = myMax<char>;
    std::cout << "max value " << t_max << std::endl;
    auto value = myClass_max_v<int>;
    std::cout << "value " << value << std::endl;

    //
    chfStack<int> iStack;
    chfStack<float> fStack;
    iStack.push(1);
    iStack.push(2);
    iStack.push(3);
    fStack = iStack;
    std::cout << "fStack top value " << fStack.top() << std::endl;

    fStack.push(18.5);
    iStack = fStack;
    std::cout << "iStack top value " << iStack.top() << std::endl;

    chfStack<double> vStack;
    vStack = iStack;
    vStack.push(19.9);
    while (!vStack.empty()) {
        std::cout << vStack.top() << " ";
        vStack.pop();
    }
    std::cout << "\n";
}

int main (int argc, char** argv) {
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}
#include <iostream>
#include "../gtest/include/gtest/gtest.h"
#include "../../chapter/chapter_05.h"

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
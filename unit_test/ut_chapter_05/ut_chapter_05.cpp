#include <iostream>
#include "../gtest/include/gtest/gtest.h"
#include "../../chapter/chapter_05.h"

TEST(TestChapterFive, template_param) {
    std::cout << "unit test chapter 05"<< std::endl;
    auto t_max = myMax<char>;
    std::cout << "max value " << t_max << std::endl;
}

int main (int argc, char** argv) {
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}
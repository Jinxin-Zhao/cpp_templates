#include <iostream>
#include "../gtest/include/gtest/gtest.h"
#include "../../chapter/chapter_19.h"

TEST(TestChapterEight, test_traits) {
    char name[] = "templates";
    int length = sizeof(name) - 1;
    std::cout << "accum average tempaltes " << accum(name, name + length) / length << std::endl;
}

int main (int argc, char** argv) {
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}
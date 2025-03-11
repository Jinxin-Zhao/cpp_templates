#include <iostream>
#include <chrono>
#include "../gtest/include/gtest/gtest.h"
#include "../../chapter/chapter_19.h"




TEST(TestChapterEight, test_traits) {
    // char name[] = "templates";
    // int length = sizeof(name) - 1;
    // std::cout << "accum average tempaltes " << accum(name, name + length) / length << std::endl;
    //
    static int num[] = {1, 2, 3, 4, 5};
    std::cout << "the result is " << accum<int, MultPolicy>(num, num + 5) << std::endl;

    std::vector<bool> s;
    printElemType(s);
    int arr[43];
    printElemType(arr);
}

int main (int argc, char** argv) {
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();

    return 0;
}
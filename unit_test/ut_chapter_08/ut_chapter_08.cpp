#include <iostream>
#include "../gtest/include/gtest/gtest.h"
#include "../../chapter/chapter_08.h"

TEST(TestChapterEight, test_compile_program) {

}

int main (int argc, char** argv) {
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}
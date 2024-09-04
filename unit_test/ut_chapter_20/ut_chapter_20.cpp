#include <iostream>
#include "../gtest/include/gtest/gtest.h"
#include "../../chapter/chapter_20.h"

TEST(TestChapterTwenty, test_class) {
    auto firstSize = sizeof(Empty);
    auto secondSize = sizeof(TooEmpty);
    auto thirdSize = sizeof(ThEmpty);
    auto multsize = sizeof(multiEmpty);
    std::cout << "Empty Size " << firstSize << std::endl;
    std::cout << "second Size " << secondSize << std::endl;
    std::cout << "Third Size " << thirdSize << std::endl;
    std::cout << "Multi Size " << multsize << std::endl;
}

TEST(TestChapterTwenty, TestSelfClass) {
    SelfClass<char> testCharClass, secondClass;
    std::cout << "testCharClass count " << SelfClass<char>::live() << std::endl;
    //
    X x1, x2;
    if (x1 != x2) {
        std::cout << "not equal " << std::endl;
    } else {
        std::cout << "equal " << std::endl;
    }
}

int main (int argc, char** argv) {
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}
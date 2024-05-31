#include <iostream>
#include "../gtest/include/gtest/gtest.h"
#include "../../chapter/chapter_07.h"

TEST(TestChapterSeven, test_type_decay) {
    std::string const c = "hi";
    CNormalValueTemplateFunc(c); // c decays so that arg has type [std::string]
    CNormalValueTemplateFunc("hi"); // from char const[3] to [char const *]
    int arr[4];
    CNormalValueTemplateFunc(arr); // int *
}

TEST(TestChapterSeven, test_reference_type) {
    std::string const c = "hi";
    CReferenceTemplateFunc(c); // no copy
    CReferenceTemplateFunc("hi"); //
    int arr[4];
    CReferenceTemplateFunc(arr); // 
}



int main (int argc, char** argv) {
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}
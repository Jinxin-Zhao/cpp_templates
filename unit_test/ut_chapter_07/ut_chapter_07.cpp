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
    std::string const s = "hi";
    CReferenceTemplateFunc(s); // no copy, T deduced as std::string
    CReferenceTemplateFunc(std::string("hi")); // no copy
    CReferenceTemplateFunc(std::move(s)); // no copy
    int arr[4];
    CReferenceTemplateFunc(arr); // T deduced as char[3], function parameter is int const(&)[4]
}

TEST(TestChapterSeven, test_reference_non_const_type) {
    std::string const s = "hi";
    CReferenceNonConstFunc(s); // T deduced as std::string const
    CReferenceNonConstFunc("hi"); // T deduced as char const[3]
    CReferenceNonConstFunc(std::move(s)); //
    // 这种情况下仔函数模板内部任何试图更改被传递参数值的行为都是错误的
    // 确实也可以传递一个const对象给这个函数，但在函数充分实例化后（也有可能发生在编译过程中），任何试图
    // 修改参数值的行为都会触发错误，这个有可能发生在很深层次的模板调用逻辑种。
    // solution：这种情况可以限制这种普通模板调用过程：
    // 1. 增加一个static_assert(!std::is_const<T>::value, "param is const");
    // 2. 通过std::enable_if;
}

TEST(TestChapterSeven, test_double_reference) {
    std::string s = "hi";
    passR(s); // T deduced as [std::string &]
    passR(std::string("hi")); // T deduced as std::string
    passR(std::move("hi")); // T deduced as std::string

    //
    passR("hi"); // T deduced as [char const(&)[3] ]
    std::string const c = "const_str";
    passR(c); // T deduced as [std::string const &]
    int arr[4];
    passR(arr); // T deduced as int& [4]
    // 唯一一种传参方式可以在passR内部从arg类型得知被传递的参数是一个rvalue或是一个const or 非const的左值
    // 带来的弊端就是，如果在函数内部使用T声明一个未初始化的局部变量，有可能会导致错误(当T被推断为一个类型的引用时，引用变量声明时须初始化)
}

TEST(TestChapterSeven, test_cref) {
    std::string s = "hello";
    // 
    printT(std::cref(s));
}

int main (int argc, char** argv) {
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}
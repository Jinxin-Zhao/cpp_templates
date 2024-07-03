#include <iostream>
#include "../gtest/include/gtest/gtest.h"
#include "../../chapter/chapter_08.h"

TEST(TestChapterEight, test_compile_program) {
    IsPrime<9>::value;

    std::vector<int> v;
    // match the typename T::size_t f_len(T const & t) function
    f_len(v);
    // error: ‘const class std::allocator<int>’ has no member named ‘size’
    // 虽然 typename T::size_type f_len(const T & t) 和 std::size_t f_len(T...) 都能匹配上
    // 但是第二个函数依然是最佳匹配项，因此编译器依然会匹配到第二个且报错说缺少size()函数
    // std::allocator<int> a;
    // f_len(a);
}

int main (int argc, char** argv) {
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}
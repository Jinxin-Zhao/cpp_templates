#include <iostream>
#include "../gtest/include/gtest/gtest.h"
#include "../../chapter/chapter_06.h"

TEST(TestChapterSix, template_param) {
   int value = 1;
   const int c = 1;
   f(int());  // call g(int &&)
   f(std::move(value)); // call g(int &&)
   // test member function tempalte
   std::string s = "sname";
   CPerson p1(s);
   CPerson("s");
   CPerson p3(p1);
   CPerson p4(std::move(p1));
   // template constructor
   // STR这里被推断为一个char const[6]
   CTPPerson tp("bname");
   CTPPerson tp2(s);
   // 下面的语句错误原因: 对于一个非const左值CTPPerson,成员模板template<typename T>对于一个非const左值CTPPerson(STR && n)
   // 通常比预定义的拷贝构造函数CTPPerson(CTPPerson const & p)更匹配，这里STR可以替换为CTPPerson &
   // 此时提供一个CTPPerson(CTPPerson & p)就可以，不过更好的办法是使用enable_if
   //CTPPerson tp3(tp); // error
   const CTPPerson cp("tmp");
   CTPPerson tp3(cp);
   // test FinalPerson using enable_if
   FinalPerson fp("finalTmp");
   // 此时就匹配到了FinalPerson(const FinalPerson &)构造函数了
   FinalPerson fp2(fp);

   //
   C x;
   C y{x}; // still use predefined copy constructor instead of template copy constructor

    // 以下报错
    // CPlus<int> cplus;
    // CPlus<int> cplus2(cplus);
    CPlus<double> cplus;
    CPlus<double> cplus2(cplus);

    // test require & concept
    CRequire r("require");
}

int main (int argc, char** argv) {
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}
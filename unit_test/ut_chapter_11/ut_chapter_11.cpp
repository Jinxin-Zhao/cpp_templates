#include "../gtest/include/gtest/gtest.h"
#include "../../chapter/chapter_11.h"

TEST(TestChapterEight, test_template_lib) {
   std::vector<int> primes = {1, 2, 4, 5, 6, 8, 10};
   foreach(primes.begin(), primes.end(), func);
   foreach(primes.begin(), primes.end(), &func);
   // op(*current) will be transformed to "op.operator()(*current)"
   foreach(primes.begin(), primes.end(), FuncObj());
   foreach(primes.begin(), primes.end(), [](int i) {
        std::cout << "lambda as callable" << std::endl;
   });
   //
   foreach(primes.begin(), primes.end(), [](std::string const & prefix, int i) {
        std::cout << prefix << i;
   }, " value: "); // "- value" is 1st arg of lambda
   std::cout << std::endl;
   //
   MyClass obj;
   foreach(primes.begin(), primes.end(), &MyClass::memfunc, obj);
}

TEST(TestChapterEight, test_template_is_reference) {
    int i;
    int & r = i;
    tmplParamIsReference(i); // false
    tmplParamIsReference(r); // false
    tmplParamIsReference<int&>(i); // true
    tmplParamIsReference<int&>(r); // true
}

int main (int argc, char** argv) {
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}
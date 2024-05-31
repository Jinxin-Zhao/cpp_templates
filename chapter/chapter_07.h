#ifndef _CHAPTER_07_H_
#define _CHAPTER_07_H_

// 1.6.1节中建议在函数模板中应该优先使用按值传递，除非遇到以下情况：
// - 对象不允许被copy
// 参数被用于返回数据
// 参数以及其所有属性需要被模板转发到别的地方
// 可以获得明显的性能提升

// passed by value results in type decay
template <typename T>
void CNormalValueTemplateFunc(T arg){
}

// passed by reference
template <typename T>
void CReferenceTemplateFunc(T const & arg) {

}





#endif
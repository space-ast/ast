#include "AstUtil/SharedPtr.hpp"
#include "AstUtil/Object.hpp"

class MyObject: public _AST Object
{
public:
    void doSomething(){/* todo: implement functionality */}
};

int main()
{
    AST_USING_NAMESPACE 
    // 创建共享指针对象（只能用于Object的子类）
    SharedPtr<MyObject> ptr1 = new MyObject();  // MyObject必须继承自Object

    // 共享所有权
    SharedPtr<MyObject> ptr2 = ptr1;

    // 访问对象成员
    ptr1->doSomething();

    // 获取原始指针
    MyObject* rawPtr = ptr1.get();
}
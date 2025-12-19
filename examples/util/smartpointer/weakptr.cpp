#include "AstUtil/WeakPtr.hpp"
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
    // 创建共享指针（只能用于Object的子类）
    SharedPtr<MyObject> shared = new MyObject();  // MyObject必须继承自Object

    // 创建弱引用指针
    WeakPtr<MyObject> weak = shared.get();

    // 安全访问对象
    if (MyObject* obj = weak.get()) {
        obj->doSomething();
    }
}
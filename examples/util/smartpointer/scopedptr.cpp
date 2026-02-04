#include "AstUtil/ScopedPtr.hpp"
#include "AstUtil/Object.hpp"

class MyObject: public _AST Object
{
public:
    void doSomething(){/* todo: implement functionality */}
};

int main()
{
    AST_USING_NAMESPACE
    {  
        // 作用域开始
        // 可以用于普通类型
        ScopedPtr<int> intPtr(new int(42));
        
        // 也可以用于Object子类（有特殊的删除处理）
        ScopedPtr<MyObject> objPtr(new MyObject());  // MyObject必须继承自Object
        objPtr->doSomething();
        
        // 重置指针
        objPtr.reset(new MyObject());
        
        // 作用域结束时自动释放
    }
}
#include "Vector.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Vector::staticType;

Class* Vector::StaticType()
{
    return &staticType;
}

Class* Vector::getType() const
{
    return &staticType;
}

static bool Vector_ClassInited = (Vector::ClassInit(&Vector::staticType), true);

void Vector::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "Vector"));
    cls->setDesc(u8R"(向量抽象基类)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<Vector>();

}

AST_NAMESPACE_END
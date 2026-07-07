#include "Vector.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Vector::staticType;

static bool Vector_ClassInited = (Vector::ClassInit(&Vector::staticType), true);

void Vector::ClassInit(Class* cls)
{

    cls->setName("Vector");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<Vector>();

}

AST_NAMESPACE_END
#include "MockObject.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class MockObject::staticType;

static bool MockObject_ClassInited = (MockObject::ClassInit(&MockObject::staticType), true);

void MockObject::ClassInit(Class* cls)
{

    cls->setName("MockObject");
    cls->addToRegistry();
    cls->setParent<Object>();
    cls->setConstructor<MockObject>();

    cls->addProperty("x", aNewPropertyQuantity<MockObject, &MockObject::x, &MockObject::setX>(Dimension::Length()));
    cls->addProperty("duration", aNewPropertyQuantity<MockObject, &MockObject::duration, &MockObject::setDuration>(Dimension::Time()));
    cls->addProperty("factor", aNewPropertyDouble<MockObject, &MockObject::factor, &MockObject::setFactor>());
}

AST_NAMESPACE_END
#include "ObjectCalculation.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class ObjectCalculation::staticType;

static bool ObjectCalculation_ClassInited = (ObjectCalculation::ClassInit(&ObjectCalculation::staticType), true);

void ObjectCalculation::ClassInit(Class* cls)
{

    cls->setName("ObjectCalculation");
    cls->setDesc(u8R"(对象计算基类)");
    cls->addToRegistry();
    cls->setParent<Object>();
    cls->setConstructor<ObjectCalculation>();

}

AST_NAMESPACE_END
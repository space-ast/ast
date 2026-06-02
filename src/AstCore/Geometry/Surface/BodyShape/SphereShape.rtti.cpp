#include "SphereShape.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class SphereShape::staticType;

static bool SphereShape_ClassInited = (SphereShape::ClassInit(&SphereShape::staticType), true);

void SphereShape::ClassInit(Class* cls)
{

    cls->setName("SphereShape");
    cls->setDesc(u8R"(球体形状)");
    cls->addToRegistry();
    cls->setParent<BodyShape>();
    cls->setConstructor<SphereShape>();

}

AST_NAMESPACE_END
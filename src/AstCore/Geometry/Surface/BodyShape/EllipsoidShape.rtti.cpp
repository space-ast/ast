#include "EllipsoidShape.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class EllipsoidShape::staticType;

static bool EllipsoidShape_ClassInited = (EllipsoidShape::ClassInit(&EllipsoidShape::staticType), true);

void EllipsoidShape::ClassInit(Class* cls)
{

    cls->setName("EllipsoidShape");
    cls->setDesc(u8R"(椭球体形状)");
    cls->addToRegistry();
    cls->setParent<BodyShape>();
    cls->setConstructor<EllipsoidShape>();

}

AST_NAMESPACE_END
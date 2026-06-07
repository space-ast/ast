#include "EllipsoidShape.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class EllipsoidShape::staticType;

static bool EllipsoidShape_ClassInited = (EllipsoidShape::ClassInit(&EllipsoidShape::staticType), true);

void EllipsoidShape::ClassInit(Class* cls)
{

    cls->setName(N_("EllipsoidShape"));
    cls->setDesc(u8R"(椭球体形状)");
    cls->addToRegistry();
    cls->setParent<BodyShape>();
    cls->setConstructor<EllipsoidShape>();

    cls->addProperty("majorAxis", aNewPropertyQuantityMem<EllipsoidShape, &EllipsoidShape::majorAxis_>(Dimension::Length()));
    cls->addProperty("middleAxis", aNewPropertyQuantityMem<EllipsoidShape, &EllipsoidShape::middleAxis_>(Dimension::Length()));
    cls->addProperty("minorAxis", aNewPropertyQuantityMem<EllipsoidShape, &EllipsoidShape::minorAxis_>(Dimension::Length()));
}

AST_NAMESPACE_END
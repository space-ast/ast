#include "SpheroidShape.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class SpheroidShape::staticType;

static bool SpheroidShape_ClassInited = (SpheroidShape::ClassInit(&SpheroidShape::staticType), true);

void SpheroidShape::ClassInit(Class* cls)
{

    cls->setName("SpheroidShape");
    cls->setDesc(u8R"(旋转椭球体/扁球体)");
    cls->addToRegistry();
    cls->setParent<BodyShape>();
    cls->setConstructor<SpheroidShape>();

    cls->addProperty("majorAxis", aNewPropertyQuantityMem<SpheroidShape, &SpheroidShape::majorAxis_>(Dimension::Length()));
    cls->addProperty("flatfactor", aNewPropertyDoubleMem<SpheroidShape, &SpheroidShape::flatfactor_>());
}

AST_NAMESPACE_END
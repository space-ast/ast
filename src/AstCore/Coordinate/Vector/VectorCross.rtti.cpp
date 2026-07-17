#include "VectorCross.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class VectorCross::staticType;

static bool VectorCross_ClassInited = (VectorCross::ClassInit(&VectorCross::staticType), true);

void VectorCross::ClassInit(Class* cls)
{
    cls->setName("VectorCross");
    cls->addToRegistry();
    cls->setParent<Vector>();
    cls->setConstructor<VectorCross>();

    cls->addProperty("Axes",    aNewPropertyObject<VectorCross, Axes,   &VectorCross::getAxes>());
    cls->addProperty("Vector1", aNewPropertyObject<VectorCross, Vector, &VectorCross::getVector1, &VectorCross::setVector1>());
    cls->addProperty("Vector2", aNewPropertyObject<VectorCross, Vector, &VectorCross::getVector2, &VectorCross::setVector2>());
}

AST_NAMESPACE_END

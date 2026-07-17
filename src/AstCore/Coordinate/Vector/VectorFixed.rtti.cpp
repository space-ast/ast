#include "VectorFixed.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class VectorFixed::staticType;

static bool VectorFixed_ClassInited = (VectorFixed::ClassInit(&VectorFixed::staticType), true);

void VectorFixed::ClassInit(Class* cls)
{
    cls->setName("VectorFixed");
    cls->addToRegistry();
    cls->setParent<Vector>();
    cls->setConstructor<VectorFixed>();

    cls->addProperty("Axes", aNewPropertyObject<VectorFixed, Axes, &VectorFixed::getAxes, &VectorFixed::setAxes>());
}

AST_NAMESPACE_END

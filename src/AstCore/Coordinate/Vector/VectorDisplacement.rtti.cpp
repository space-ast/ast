#include "VectorDisplacement.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class VectorDisplacement::staticType;

static bool VectorDisplacement_ClassInited = (VectorDisplacement::ClassInit(&VectorDisplacement::staticType), true);

void VectorDisplacement::ClassInit(Class* cls)
{

    cls->setName("VectorDisplacement");
    cls->addToRegistry();
    cls->setParent<Vector>();
    cls->setConstructor<VectorDisplacement>();

    cls->addProperty("Origin", aNewPropertyObject<VectorDisplacement, Point, &VectorDisplacement::getOrigin, &VectorDisplacement::setOrigin>());
    cls->addProperty("Destination", aNewPropertyObject<VectorDisplacement, Point, &VectorDisplacement::getDestination, &VectorDisplacement::setDestination>());
}

AST_NAMESPACE_END
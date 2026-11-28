#include "VectorDisplacement.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class VectorDisplacement::staticType;

Class* VectorDisplacement::StaticType()
{
    return &staticType;
}

Class* VectorDisplacement::getType() const
{
    return &staticType;
}

static bool VectorDisplacement_ClassInited = (VectorDisplacement::ClassInit(&VectorDisplacement::staticType), true);

void VectorDisplacement::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "VectorDisplacement"));
    cls->addToRegistry();
    cls->setParent<Vector>();
    cls->setConstructor<VectorDisplacement>();

    cls->addProperty("Origin", aNewPropertyObject<VectorDisplacement, Point, &VectorDisplacement::getOrigin, &VectorDisplacement::setOrigin>());
    cls->addProperty("Destination", aNewPropertyObject<VectorDisplacement, Point, &VectorDisplacement::getDestination, &VectorDisplacement::setDestination>());
}

AST_NAMESPACE_END
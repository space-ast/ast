#include "VectorCross.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class VectorCross::staticType;

Class* VectorCross::StaticType()
{
    return &staticType;
}

Class* VectorCross::getType() const
{
    return &staticType;
}

static bool VectorCross_ClassInited = (VectorCross::ClassInit(&VectorCross::staticType), true);

void VectorCross::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "VectorCross"));
    cls->setDesc(u8R"(叉乘向量类 (Cross Product))");
    cls->addToRegistry();
    cls->setParent<Vector>();
    cls->setConstructor<VectorCross>();

    cls->addProperty("Vector1", aNewPropertyObject<VectorCross, Vector, &VectorCross::getVector1, &VectorCross::setVector1>());
    cls->addProperty("Vector2", aNewPropertyObject<VectorCross, Vector, &VectorCross::getVector2, &VectorCross::setVector2>());
}

AST_NAMESPACE_END
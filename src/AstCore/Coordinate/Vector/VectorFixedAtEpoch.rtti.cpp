#include "VectorFixedAtEpoch.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class VectorFixedAtEpoch::staticType;

Class* VectorFixedAtEpoch::StaticType()
{
    return &staticType;
}

Class* VectorFixedAtEpoch::getType() const
{
    return &staticType;
}

static bool VectorFixedAtEpoch_ClassInited = (VectorFixedAtEpoch::ClassInit(&VectorFixedAtEpoch::staticType), true);

void VectorFixedAtEpoch::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "VectorFixedAtEpoch"));
    cls->setDesc(u8R"(历元固定向量类 (Fixed at Epoch))");
    cls->addToRegistry();
    cls->setParent<Vector>();
    cls->setConstructor<VectorFixedAtEpoch>();

    cls->addProperty("SourceVector", aNewPropertyObject<VectorFixedAtEpoch, Vector, &VectorFixedAtEpoch::getSourceVector, &VectorFixedAtEpoch::setSourceVector>());
}

AST_NAMESPACE_END
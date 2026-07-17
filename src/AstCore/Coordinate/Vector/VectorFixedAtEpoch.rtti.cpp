#include "VectorFixedAtEpoch.hpp"
#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class VectorFixedAtEpoch::staticType;

static bool VectorFixedAtEpoch_ClassInited = (VectorFixedAtEpoch::ClassInit(&VectorFixedAtEpoch::staticType), true);

void VectorFixedAtEpoch::ClassInit(Class* cls)
{
    cls->setName("VectorFixedAtEpoch");
    cls->addToRegistry();
    cls->setParent<Vector>();
    cls->setConstructor<VectorFixedAtEpoch>();

    cls->addProperty("SourceVector", aNewPropertyObject<VectorFixedAtEpoch, Vector, &VectorFixedAtEpoch::getSourceVector, &VectorFixedAtEpoch::setSourceVector>());
    cls->addProperty("Axes",         aNewPropertyObject<VectorFixedAtEpoch, Axes,   &VectorFixedAtEpoch::getAxes,         &VectorFixedAtEpoch::setAxes>());
    cls->addProperty("Epoch",        aNewPropertyTimePoint<VectorFixedAtEpoch,      &VectorFixedAtEpoch::getEpoch,        &VectorFixedAtEpoch::setEpoch>());
}

AST_NAMESPACE_END

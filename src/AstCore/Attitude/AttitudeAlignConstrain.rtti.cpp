#include "AttitudeAlignConstrain.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AttitudeAlignConstrain::staticType;

Class* AttitudeAlignConstrain::StaticType()
{
    return &staticType;
}

Class* AttitudeAlignConstrain::getType() const
{
    return &staticType;
}

static bool AttitudeAlignConstrain_ClassInited = (AttitudeAlignConstrain::ClassInit(&AttitudeAlignConstrain::staticType), true);

void AttitudeAlignConstrain::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "AttitudeAlignConstrain"));
    cls->setDesc(u8R"(对齐/约束姿态(Aligned and Constrained))");
    cls->addToRegistry();
    cls->setParent<AttitudeProfile>();
    cls->setConstructor<AttitudeAlignConstrain>();

}

AST_NAMESPACE_END
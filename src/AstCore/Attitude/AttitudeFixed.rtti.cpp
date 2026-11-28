#include "AttitudeFixed.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AttitudeFixed::staticType;

Class* AttitudeFixed::StaticType()
{
    return &staticType;
}

Class* AttitudeFixed::getType() const
{
    return &staticType;
}

static bool AttitudeFixed_ClassInited = (AttitudeFixed::ClassInit(&AttitudeFixed::staticType), true);

void AttitudeFixed::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "AttitudeFixed"));
    cls->setDesc(u8R"(固定姿态)");
    cls->addToRegistry();
    cls->setParent<AttitudeProfile>();
    cls->setConstructor<AttitudeFixed>();

    cls->addProperty("referenceAxes", aNewPropertyObject<AttitudeFixed, Axes, &AttitudeFixed::referenceAxes, &AttitudeFixed::setReferenceAxes>());
}

AST_NAMESPACE_END
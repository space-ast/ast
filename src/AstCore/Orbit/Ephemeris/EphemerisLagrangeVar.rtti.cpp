#include "EphemerisLagrangeVar.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class EphemerisLagrangeVar::staticType;

static bool EphemerisLagrangeVar_ClassInited = (EphemerisLagrangeVar::ClassInit(&EphemerisLagrangeVar::staticType), true);

void EphemerisLagrangeVar::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "EphemerisLagrangeVar"));
    cls->setDesc(u8R"(可变步长拉格朗日插值星历)");
    cls->addToRegistry();
    cls->setParent<Ephemeris>();
    cls->setConstructor<EphemerisLagrangeVar>();

}

AST_NAMESPACE_END
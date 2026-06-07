#include "BodyEphemerisSPK.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class BodyEphemerisSPK::staticType;

static bool BodyEphemerisSPK_ClassInited = (BodyEphemerisSPK::ClassInit(&BodyEphemerisSPK::staticType), true);

void BodyEphemerisSPK::ClassInit(Class* cls)
{

    cls->setName(N_("BodyEphemerisSPK"));
    cls->addToRegistry();
    cls->setParent<BodyEphemeris>();
    cls->setConstructor<BodyEphemerisSPK>();

    cls->addProperty("SpiceIndex", aNewPropertyInt<BodyEphemerisSPK, &BodyEphemerisSPK::getSpiceIndex, &BodyEphemerisSPK::setSpiceIndex>());
}

AST_NAMESPACE_END
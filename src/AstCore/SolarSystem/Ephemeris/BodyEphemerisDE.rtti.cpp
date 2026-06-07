#include "BodyEphemerisDE.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class BodyEphemerisDE::staticType;

static bool BodyEphemerisDE_ClassInited = (BodyEphemerisDE::ClassInit(&BodyEphemerisDE::staticType), true);

void BodyEphemerisDE::ClassInit(Class* cls)
{

    cls->setName(N_("BodyEphemerisDE"));
    cls->addToRegistry();
    cls->setParent<BodyEphemeris>();
    cls->setConstructor<BodyEphemerisDE>();

    cls->addProperty("JplIndex", aNewPropertyInt<BodyEphemerisDE, &BodyEphemerisDE::getJplIndex, &BodyEphemerisDE::setJplIndex>());
}

AST_NAMESPACE_END
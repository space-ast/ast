#include "CelestialBody.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class CelestialBody::staticType;

static bool CelestialBody_ClassInited = (CelestialBody::ClassInit(&CelestialBody::staticType), true);

void CelestialBody::ClassInit(Class* cls)
{

    cls->setName("CelestialBody");
    cls->setDesc(u8R"(天体)");
    cls->addToRegistry();
    cls->setParent<Point>();
    cls->setConstructor<CelestialBody>();

}

AST_NAMESPACE_END
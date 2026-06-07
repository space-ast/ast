#include "SolarSystem.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class SolarSystem::staticType;

static bool SolarSystem_ClassInited = (SolarSystem::ClassInit(&SolarSystem::staticType), true);

void SolarSystem::ClassInit(Class* cls)
{

    cls->setName(N_("SolarSystem"));
    cls->setDesc(u8R"(太阳系)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<SolarSystem>();

}

AST_NAMESPACE_END
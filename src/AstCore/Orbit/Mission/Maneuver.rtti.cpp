#include "Maneuver.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Maneuver::staticType;

static bool Maneuver_ClassInited = (Maneuver::ClassInit(&Maneuver::staticType), true);

void Maneuver::ClassInit(Class* cls)
{

    cls->setName("Maneuver");
    cls->setDesc(u8R"(机动轨道段，用于建模任务序列(MissionCommand)中的机动任务)");
    cls->addToRegistry();
    cls->setParent<Segment>();
    cls->setConstructor<Maneuver>();

    cls->addProperty("burn", aNewPropertyObject<Maneuver, Burn, &Maneuver::burn, &Maneuver::setBurn>());
}

AST_NAMESPACE_END
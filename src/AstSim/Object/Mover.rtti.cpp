#include "Mover.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Mover::staticType;

static bool Mover_ClassInited = (Mover::ClassInit(&Mover::staticType), true);

void Mover::ClassInit(Class* cls)
{

    cls->setName("Mover");
    cls->setDesc(u8R"(运动对象)");
    cls->addToRegistry();
    cls->setParent<Point>();
    cls->setConstructor<Mover>();

    cls->addProperty("MotionProfile", aNewPropertyObject<Mover, MotionProfile, &Mover::getMotionProfile, &Mover::setMotionProfile>());
    cls->addProperty("AttitudeProfile", aNewPropertyObject<Mover, AttitudeProfile, &Mover::getAttitudeProfile, &Mover::setAttitudeProfile>());
    cls->addProperty("Ephemeris", aNewPropertyObject<Mover, Ephemeris, &Mover::getEphemeris, &Mover::setEphemeris>());
}

AST_NAMESPACE_END
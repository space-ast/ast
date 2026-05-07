#include "BurnImpulsive.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class BurnImpulsive::staticType;

static bool BurnImpulsive_ClassInited = (BurnImpulsive::ClassInit(&BurnImpulsive::staticType), true);

void BurnImpulsive::ClassInit(Class* cls)
{

    cls->setName("BurnImpulsive");
    cls->setDesc(u8R"(脉冲点火)");
    cls->addToRegistry();
    cls->setParent<Burn>();
    cls->setConstructor<BurnImpulsive>();

    cls->addProperty("x", aNewPropertyDouble<BurnImpulsive, &BurnImpulsive::x, &BurnImpulsive::setX>());
    cls->addProperty("y", aNewPropertyDouble<BurnImpulsive, &BurnImpulsive::y, &BurnImpulsive::setY>());
    cls->addProperty("z", aNewPropertyDouble<BurnImpulsive, &BurnImpulsive::z, &BurnImpulsive::setZ>());
    cls->addProperty("axes", aNewPropertyObject<BurnImpulsive, Axes, &BurnImpulsive::axes, &BurnImpulsive::setAxes>());
}

AST_NAMESPACE_END
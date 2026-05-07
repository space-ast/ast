#include "StateCartesian.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class StateCartesian::staticType;

static bool StateCartesian_ClassInited = (StateCartesian::ClassInit(&StateCartesian::staticType), true);

void StateCartesian::ClassInit(Class* cls)
{

    cls->setName("StateCartesian");
    cls->setDesc(u8R"(笛卡尔状态(直角坐标))");
    cls->addToRegistry();
    cls->setParent<State>();
    cls->setConstructor<StateCartesian>();

    cls->addProperty("x", aNewPropertyQuantity<StateCartesian, &StateCartesian::x, &StateCartesian::setX>(Dimension::Length()));
    cls->addProperty("y", aNewPropertyQuantity<StateCartesian, &StateCartesian::y, &StateCartesian::setY>(Dimension::Length()));
    cls->addProperty("z", aNewPropertyQuantity<StateCartesian, &StateCartesian::z, &StateCartesian::setZ>(Dimension::Length()));
    cls->addProperty("vx", aNewPropertyQuantity<StateCartesian, &StateCartesian::vx, &StateCartesian::setVx>(Dimension::Speed()));
    cls->addProperty("vy", aNewPropertyQuantity<StateCartesian, &StateCartesian::vy, &StateCartesian::setVy>(Dimension::Speed()));
    cls->addProperty("vz", aNewPropertyQuantity<StateCartesian, &StateCartesian::vz, &StateCartesian::setVz>(Dimension::Speed()));
}

AST_NAMESPACE_END
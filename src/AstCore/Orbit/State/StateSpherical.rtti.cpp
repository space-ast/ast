#include "StateSpherical.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class StateSpherical::staticType;

static bool StateSpherical_ClassInited = (StateSpherical::ClassInit(&StateSpherical::staticType), true);

void StateSpherical::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "StateSpherical"));
    cls->setDesc(u8R"(球坐标状态)");
    cls->addToRegistry();
    cls->setParent<State>();
    cls->setConstructor<StateSpherical>();

    cls->addProperty("RA", aNewPropertyQuantity<StateSpherical, &StateSpherical::getRA, &StateSpherical::setRA>(Dimension::Angle()));
    cls->addProperty("Dec", aNewPropertyQuantity<StateSpherical, &StateSpherical::getDec, &StateSpherical::setDec>(Dimension::Angle()));
    cls->addProperty("Radius", aNewPropertyQuantity<StateSpherical, &StateSpherical::getRadius, &StateSpherical::setRadius>(Dimension::Length()));
    cls->addProperty("FltPathAng", aNewPropertyQuantity<StateSpherical, &StateSpherical::getFltPathAng, &StateSpherical::setFltPathAng>(Dimension::Angle()));
    cls->addProperty("FltPathAzi", aNewPropertyQuantity<StateSpherical, &StateSpherical::getFltPathAzi, &StateSpherical::setFltPathAzi>(Dimension::Angle()));
    cls->addProperty("Vel", aNewPropertyQuantity<StateSpherical, &StateSpherical::getVel, &StateSpherical::setVel>(Dimension::Speed()));
}

AST_NAMESPACE_END
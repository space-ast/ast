#include "StateMixedSpherical.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class StateMixedSpherical::staticType;

static bool StateMixedSpherical_ClassInited = (StateMixedSpherical::ClassInit(&StateMixedSpherical::staticType), true);

void StateMixedSpherical::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "StateMixedSpherical"));
    cls->setDesc(u8R"(混合球坐标状态)");
    cls->addToRegistry();
    cls->setParent<State>();
    cls->setConstructor<StateMixedSpherical>();

    cls->addProperty("Lon", aNewPropertyQuantity<StateMixedSpherical, &StateMixedSpherical::getLon, &StateMixedSpherical::setLon>(Dimension::Angle()));
    cls->addProperty("Lat", aNewPropertyQuantity<StateMixedSpherical, &StateMixedSpherical::getLat, &StateMixedSpherical::setLat>(Dimension::Angle()));
    cls->addProperty("Alt", aNewPropertyQuantity<StateMixedSpherical, &StateMixedSpherical::getAlt, &StateMixedSpherical::setAlt>(Dimension::Length()));
    cls->addProperty("FltPathAng", aNewPropertyQuantity<StateMixedSpherical, &StateMixedSpherical::getFltPathAng, &StateMixedSpherical::setFltPathAng>(Dimension::Angle()));
    cls->addProperty("VertFltPathAng", aNewPropertyQuantity<StateMixedSpherical, &StateMixedSpherical::getVertFltPathAng, &StateMixedSpherical::setVertFltPathAng>(Dimension::Angle()));
    cls->addProperty("FltPathAzi", aNewPropertyQuantity<StateMixedSpherical, &StateMixedSpherical::getFltPathAzi, &StateMixedSpherical::setFltPathAzi>(Dimension::Angle()));
    cls->addProperty("Vel", aNewPropertyQuantity<StateMixedSpherical, &StateMixedSpherical::getVel, &StateMixedSpherical::setVel>(Dimension::Speed()));
}

AST_NAMESPACE_END
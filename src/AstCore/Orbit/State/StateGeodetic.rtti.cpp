#include "StateGeodetic.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class StateGeodetic::staticType;

static bool StateGeodetic_ClassInited = (StateGeodetic::ClassInit(&StateGeodetic::staticType), true);

void StateGeodetic::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "StateGeodetic"));
    cls->setDesc(u8R"(大地坐标状态)");
    cls->addToRegistry();
    cls->setParent<State>();
    cls->setConstructor<StateGeodetic>();

    cls->addProperty("Lon", aNewPropertyQuantity<StateGeodetic, &StateGeodetic::getLon, &StateGeodetic::setLon>(Dimension::Angle()));
    cls->addProperty("Lat", aNewPropertyQuantity<StateGeodetic, &StateGeodetic::getLat, &StateGeodetic::setLat>(Dimension::Angle()));
    cls->addProperty("Alt", aNewPropertyQuantity<StateGeodetic, &StateGeodetic::getAlt, &StateGeodetic::setAlt>(Dimension::Length()));
    cls->addProperty("LonRate", aNewPropertyQuantity<StateGeodetic, &StateGeodetic::getLonRate, &StateGeodetic::setLonRate>(Dimension::AngularVelocity()));
    cls->addProperty("LatRate", aNewPropertyQuantity<StateGeodetic, &StateGeodetic::getLatRate, &StateGeodetic::setLatRate>(Dimension::AngularVelocity()));
    cls->addProperty("AltRate", aNewPropertyQuantity<StateGeodetic, &StateGeodetic::getAltRate, &StateGeodetic::setAltRate>(Dimension::Speed()));
}

AST_NAMESPACE_END

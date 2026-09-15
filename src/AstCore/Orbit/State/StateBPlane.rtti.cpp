#include "StateBPlane.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class StateBPlane::staticType;

static bool StateBPlane_ClassInited = (StateBPlane::ClassInit(&StateBPlane::staticType), true);

void StateBPlane::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "StateBPlane"));
    cls->setDesc(u8R"(B平面状态)");
    cls->addToRegistry();
    cls->setParent<State>();
    cls->setConstructor<StateBPlane>();

    cls->addProperty("RA", aNewPropertyQuantity<StateBPlane, &StateBPlane::getRA, &StateBPlane::setRA>(Dimension::Angle()));
    cls->addProperty("Dec", aNewPropertyQuantity<StateBPlane, &StateBPlane::getDec, &StateBPlane::setDec>(Dimension::Angle()));
    cls->addProperty("BDotR", aNewPropertyQuantity<StateBPlane, &StateBPlane::getBDotR, &StateBPlane::setBDotR>(Dimension::Length()));
    cls->addProperty("BDotT", aNewPropertyQuantity<StateBPlane, &StateBPlane::getBDotT, &StateBPlane::setBDotT>(Dimension::Length()));
    cls->addProperty("C3", aNewPropertyDouble<StateBPlane, &StateBPlane::getC3, &StateBPlane::setC3>());
    cls->addProperty("TrueAnomaly", aNewPropertyQuantity<StateBPlane, &StateBPlane::getTrueAnomaly, &StateBPlane::setTrueAnomaly>(Dimension::Angle()));
}

AST_NAMESPACE_END
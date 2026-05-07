#include "Facility.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Facility::staticType;

static bool Facility_ClassInited = (Facility::ClassInit(&Facility::staticType), true);

void Facility::ClassInit(Class* cls)
{

    cls->setName("Facility");
    cls->setDesc(u8R"(设施对象)");
    cls->addToRegistry();
    cls->setParent<Point>();
    cls->setConstructor<Facility>();

    cls->addProperty("latitude", aNewPropertyQuantity<Facility, &Facility::latitude, &Facility::setLatitude>(Dimension::Angle()));
    cls->addProperty("longitude", aNewPropertyQuantity<Facility, &Facility::longitude, &Facility::setLongitude>(Dimension::Angle()));
    cls->addProperty("altitude", aNewPropertyQuantity<Facility, &Facility::altitude, &Facility::setAltitude>(Dimension::Length()));
    cls->addProperty("body", aNewPropertyObject<Facility, Body, &Facility::body, &Facility::setBody>());
}

AST_NAMESPACE_END
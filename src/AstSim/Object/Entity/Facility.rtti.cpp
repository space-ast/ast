#include "Facility.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Facility::staticType;

Class* Facility::StaticType()
{
    return &staticType;
}

Class* Facility::getType() const
{
    return &staticType;
}

static bool Facility_ClassInited = (Facility::ClassInit(&Facility::staticType), true);

void Facility::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "Facility"));
    cls->setDesc(u8R"(设施对象)");
    cls->addToRegistry();
    cls->setParent<Platform>();
    cls->setConstructor<Facility>();

    cls->addProperty("latitude", aNewPropertyQuantity<Facility, &Facility::latitude, &Facility::setLatitude>(Dimension::Angle()));
    cls->addProperty("longitude", aNewPropertyQuantity<Facility, &Facility::longitude, &Facility::setLongitude>(Dimension::Angle()));
    cls->addProperty("altitude", aNewPropertyQuantity<Facility, &Facility::altitude, &Facility::setAltitude>(Dimension::Length()));
    cls->addProperty("body", aNewPropertyObject<Facility, Body, &Facility::body, &Facility::setBody>());
}

AST_NAMESPACE_END
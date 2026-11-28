#include "Place.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Place::staticType;

Class* Place::StaticType()
{
    return &staticType;
}

Class* Place::getType() const
{
    return &staticType;
}

static bool Place_ClassInited = (Place::ClassInit(&Place::staticType), true);

void Place::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "Place"));
    cls->setDesc(u8R"(地点对象)");
    cls->addToRegistry();
    cls->setParent<Facility>();
    cls->setConstructor<Place>();

}

AST_NAMESPACE_END
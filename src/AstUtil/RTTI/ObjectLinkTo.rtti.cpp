#include "ObjectLinkTo.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class ObjectLinkTo::staticType;

Class* ObjectLinkTo::StaticType()
{
    return &staticType;
}

Class* ObjectLinkTo::getType() const
{
    return &staticType;
}

static bool ObjectLinkTo_ClassInited = (ObjectLinkTo::ClassInit(&ObjectLinkTo::staticType), true);

void ObjectLinkTo::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "ObjectLinkTo"));
    cls->setDesc(u8R"(链接到对象)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<ObjectLinkTo>();

}

AST_NAMESPACE_END
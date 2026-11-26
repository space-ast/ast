#include "Access.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Access::staticType;

Class* Access::StaticType()
{
    return &staticType;
}

Class* Access::getType() const
{
    return &staticType;
}

static bool Access_ClassInited = (Access::ClassInit(&Access::staticType), true);

void Access::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "Access"));
    cls->setDesc(u8R"(访问对象)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<Access>();

    cls->addProperty("BaseObject", aNewPropertyObject<Access, Object, &Access::baseObject, &Access::setBaseObject>());
    cls->addProperty("TargetObject", aNewPropertyObject<Access, Object, &Access::targetObject, &Access::setTargetObject>());
}

AST_NAMESPACE_END
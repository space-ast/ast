#include "GravityForce.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class GravityForce::staticType;

Class* GravityForce::StaticType()
{
    return &staticType;
}

Class* GravityForce::getType() const
{
    return &staticType;
}

static bool GravityForce_ClassInited = (GravityForce::ClassInit(&GravityForce::staticType), true);

void GravityForce::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "GravityForce"));
    cls->addToRegistry();
    cls->setParent<BodyAttraction>();
    cls->setConstructor<GravityForce>();

}

AST_NAMESPACE_END
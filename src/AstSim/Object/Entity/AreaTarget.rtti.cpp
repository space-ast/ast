#include "AreaTarget.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AreaTarget::staticType;

Class* AreaTarget::StaticType()
{
    return &staticType;
}

Class* AreaTarget::getType() const
{
    return &staticType;
}

static bool AreaTarget_ClassInited = (AreaTarget::ClassInit(&AreaTarget::staticType), true);

void AreaTarget::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "AreaTarget"));
    cls->setDesc(u8R"(区域目标对象)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<AreaTarget>();

}

AST_NAMESPACE_END
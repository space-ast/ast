#include "TargeterProfile.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class TargeterProfile::staticType;

Class* TargeterProfile::StaticType()
{
    return &staticType;
}

Class* TargeterProfile::getType() const
{
    return &staticType;
}

static bool TargeterProfile_ClassInited = (TargeterProfile::ClassInit(&TargeterProfile::staticType), true);

void TargeterProfile::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "TargeterProfile"));
    cls->setDesc(u8R"(目标器配置文件基类)");
    cls->addToRegistry();
    cls->setParent<Command>();
    cls->setConstructor<TargeterProfile>();

}

AST_NAMESPACE_END
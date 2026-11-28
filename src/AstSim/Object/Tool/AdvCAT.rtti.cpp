#include "AdvCAT.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AdvCAT::staticType;

Class* AdvCAT::StaticType()
{
    return &staticType;
}

Class* AdvCAT::getType() const
{
    return &staticType;
}

static bool AdvCAT_ClassInited = (AdvCAT::ClassInit(&AdvCAT::staticType), true);

void AdvCAT::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "AdvCAT"));
    cls->setDesc(u8R"(高级接近分析工具)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<AdvCAT>();

}

AST_NAMESPACE_END
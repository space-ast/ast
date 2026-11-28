#include "Class.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Class::staticType;

Class* Class::StaticType()
{
    return &staticType;
}

Class* Class::getType() const
{
    return &staticType;
}

static bool Class_ClassInited = (Class::ClassInit(&Class::staticType), true);

void Class::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "Class"));
    cls->setDesc(u8R"(类元信息)");
    cls->addToRegistry();
    cls->setParent<Struct>();
    cls->setConstructor<Class>();

}

AST_NAMESPACE_END
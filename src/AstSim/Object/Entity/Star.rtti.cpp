#include "Star.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Star::staticType;

Class* Star::StaticType()
{
    return &staticType;
}

Class* Star::getType() const
{
    return &staticType;
}

static bool Star_ClassInited = (Star::ClassInit(&Star::staticType), true);

void Star::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "Star"));
    cls->setDesc(u8R"(恒星对象)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<Star>();

}

AST_NAMESPACE_END
#include "EventTimeExplicit.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class EventTimeExplicit::staticType;

Class* EventTimeExplicit::StaticType()
{
    return &staticType;
}

Class* EventTimeExplicit::getType() const
{
    return &staticType;
}

static bool EventTimeExplicit_ClassInited = (EventTimeExplicit::ClassInit(&EventTimeExplicit::staticType), true);

void EventTimeExplicit::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "EventTimeExplicit"));
    cls->setDesc(u8R"(显式事件时间)");
    cls->addToRegistry();
    cls->setParent<EventTime>();
    cls->setConstructor<EventTimeExplicit>();

}

AST_NAMESPACE_END
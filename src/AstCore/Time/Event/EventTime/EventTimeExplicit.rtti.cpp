#include "EventTimeExplicit.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class EventTimeExplicit::staticType;

static bool EventTimeExplicit_ClassInited = (EventTimeExplicit::ClassInit(&EventTimeExplicit::staticType), true);

void EventTimeExplicit::ClassInit(Class* cls)
{

    cls->setName("EventTimeExplicit");
    cls->setDesc(u8R"(显式事件时间)");
    cls->addToRegistry();
    cls->setParent<EventTime>();
    cls->setConstructor<EventTimeExplicit>();

    cls->addProperty("Time", aNewPropertyTimePoint<EventTimeExplicit, &EventTimeExplicit::getTime, &EventTimeExplicit::setTime>());
}

AST_NAMESPACE_END
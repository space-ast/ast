#include "EventTime.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class EventTime::staticType;

static bool EventTime_ClassInited = (EventTime::ClassInit(&EventTime::staticType), true);

void EventTime::ClassInit(Class* cls)
{

    cls->setName("EventTime");
    cls->setDesc(u8R"(事件时间)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<EventTime>();
    cls->setResolve<EventTime>();

}

AST_NAMESPACE_END
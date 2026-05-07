#include "EventDetector.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class EventDetector::staticType;

static bool EventDetector_ClassInited = (EventDetector::ClassInit(&EventDetector::staticType), true);

void EventDetector::ClassInit(Class* cls)
{

    cls->setName("EventDetector");
    cls->setDesc(u8R"(事件检测基类 事件检测基类，用于检测事件是否发生。 参考orekit的EventDetector类)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<EventDetector>();

    cls->addProperty("active", aNewPropertyBool<EventDetector, &EventDetector::active, &EventDetector::setActive>());
    cls->addProperty("repeatCount", aNewPropertyInt<EventDetector, &EventDetector::repeatCount, &EventDetector::setRepeatCount>());
    cls->addProperty("threshold", aNewPropertyDouble<EventDetector, &EventDetector::threshold, &EventDetector::setThreshold>());
    cls->addProperty("goal", aNewPropertyDouble<EventDetector, &EventDetector::goal, &EventDetector::setGoal>());
}

AST_NAMESPACE_END
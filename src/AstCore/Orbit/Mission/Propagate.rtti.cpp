#include "Propagate.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Propagate::staticType;

static bool Propagate_ClassInited = (Propagate::ClassInit(&Propagate::staticType), true);

void Propagate::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "Propagate"));
    cls->setDesc(u8R"(预报段，用于建模任务序列(MissionCommand)中的轨道预报)");
    cls->addToRegistry();
    cls->setParent<Segment>();
    cls->setConstructor<Propagate>();

    cls->addProperty("minPropTime", aNewPropertyDouble<Propagate, &Propagate::minPropTime, &Propagate::setMinPropTime>());
    cls->addProperty("maxPropTime", aNewPropertyDouble<Propagate, &Propagate::maxPropTime, &Propagate::setMaxPropTime>());
    cls->addProperty("useMaxPropTime", aNewPropertyBool<Propagate, &Propagate::useMaxPropTime, &Propagate::setUseMaxPropTime>());
    cls->addProperty("useMaxPropTimeWarn", aNewPropertyBool<Propagate, &Propagate::useMaxPropTimeWarn, &Propagate::setUseMaxPropTimeWarn>());
    cls->addProperty("overrideMaxPropTime", aNewPropertyBool<Propagate, &Propagate::overrideMaxPropTime, &Propagate::setOverrideMaxPropTime>());
}

AST_NAMESPACE_END
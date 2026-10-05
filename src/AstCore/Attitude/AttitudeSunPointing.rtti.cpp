#include "AttitudeSunPointing.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AttitudeSunPointing::staticType;

static bool AttitudeSunPointing_ClassInited = (AttitudeSunPointing::ClassInit(&AttitudeSunPointing::staticType), true);

void AttitudeSunPointing::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "AttitudeSunPointing"));
    cls->setDesc(u8R"(太阳指向姿态)");
    cls->addToRegistry();
    cls->setParent<AttitudeSunRelated>();
    cls->setConstructor<AttitudeSunPointing>();

}

AST_NAMESPACE_END
#include "AttitudeSunPointingEclpNormal.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AttitudeSunPointingEclpNormal::staticType;

static bool AttitudeSunPointingEclpNormal_ClassInited = (AttitudeSunPointingEclpNormal::ClassInit(&AttitudeSunPointingEclpNormal::staticType), true);

void AttitudeSunPointingEclpNormal::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "AttitudeSunPointingEclpNormal"));
    cls->setDesc(u8R"(太阳指向 + 黄道法向约束姿态)");
    cls->addToRegistry();
    cls->setParent<AttitudeSunRelated>();
    cls->setConstructor<AttitudeSunPointingEclpNormal>();

}

AST_NAMESPACE_END
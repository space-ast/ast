#include "AttitudeSunPointingCbiZ.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AttitudeSunPointingCbiZ::staticType;

static bool AttitudeSunPointingCbiZ_ClassInited = (AttitudeSunPointingCbiZ::ClassInit(&AttitudeSunPointingCbiZ::staticType), true);

void AttitudeSunPointingCbiZ::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "AttitudeSunPointingCbiZ"));
    cls->setDesc(u8R"(太阳指向 + 天体惯性系Z轴方向约束)");
    cls->addToRegistry();
    cls->setParent<AttitudeSunRelated>();
    cls->setConstructor<AttitudeSunPointingCbiZ>();

}

AST_NAMESPACE_END
#include "GroundStation.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class GroundStation::staticType;

static bool GroundStation_ClassInited = (GroundStation::ClassInit(&GroundStation::staticType), true);

void GroundStation::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "GroundStation"));
    cls->setDesc(u8R"(地面站)");
    cls->addToRegistry();
    cls->setParent<Facility>();
    cls->setConstructor<GroundStation>();

}

AST_NAMESPACE_END
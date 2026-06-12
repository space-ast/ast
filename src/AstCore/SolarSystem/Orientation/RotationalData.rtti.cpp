#include "RotationalData.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class RotationalData::staticType;

static bool RotationalData_ClassInited = (RotationalData::ClassInit(&RotationalData::staticType), true);

void RotationalData::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "RotationalData"));
    cls->setDesc(u8R"(行星旋转数据)");
    cls->addToRegistry();
    cls->setParent<BodyOrientation>();
    cls->setConstructor<RotationalData>();

}

AST_NAMESPACE_END
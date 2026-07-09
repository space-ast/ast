#include "Angle.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Angle::staticType;

static bool Angle_ClassInited = (Angle::ClassInit(&Angle::staticType), true);

void Angle::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "Angle"));
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<Angle>();

}

AST_NAMESPACE_END

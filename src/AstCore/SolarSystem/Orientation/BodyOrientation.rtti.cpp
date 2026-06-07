#include "BodyOrientation.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class BodyOrientation::staticType;

static bool BodyOrientation_ClassInited = (BodyOrientation::ClassInit(&BodyOrientation::staticType), true);

void BodyOrientation::ClassInit(Class* cls)
{

    cls->setName(N_("BodyOrientation"));
    cls->setDesc(u8R"(天体姿态/指向)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<BodyOrientation>();

}

AST_NAMESPACE_END
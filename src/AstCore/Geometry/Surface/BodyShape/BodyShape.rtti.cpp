#include "BodyShape.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class BodyShape::staticType;

static bool BodyShape_ClassInited = (BodyShape::ClassInit(&BodyShape::staticType), true);

void BodyShape::ClassInit(Class* cls)
{

    cls->setName(N_("BodyShape"));
    cls->setDesc(u8R"(天体形状基类)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<BodyShape>();

}

AST_NAMESPACE_END
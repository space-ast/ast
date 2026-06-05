#include "BodyAttraction.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class BodyAttraction::staticType;

static bool BodyAttraction_ClassInited = (BodyAttraction::ClassInit(&BodyAttraction::staticType), true);

void BodyAttraction::ClassInit(Class* cls)
{

    cls->setName("BodyAttraction");
    cls->setDesc(u8R"(天体引力模型)");
    cls->addToRegistry();
    cls->setParent<ForceModel>();
    cls->setConstructor<BodyAttraction>();

}

AST_NAMESPACE_END
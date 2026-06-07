#include "ForceModel.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class ForceModel::staticType;

static bool ForceModel_ClassInited = (ForceModel::ClassInit(&ForceModel::staticType), true);

void ForceModel::ClassInit(Class* cls)
{

    cls->setName(N_("ForceModel"));
    cls->setDesc(u8R"(力模型)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<ForceModel>();

}

AST_NAMESPACE_END
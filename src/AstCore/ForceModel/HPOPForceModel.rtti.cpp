#include "HPOPForceModel.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class HPOPForceModel::staticType;

static bool HPOPForceModel_ClassInited = (HPOPForceModel::ClassInit(&HPOPForceModel::staticType), true);

void HPOPForceModel::ClassInit(Class* cls)
{

    cls->setName("HPOPForceModel");
    cls->setDesc(u8R"(高精度轨道预报力模型)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<HPOPForceModel>();

}

AST_NAMESPACE_END
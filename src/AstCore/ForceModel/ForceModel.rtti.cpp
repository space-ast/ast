#include "ForceModel.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class ForceModel::staticType;

Class* ForceModel::StaticType()
{
    return &staticType;
}

Class* ForceModel::getType() const
{
    return &staticType;
}

static bool ForceModel_ClassInited = (ForceModel::ClassInit(&ForceModel::staticType), true);

void ForceModel::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "ForceModel"));
    cls->setDesc(u8R"(力模型)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<ForceModel>();

}

AST_NAMESPACE_END
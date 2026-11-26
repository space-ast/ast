#include "RelativityCorrection.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class RelativityCorrection::staticType;

Class* RelativityCorrection::StaticType()
{
    return &staticType;
}

Class* RelativityCorrection::getType() const
{
    return &staticType;
}

static bool RelativityCorrection_ClassInited = (RelativityCorrection::ClassInit(&RelativityCorrection::staticType), true);

void RelativityCorrection::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "RelativityCorrection"));
    cls->setDesc(u8R"(相对论修正模型)");
    cls->addToRegistry();
    cls->setParent<ForceModel>();
    cls->setConstructor<RelativityCorrection>();

}

AST_NAMESPACE_END
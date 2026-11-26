#include "ODEFixedStepIntegrator.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class ODEFixedStepIntegrator::staticType;

Class* ODEFixedStepIntegrator::StaticType()
{
    return &staticType;
}

Class* ODEFixedStepIntegrator::getType() const
{
    return &staticType;
}

static bool ODEFixedStepIntegrator_ClassInited = (ODEFixedStepIntegrator::ClassInit(&ODEFixedStepIntegrator::staticType), true);

void ODEFixedStepIntegrator::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "ODEFixedStepIntegrator"));
    cls->setDesc(u8R"(固定步长积分器)");
    cls->addToRegistry();
    cls->setParent<ODEIntegrator>();
    cls->setConstructor<ODEFixedStepIntegrator>();

}

AST_NAMESPACE_END
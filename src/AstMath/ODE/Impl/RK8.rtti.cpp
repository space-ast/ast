#include "RK8.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class RK8::staticType;

Class* RK8::StaticType()
{
    return &staticType;
}

Class* RK8::getType() const
{
    return &staticType;
}

static bool RK8_ClassInited = (RK8::ClassInit(&RK8::staticType), true);

void RK8::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "RK8"));
    cls->setDesc(u8R"(RK8 积分器)");
    cls->addToRegistry();
    cls->setParent<ODEFixedStepIntegrator>();
    cls->setConstructor<RK8>();

}

AST_NAMESPACE_END
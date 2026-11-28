#include "RKF56.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class RKF56::staticType;

Class* RKF56::StaticType()
{
    return &staticType;
}

Class* RKF56::getType() const
{
    return &staticType;
}

static bool RKF56_ClassInited = (RKF56::ClassInit(&RKF56::staticType), true);

void RKF56::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "RKF56"));
    cls->setDesc(u8R"(Runge-Kutta-Fehlberg 5(6) 积分器)");
    cls->addToRegistry();
    cls->setParent<ODEVarStepIntegrator>();
    cls->setConstructor<RKF56>();

}

AST_NAMESPACE_END
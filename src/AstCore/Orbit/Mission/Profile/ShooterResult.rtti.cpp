#include "ShooterResult.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class ShooterResult::staticType;

static bool ShooterResult_ClassInited = (ShooterResult::ClassInit(&ShooterResult::staticType), true);

void ShooterResult::ClassInit(Class* cls)
{

    cls->setName("ShooterResult");
    cls->setDesc(u8R"(结果变量，用于定义微分修正中的结果变量)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<ShooterResult>();

    cls->addProperty("active", aNewPropertyBool<ShooterResult, &ShooterResult::active, &ShooterResult::setActive>());
    cls->addProperty("desired", aNewPropertyDouble<ShooterResult, &ShooterResult::desired, &ShooterResult::setDesired>());
    cls->addProperty("scale", aNewPropertyDouble<ShooterResult, &ShooterResult::scale, &ShooterResult::setScale>());
    cls->addProperty("tolerance", aNewPropertyDouble<ShooterResult, &ShooterResult::tolerance, &ShooterResult::setTolerance>());
    cls->addProperty("valid", aNewPropertyBool<ShooterResult, &ShooterResult::valid, &ShooterResult::setValid>());
    cls->addProperty("weight", aNewPropertyDouble<ShooterResult, &ShooterResult::weight, &ShooterResult::setWeight>());
}

AST_NAMESPACE_END
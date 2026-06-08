#include "FeasibleRegionStudy.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class FeasibleRegionVariable::staticType;

static bool FeasibleRegionVariable_ClassInited = (FeasibleRegionVariable::ClassInit(&FeasibleRegionVariable::staticType), true);

void FeasibleRegionVariable::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "FeasibleRegionVariable"));
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<FeasibleRegionVariable>();

}

AST_NAMESPACE_END

AST_NAMESPACE_BEGIN

Class FeasibleRegionConstraint::staticType;

static bool FeasibleRegionConstraint_ClassInited = (FeasibleRegionConstraint::ClassInit(&FeasibleRegionConstraint::staticType), true);

void FeasibleRegionConstraint::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "FeasibleRegionConstraint"));
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<FeasibleRegionConstraint>();

}

AST_NAMESPACE_END

AST_NAMESPACE_BEGIN

Class FeasibleRegionStudy::staticType;

static bool FeasibleRegionStudy_ClassInited = (FeasibleRegionStudy::ClassInit(&FeasibleRegionStudy::staticType), true);

void FeasibleRegionStudy::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "FeasibleRegionStudy"));
    cls->setDesc(u8R"(可行域研究，用于搜索满足约束的解空间)");
    cls->addToRegistry();
    cls->setParent<Command>();
    cls->setConstructor<FeasibleRegionStudy>();

}

AST_NAMESPACE_END
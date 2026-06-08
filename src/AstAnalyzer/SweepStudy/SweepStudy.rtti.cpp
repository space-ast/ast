#include "SweepStudy.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class SweepVariable::staticType;

static bool SweepVariable_ClassInited = (SweepVariable::ClassInit(&SweepVariable::staticType), true);

void SweepVariable::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "SweepVariable"));
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<SweepVariable>();

}

AST_NAMESPACE_END

AST_NAMESPACE_BEGIN

Class SweepOutput::staticType;

static bool SweepOutput_ClassInited = (SweepOutput::ClassInit(&SweepOutput::staticType), true);

void SweepOutput::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "SweepOutput"));
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<SweepOutput>();

}

AST_NAMESPACE_END

AST_NAMESPACE_BEGIN

Class SweepStudy::staticType;

static bool SweepStudy_ClassInited = (SweepStudy::ClassInit(&SweepStudy::staticType), true);

void SweepStudy::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "SweepStudy"));
    cls->setDesc(u8R"(遍历搜索分析器（参数化扫描），对变量进行嵌套循环遍历并执行仿真命令)");
    cls->addToRegistry();
    cls->setParent<Command>();
    cls->setConstructor<SweepStudy>();

}

AST_NAMESPACE_END
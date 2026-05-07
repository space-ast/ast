#include "AttitudeCoverage.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AttitudeCoverage::staticType;

static bool AttitudeCoverage_ClassInited = (AttitudeCoverage::ClassInit(&AttitudeCoverage::staticType), true);

void AttitudeCoverage::ClassInit(Class* cls)
{

    cls->setName("AttitudeCoverage");
    cls->setDesc(u8R"(姿态覆盖工具)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<AttitudeCoverage>();

}

AST_NAMESPACE_END
#include "UncertaintyStudy.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class UncertaintyStudy::staticType;

static bool UncertaintyStudy_ClassInited = (UncertaintyStudy::ClassInit(&UncertaintyStudy::staticType), true);

void UncertaintyStudy::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "UncertaintyStudy"));
    cls->addToRegistry();
    cls->setParent<Command>();
    cls->setConstructor<UncertaintyStudy>();

}

AST_NAMESPACE_END
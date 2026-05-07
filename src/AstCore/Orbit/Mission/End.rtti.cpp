#include "End.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class End::staticType;

static bool End_ClassInited = (End::ClassInit(&End::staticType), true);

void End::ClassInit(Class* cls)
{

    cls->setName("End");
    cls->setDesc(u8R"(结束段，仅用于标识任务序列Sequence的结束)");
    cls->addToRegistry();
    cls->setParent<MissionCommand>();
    cls->setConstructor<End>();

}

AST_NAMESPACE_END
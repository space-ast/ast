#include "Return.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Return::staticType;

static bool Return_ClassInited = (Return::ClassInit(&Return::staticType), true);

void Return::ClassInit(Class* cls)
{

    cls->setName("Return");
    cls->setDesc(u8R"(返回段，用于停止当前任务序列Sequence的执行，并返回到上一级的Sequence)");
    cls->addToRegistry();
    cls->setParent<MissionCommand>();
    cls->setConstructor<Return>();

    cls->addProperty("enabled", aNewPropertyBool<Return, &Return::enabled, &Return::setEnabled>());
}

AST_NAMESPACE_END
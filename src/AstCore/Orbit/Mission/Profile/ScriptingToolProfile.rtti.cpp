#include "ScriptingToolProfile.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class ScriptingToolProfile::staticType;

static bool ScriptingToolProfile_ClassInited = (ScriptingToolProfile::ClassInit(&ScriptingToolProfile::staticType), true);

void ScriptingToolProfile::ClassInit(Class* cls)
{

    cls->setName("ScriptingToolProfile");
    cls->setDesc(u8R"(脚本工具配置文件)");
    cls->addToRegistry();
    cls->setParent<TargeterProfile>();
    cls->setConstructor<ScriptingToolProfile>();

}

AST_NAMESPACE_END
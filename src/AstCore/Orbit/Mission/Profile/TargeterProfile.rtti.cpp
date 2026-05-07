#include "TargeterProfile.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class TargeterProfile::staticType;

static bool TargeterProfile_ClassInited = (TargeterProfile::ClassInit(&TargeterProfile::staticType), true);

void TargeterProfile::ClassInit(Class* cls)
{

    cls->setName("TargeterProfile");
    cls->setDesc(u8R"(目标器配置文件基类)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<TargeterProfile>();

}

AST_NAMESPACE_END
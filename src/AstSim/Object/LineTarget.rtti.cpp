#include "LineTarget.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class LineTarget::staticType;

static bool LineTarget_ClassInited = (LineTarget::ClassInit(&LineTarget::staticType), true);

void LineTarget::ClassInit(Class* cls)
{

    cls->setName("LineTarget");
    cls->setDesc(u8R"(线目标对象)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<LineTarget>();

}

AST_NAMESPACE_END
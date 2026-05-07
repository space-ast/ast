#include "MTO.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class MTO::staticType;

static bool MTO_ClassInited = (MTO::ClassInit(&MTO::staticType), true);

void MTO::ClassInit(Class* cls)
{

    cls->setName("MTO");
    cls->setDesc(u8R"(多目标对象)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<MTO>();

}

AST_NAMESPACE_END
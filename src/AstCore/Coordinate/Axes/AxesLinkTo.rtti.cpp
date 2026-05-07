#include "AxesLinkTo.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AxesLinkTo::staticType;

static bool AxesLinkTo_ClassInited = (AxesLinkTo::ClassInit(&AxesLinkTo::staticType), true);

void AxesLinkTo::ClassInit(Class* cls)
{

    cls->setName("AxesLinkTo");
    cls->addToRegistry();
    cls->setParent<Axes>();
    cls->setConstructor<AxesLinkTo>();

}

AST_NAMESPACE_END
#include "AxesLinkTo.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AxesLinkTo::staticType;

Class* AxesLinkTo::StaticType()
{
    return &staticType;
}

Class* AxesLinkTo::getType() const
{
    return &staticType;
}

static bool AxesLinkTo_ClassInited = (AxesLinkTo::ClassInit(&AxesLinkTo::staticType), true);

void AxesLinkTo::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "AxesLinkTo"));
    cls->addToRegistry();
    cls->setParent<Axes>();
    cls->setConstructor<AxesLinkTo>();

}

AST_NAMESPACE_END
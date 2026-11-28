#include "MoonOrientation.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class MoonOrientation::staticType;

Class* MoonOrientation::StaticType()
{
    return &staticType;
}

Class* MoonOrientation::getType() const
{
    return &staticType;
}

static bool MoonOrientation_ClassInited = (MoonOrientation::ClassInit(&MoonOrientation::staticType), true);

void MoonOrientation::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "MoonOrientation"));
    cls->addToRegistry();
    cls->setParent<BodyOrientation>();
    cls->setConstructor<MoonOrientation>();

}

AST_NAMESPACE_END
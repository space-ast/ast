#include "LandingSite.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class LandingSite::staticType;

Class* LandingSite::StaticType()
{
    return &staticType;
}

Class* LandingSite::getType() const
{
    return &staticType;
}

static bool LandingSite_ClassInited = (LandingSite::ClassInit(&LandingSite::staticType), true);

void LandingSite::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "LandingSite"));
    cls->addToRegistry();
    cls->setParent<Segment>();
    cls->setConstructor<LandingSite>();

}

AST_NAMESPACE_END
#include "DetectorCartesian.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class DetectorCartesian::staticType;

Class* DetectorCartesian::StaticType()
{
    return &staticType;
}

Class* DetectorCartesian::getType() const
{
    return &staticType;
}

static bool DetectorCartesian_ClassInited = (DetectorCartesian::ClassInit(&DetectorCartesian::staticType), true);

void DetectorCartesian::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "DetectorCartesian"));
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorCartesian>();

}

AST_NAMESPACE_END
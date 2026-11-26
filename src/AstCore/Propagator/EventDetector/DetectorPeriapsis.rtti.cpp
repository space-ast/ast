#include "DetectorPeriapsis.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class DetectorPeriapsis::staticType;

Class* DetectorPeriapsis::StaticType()
{
    return &staticType;
}

Class* DetectorPeriapsis::getType() const
{
    return &staticType;
}

static bool DetectorPeriapsis_ClassInited = (DetectorPeriapsis::ClassInit(&DetectorPeriapsis::staticType), true);

void DetectorPeriapsis::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "DetectorPeriapsis"));
    cls->setDesc(u8R"(近拱点事件检测器)");
    cls->addToRegistry();
    cls->setParent<DetectorBodyRelated>();
    cls->setConstructor<DetectorPeriapsis>();

}

AST_NAMESPACE_END
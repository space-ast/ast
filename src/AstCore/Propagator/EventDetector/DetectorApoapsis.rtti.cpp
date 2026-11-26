#include "DetectorApoapsis.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class DetectorApoapsis::staticType;

Class* DetectorApoapsis::StaticType()
{
    return &staticType;
}

Class* DetectorApoapsis::getType() const
{
    return &staticType;
}

static bool DetectorApoapsis_ClassInited = (DetectorApoapsis::ClassInit(&DetectorApoapsis::staticType), true);

void DetectorApoapsis::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "DetectorApoapsis"));
    cls->setDesc(u8R"(远拱点事件检测器)");
    cls->addToRegistry();
    cls->setParent<DetectorBodyRelated>();
    cls->setConstructor<DetectorApoapsis>();

}

AST_NAMESPACE_END
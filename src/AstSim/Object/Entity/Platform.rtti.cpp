#include "Platform.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Platform::staticType;

static bool Platform_ClassInited = (Platform::ClassInit(&Platform::staticType), true);

void Platform::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "Platform"));
    cls->setDesc(u8R"(通用平台类，用于表示空间中的平台对象，例如卫星、飞机、地面站等，具有位置、姿态两个属性)");
    cls->addToRegistry();
    cls->setParent<Point>();
    cls->setConstructor<Platform>();

}

AST_NAMESPACE_END
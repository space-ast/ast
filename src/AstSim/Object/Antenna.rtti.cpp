#include "Antenna.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Antenna::staticType;

static bool Antenna_ClassInited = (Antenna::ClassInit(&Antenna::staticType), true);

void Antenna::ClassInit(Class* cls)
{

    cls->setName("Antenna");
    cls->setDesc(u8R"(天线对象)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<Antenna>();

}

AST_NAMESPACE_END
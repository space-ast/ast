#include "Satellite.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Satellite::staticType;

static bool Satellite_ClassInited = (Satellite::ClassInit(&Satellite::staticType), true);

void Satellite::ClassInit(Class* cls)
{

    cls->setName("Satellite");
    cls->setDesc(u8R"(卫星对象)");
    cls->addToRegistry();
    cls->setParent<Spacecraft>();
    cls->setConstructor<Satellite>();

}

AST_NAMESPACE_END
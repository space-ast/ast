#include "Planet.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Planet::staticType;

static bool Planet_ClassInited = (Planet::ClassInit(&Planet::staticType), true);

void Planet::ClassInit(Class* cls)
{

    cls->setName("Planet");
    cls->setDesc(u8R"(行星对象)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<Planet>();

}

AST_NAMESPACE_END
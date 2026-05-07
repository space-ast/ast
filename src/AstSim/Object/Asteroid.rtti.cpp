#include "Asteroid.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Asteroid::staticType;

static bool Asteroid_ClassInited = (Asteroid::ClassInit(&Asteroid::staticType), true);

void Asteroid::ClassInit(Class* cls)
{

    cls->setName("Asteroid");
    cls->setDesc(u8R"(小行星)");
    cls->addToRegistry();
    cls->setParent<SpaceObject>();
    cls->setConstructor<Asteroid>();

}

AST_NAMESPACE_END
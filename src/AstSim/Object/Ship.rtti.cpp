#include "Ship.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Ship::staticType;

static bool Ship_ClassInited = (Ship::ClassInit(&Ship::staticType), true);

void Ship::ClassInit(Class* cls)
{

    cls->setName("Ship");
    cls->setDesc(u8R"(舰船对象)");
    cls->addToRegistry();
    cls->setParent<Mover>();
    cls->setConstructor<Ship>();

}

AST_NAMESPACE_END
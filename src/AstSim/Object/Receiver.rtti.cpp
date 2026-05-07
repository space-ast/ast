#include "Receiver.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Receiver::staticType;

static bool Receiver_ClassInited = (Receiver::ClassInit(&Receiver::staticType), true);

void Receiver::ClassInit(Class* cls)
{

    cls->setName("Receiver");
    cls->setDesc(u8R"(接收器对象)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<Receiver>();

}

AST_NAMESPACE_END
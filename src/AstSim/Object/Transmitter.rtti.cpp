#include "Transmitter.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Transmitter::staticType;

static bool Transmitter_ClassInited = (Transmitter::ClassInit(&Transmitter::staticType), true);

void Transmitter::ClassInit(Class* cls)
{

    cls->setName("Transmitter");
    cls->setDesc(u8R"(发射机对象)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<Transmitter>();

}

AST_NAMESPACE_END
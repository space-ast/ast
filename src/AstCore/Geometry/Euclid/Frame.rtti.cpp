#include "Frame.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Frame::staticType;

Class* Frame::StaticType()
{
    return &staticType;
}

Class* Frame::getType() const
{
    return &staticType;
}

static bool Frame_ClassInited = (Frame::ClassInit(&Frame::staticType), true);

void Frame::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "Frame"));
    cls->setDesc(u8R"(坐标系抽象基类)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<Frame>();
    cls->setResolve<Frame>();

}

AST_NAMESPACE_END
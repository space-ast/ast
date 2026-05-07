#include "FrameAssembly.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class FrameAssembly::staticType;

static bool FrameAssembly_ClassInited = (FrameAssembly::ClassInit(&FrameAssembly::staticType), true);

void FrameAssembly::ClassInit(Class* cls)
{

    cls->setName("FrameAssembly");
    cls->setDesc(u8R"(组装坐标系)");
    cls->addToRegistry();
    cls->setParent<Frame>();
    cls->setConstructor<FrameAssembly>();

    cls->addProperty("Origin", aNewPropertyObject<FrameAssembly, Point, &FrameAssembly::getOrigin, &FrameAssembly::setOrigin>());
    cls->addProperty("Axes", aNewPropertyObject<FrameAssembly, Axes, &FrameAssembly::getAxes, &FrameAssembly::setAxes>());
}

AST_NAMESPACE_END
#include "DetectorFrameRelated.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class DetectorFrameRelated::staticType;

Class* DetectorFrameRelated::StaticType()
{
    return &staticType;
}

Class* DetectorFrameRelated::getType() const
{
    return &staticType;
}

static bool DetectorFrameRelated_ClassInited = (DetectorFrameRelated::ClassInit(&DetectorFrameRelated::staticType), true);

void DetectorFrameRelated::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "DetectorFrameRelated"));
    cls->addToRegistry();
    cls->setParent<EventDetector>();
    cls->setConstructor<DetectorFrameRelated>();

    cls->addProperty("frame", aNewPropertyObject<DetectorFrameRelated, Frame, &DetectorFrameRelated::frame, &DetectorFrameRelated::setFrame>());
}

AST_NAMESPACE_END
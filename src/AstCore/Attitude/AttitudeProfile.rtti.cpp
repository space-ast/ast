#include "AttitudeProfile.hpp"
#include "AstCore/Point.hpp"
#include "AstCore/Frame.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AttitudeProfileBase::staticType;

static bool AttitudeProfileBase_ClassInited = (AttitudeProfileBase::ClassInit(&AttitudeProfileBase::staticType), true);

void AttitudeProfileBase::ClassInit(Class* cls)
{
    cls->setName("AttitudeProfile");
    cls->setDesc(u8R"(姿态剖面抽象基类)");
    cls->addToRegistry();
    cls->setParent<Axes>();
    cls->setConstructor<AttitudeProfile>();

    cls->addProperty("Point", aNewPropertyObject<AttitudeProfileBase, Point, &AttitudeProfileBase::getPoint, &AttitudeProfileBase::setPoint>());
    cls->addProperty("Frame", aNewPropertyObject<AttitudeProfileBase, Frame, &AttitudeProfileBase::getFrame, &AttitudeProfileBase::setFrame>());
}

AST_NAMESPACE_END

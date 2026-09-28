#include "AttitudeProfile.hpp"
#include "AstCore/Point.hpp"
#include "AstCore/Frame.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AttitudeProfile::staticType;

static bool AttitudeProfile_ClassInited = (AttitudeProfile::ClassInit(&AttitudeProfile::staticType), true);

void AttitudeProfile::ClassInit(Class* cls)
{
    cls->setName("AttitudeProfile");
    cls->setDesc(u8R"(姿态剖面抽象基类)");
    cls->addToRegistry();
    cls->setParent<Axes>();
    cls->setConstructor<AttitudeProfile>();

    cls->addProperty("Point", aNewPropertyObject<AttitudeProfile, Point, &AttitudeProfile::getPoint, &AttitudeProfile::setPoint>());
    cls->addProperty("Frame", aNewPropertyObject<AttitudeProfile, Frame, &AttitudeProfile::getFrame, &AttitudeProfile::setFrame>());
}

AST_NAMESPACE_END

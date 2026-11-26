#include "MotionBallistic.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class MotionBallistic::staticType;

Class* MotionBallistic::StaticType()
{
    return &staticType;
}

Class* MotionBallistic::getType() const
{
    return &staticType;
}

static bool MotionBallistic_ClassInited = (MotionBallistic::ClassInit(&MotionBallistic::staticType), true);

void MotionBallistic::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "MotionBallistic"));
    cls->setDesc(u8R"(弹道运动模型，用于模拟弹道导弹运动)");
    cls->addToRegistry();
    cls->setParent<MotionProfile>();
    cls->setConstructor<MotionBallistic>();

}

AST_NAMESPACE_END
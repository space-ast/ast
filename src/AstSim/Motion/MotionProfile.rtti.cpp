#include "MotionProfile.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class MotionProfile::staticType;

static bool MotionProfile_ClassInited = (MotionProfile::ClassInit(&MotionProfile::staticType), true);

void MotionProfile::ClassInit(Class* cls)
{

    cls->setName("MotionProfile");
    cls->setDesc(u8R"(运动定义接口)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<MotionProfile>();

}

AST_NAMESPACE_END
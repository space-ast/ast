#include "BodyEphemerisNoop.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class BodyEphemerisNoop::staticType;

static bool BodyEphemerisNoop_ClassInited = (BodyEphemerisNoop::ClassInit(&BodyEphemerisNoop::staticType), true);

void BodyEphemerisNoop::ClassInit(Class* cls)
{

    cls->setName(N_("BodyEphemerisNoop"));
    cls->addToRegistry();
    cls->setParent<BodyEphemeris>();
    cls->setConstructor<BodyEphemerisNoop>();

}

AST_NAMESPACE_END
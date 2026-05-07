#include "AttitudeFigureOfMerit.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class AttitudeFigureOfMerit::staticType;

static bool AttitudeFigureOfMerit_ClassInited = (AttitudeFigureOfMerit::ClassInit(&AttitudeFigureOfMerit::staticType), true);

void AttitudeFigureOfMerit::ClassInit(Class* cls)
{

    cls->setName("AttitudeFigureOfMerit");
    cls->setDesc(u8R"(姿态覆盖性能指标)");
    cls->addToRegistry();
    cls->setParent<FigureOfMerit>();
    cls->setConstructor<AttitudeFigureOfMerit>();

}

AST_NAMESPACE_END
#include "FigureOfMerit.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class FigureOfMerit::staticType;

static bool FigureOfMerit_ClassInited = (FigureOfMerit::ClassInit(&FigureOfMerit::staticType), true);

void FigureOfMerit::ClassInit(Class* cls)
{

    cls->setName("FigureOfMerit");
    cls->setDesc(u8R"(性能指标对象)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<FigureOfMerit>();

}

AST_NAMESPACE_END
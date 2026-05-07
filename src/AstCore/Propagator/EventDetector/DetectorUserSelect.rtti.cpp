#include "DetectorUserSelect.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class DetectorUserSelect::staticType;

static bool DetectorUserSelect_ClassInited = (DetectorUserSelect::ClassInit(&DetectorUserSelect::staticType), true);

void DetectorUserSelect::ClassInit(Class* cls)
{

    cls->setName("DetectorUserSelect");
    cls->addToRegistry();
    cls->setParent(nullptr);
    cls->setConstructor<DetectorUserSelect>();

    cls->addProperty("calculation", aNewPropertyObject<DetectorUserSelect, ScStateCalculation, &DetectorUserSelect::calculation, &DetectorUserSelect::setCalculation>());
}

AST_NAMESPACE_END
#include "CoverageDefinition.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class CoverageDefinition::staticType;

static bool CoverageDefinition_ClassInited = (CoverageDefinition::ClassInit(&CoverageDefinition::staticType), true);

void CoverageDefinition::ClassInit(Class* cls)
{

    cls->setName("CoverageDefinition");
    cls->setDesc(u8R"(覆盖定义对象)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<CoverageDefinition>();

}

AST_NAMESPACE_END
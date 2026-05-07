#include "Variable.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Variable::staticType;

static bool Variable_ClassInited = (Variable::ClassInit(&Variable::staticType), true);

void Variable::ClassInit(Class* cls)
{

    cls->setName("Variable");
    cls->setDesc(u8R"(变量)");
    cls->addToRegistry();
    cls->setParent<Expr>();
    cls->setConstructor<Variable>();

    cls->addProperty("value", aNewPropertyString<Variable, &Variable::value, &Variable::setValue>());
}

AST_NAMESPACE_END
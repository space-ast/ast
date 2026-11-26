#include "OptimizationStudy.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class OptimizationStudy::staticType;

Class* OptimizationStudy::StaticType()
{
    return &staticType;
}

Class* OptimizationStudy::getType() const
{
    return &staticType;
}

static bool OptimizationStudy_ClassInited = (OptimizationStudy::ClassInit(&OptimizationStudy::staticType), true);

void OptimizationStudy::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "OptimizationStudy"));
    cls->setDesc(u8R"(优化研究)");
    cls->addToRegistry();
    cls->setParent<Command>();
    cls->setConstructor<OptimizationStudy>();

}

AST_NAMESPACE_END
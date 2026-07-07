#include "StudyWorkbench.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class StudyWorkbench::staticType;

static bool StudyWorkbench_ClassInited = (StudyWorkbench::ClassInit(&StudyWorkbench::staticType), true);

void StudyWorkbench::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "StudyWorkbench"));
    cls->setDesc(u8R"(研究工作台，用于参数扫描研究、优化研究、不确定性分析、区间分析等分析)");
    cls->addToRegistry();
    cls->setParent<Command>();
    cls->setConstructor<StudyWorkbench>();

}

AST_NAMESPACE_END
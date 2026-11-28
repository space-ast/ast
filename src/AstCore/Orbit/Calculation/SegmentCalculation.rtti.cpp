#include "SegmentCalculation.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class SegmentCalculation::staticType;

Class* SegmentCalculation::StaticType()
{
    return &staticType;
}

Class* SegmentCalculation::getType() const
{
    return &staticType;
}

static bool SegmentCalculation_ClassInited = (SegmentCalculation::ClassInit(&SegmentCalculation::staticType), true);

void SegmentCalculation::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "SegmentCalculation"));
    cls->addToRegistry();
    cls->setParent<ObjectCalculation>();
    cls->setConstructor<SegmentCalculation>();

}

AST_NAMESPACE_END
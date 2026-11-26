#include "VectorFixed.hpp"

#include "AstCore/ReflectAPI.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class VectorFixed::staticType;

Class* VectorFixed::StaticType()
{
    return &staticType;
}

Class* VectorFixed::getType() const
{
    return &staticType;
}

static bool VectorFixed_ClassInited = (VectorFixed::ClassInit(&VectorFixed::staticType), true);

void VectorFixed::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "VectorFixed"));
    cls->setDesc(u8R"(固定向量类 (Fixed in Axes))");
    cls->addToRegistry();
    cls->setParent<Vector>();
    cls->setConstructor<VectorFixed>();

}

AST_NAMESPACE_END
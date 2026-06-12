#include "EarthOrientation.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class EarthOrientation::staticType;

static bool EarthOrientation_ClassInited = (EarthOrientation::ClassInit(&EarthOrientation::staticType), true);

void EarthOrientation::ClassInit(Class* cls)
{

    cls->setName(NC_("Class", "EarthOrientation"));
    cls->setDesc(u8R"(地球旋转类)");
    cls->addToRegistry();
    cls->setParent<BodyOrientation>();
    cls->setConstructor<EarthOrientation>();

}

AST_NAMESPACE_END
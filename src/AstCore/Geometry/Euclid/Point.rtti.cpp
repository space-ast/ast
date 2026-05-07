#include "Point.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class Point::staticType;

static bool Point_ClassInited = (Point::ClassInit(&Point::staticType), true);

void Point::ClassInit(Class* cls)
{

    cls->setName("Point");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<Point>();

}

AST_NAMESPACE_END
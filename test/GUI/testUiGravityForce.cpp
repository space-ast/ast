#include "AstGlobal.h"
#include "AstCore/GravityForce.hpp"
#include "AstGUI/UiGravityForce.hpp"
#include <QApplication>

AST_USING_NAMESPACE

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 创建引力模型对象
    auto gravityForce = new GravityForce();
    
    // 创建界面
    UiGravityForce ui(gravityForce);
    ui.show();
    
    int result = app.exec();
    
    // 清理内存
    gravityForce->destruct();
    
    return result;
}
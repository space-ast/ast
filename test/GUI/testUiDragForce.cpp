#include "AstGlobal.h"
#include "AstCore/DragForce.hpp"
#include "AstGUI/UiDragForce.hpp"
#include <QApplication>

AST_USING_NAMESPACE

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 创建大气阻力模型对象
    auto dragForce = new DragForce();
    
    // 创建界面
    UiDragForce ui(dragForce);
    ui.show();
    
    int result = app.exec();
    
    // 清理内存
    dragForce->destruct();
    
    return result;
}
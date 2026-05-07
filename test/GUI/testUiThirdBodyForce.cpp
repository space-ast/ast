#include "AstGlobal.h"
#include "AstCore/ThirdBodyForce.hpp"
#include "AstGUI/UiThirdBodyForce.hpp"
#include <QApplication>

AST_USING_NAMESPACE

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 创建三体引力模型对象
    ThirdBodyForce* thirdBodyForce = new ThirdBodyForce();
    
    // 创建界面
    UiThirdBodyForce ui(thirdBodyForce);
    ui.show();
    
    int result = app.exec();
    
    // 清理内存
    thirdBodyForce->destruct();
    
    return result;
}
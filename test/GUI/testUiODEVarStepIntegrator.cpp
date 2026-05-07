#include "AstGlobal.h"
#include "AstGUI/UiODEVarStepIntegrator.hpp"
#include <QApplication>

AST_USING_NAMESPACE

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 创建界面
    UiODEVarStepIntegrator ui;
    ui.show();
    
    int result = app.exec();
    
    return result;
}
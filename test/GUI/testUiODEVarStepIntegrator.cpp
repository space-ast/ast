#ifdef AST_WITH_QT
#include "AstGlobal.h"
#include "ast/UiODEVarStepIntegrator.hpp"
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
#else
#include <stdio.h>
int main(){
    puts("没有启用Qt库");
    return 0;
}
#endif

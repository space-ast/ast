#include "AstGlobal.h"
#include "AstCore/HPOPForceModel.hpp"
#include "AstGUI/UiHPOPForceModel.hpp"
#include <QApplication>


AST_USING_NAMESPACE

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 创建HPOP力模型对象
    HPOPForceModel hpopForceModel;
    
    // 创建界面
    UiHPOPForceModel ui;
    ui.setObject(&hpopForceModel);
    ui.setHPOPForceModel(&hpopForceModel);
    ui.show();
    
    int result = app.exec();
    
    return result;
}
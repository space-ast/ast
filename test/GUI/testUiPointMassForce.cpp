#include "AstGlobal.h"
#include "AstCore/PointMassForce.hpp"
#include "AstGUI/UiPointMassForce.hpp"
#include <QApplication>

AST_USING_NAMESPACE

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 创建PointMassForce对象
    PointMassForce pointMass;
    
    // 创建UiPointMassForce界面
    UiPointMassForce ui(&pointMass);
    ui.setWindowTitle("点质量引力模型配置");
    ui.resize(600, 200);
    ui.show();
    
    return a.exec();
}
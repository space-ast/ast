#include "AstGlobal.h"
#include "AstCore/SolarRadiationPressure.hpp"
#include "AstGUI/UiSolarRadiationPressure.hpp"
#include <QApplication>

AST_USING_NAMESPACE

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 创建太阳光压模型对象
    auto solarRadiationPressure = new SolarRadiationPressure();
    
    // 创建界面
    UiSolarRadiationPressure ui(solarRadiationPressure);
    ui.show();
    
    int result = app.exec();
    
    // 清理内存
    solarRadiationPressure->destruct();
    
    return result;
}
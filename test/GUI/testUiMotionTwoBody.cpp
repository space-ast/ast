///
/// @file      testUiMotionTwoBody.cpp
/// @brief     测试二体运动模型UI组件
/// @author    axel
/// @date      2026-03-31
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "AstGlobal.h"
#include "ast/UiMotionTwoBody.hpp"
#include "ast/MotionTwoBody.hpp"
#include "ast/StateCartesian.hpp"
#include "ast/CelestialBody.hpp"
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>

AST_NAMESPACE_BEGIN

int testUiMotionTwoBody()
{
    aInitialize();
    int argc = 0;
    char** argv = nullptr;
    QApplication app(argc, argv);
    
    // 创建主窗口
    QMainWindow mainWindow;
    mainWindow.setWindowTitle("测试二体运动模型UI组件");
    mainWindow.resize(800, 600);
    
    // 创建中央部件
    QWidget* centralWidget = new QWidget(&mainWindow);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // 创建UiMotionTwoBody组件
    UiMotionTwoBody* uiMotionTwoBody = new UiMotionTwoBody(centralWidget);
    mainLayout->addWidget(uiMotionTwoBody);
    
    // 创建MotionTwoBody对象
    MotionTwoBody* motionTwoBody = MotionTwoBody::New();
    
    // 设置给UI组件
    uiMotionTwoBody->setMotionTwoBody(motionTwoBody);
    
    auto earth = aGetEarth();

    // 创建StateCartesian对象
    StateCartesian* stateCartesian = StateCartesian::New();
    
    // 设置初始状态
    stateCartesian->setX(6678137.0);
    stateCartesian->setY(0.0);
    stateCartesian->setZ(0.0);
    stateCartesian->setVx(0.0);
    stateCartesian->setVy(7905.36828);
    stateCartesian->setVz(0.0);
    stateCartesian->setStateEpoch(TimePoint::FromUTC(2026, 4, 3, 0, 1, 0));
    stateCartesian->setFrame(earth->makeFrameICRF());
    
    // 设置给MotionTwoBody
    motionTwoBody->setInitialState(stateCartesian);
    
    // 刷新UI
    uiMotionTwoBody->refreshUi();
    
    // 显示主窗口
    mainWindow.setCentralWidget(centralWidget);
    mainWindow.resize(1200, 600);
    mainWindow.show();
    
    // 运行应用
    return app.exec();
}

AST_NAMESPACE_END

int main()
{
    return ast::testUiMotionTwoBody();
}
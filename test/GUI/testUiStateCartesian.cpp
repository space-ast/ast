///
/// @file      testUiStateCartesian.cpp
/// @brief     测试 UiStateCartesian 类
/// @details   测试轨道状态编辑界面的功能
/// @author    axel
/// @date      2026-03-28
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

#include "ast/UiStateCartesian.hpp"
#include "ast/StateCartesian.hpp"
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>

AST_USING_NAMESPACE

int main(int argc, char* argv[])
{
    printf("testUiStateCartesian.cpp\n");
    QApplication app(argc, argv);
    
    QMainWindow window;
    QWidget* centralWidget = new QWidget(&window);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    
    // 创建 UiStateCartesian 组件
    UiStateCartesian* uiStateCartesian = new UiStateCartesian(centralWidget);
    layout->addWidget(uiStateCartesian);
    
    // 创建测试按钮
    QPushButton* testButton = new QPushButton("测试", centralWidget);
    layout->addWidget(testButton);
    
    // 创建 StateCartesian 对象
    StateCartesian* stateCartesian = StateCartesian::New();
    stateCartesian->setX(10000000);
    stateCartesian->setY(20000000);
    stateCartesian->setZ(30000000);
    stateCartesian->setVx(1000);
    stateCartesian->setVy(2000);
    stateCartesian->setVz(3000);
    
    // 绑定 StateCartesian 对象
    uiStateCartesian->setStateCartesian(stateCartesian);
    
    QObject::connect(uiStateCartesian, &UiStateCartesian::stateCartesianChanged, [](StateCartesian* newState) {
        if (newState)
        {
            qDebug() << "StateCartesian Changed:" << newState->x() << newState->y() << newState->z() << newState->vx() << newState->vy() << newState->vz();
        }
    });

    QObject::connect(testButton, &QPushButton::clicked, [uiStateCartesian, stateCartesian]() {
        // 测试 getStateCartesian 方法
        StateCartesian* updatedState = uiStateCartesian->getStateCartesian();
        if (updatedState)
        {
            qDebug() << "Updated StateCartesian:";
            qDebug() << "X:" << updatedState->x();
            qDebug() << "Y:" << updatedState->y();
            qDebug() << "Z:" << updatedState->z();
            qDebug() << "Vx:" << updatedState->vx();
            qDebug() << "Vy:" << updatedState->vy();
            qDebug() << "Vz:" << updatedState->vz();
        }
    });
    
    window.setCentralWidget(centralWidget);
    window.setWindowTitle("UiStateCartesian Test");
    window.resize(800, 400);
    window.show();
    
    return app.exec();
}
///
/// @file      testUiQuantity.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-27
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

#include "ast/UiQuantity.hpp"
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>

AST_USING_NAMESPACE

int main(int argc, char* argv[])
{
    printf("testUiQuantity.cpp\n");
    QApplication app(argc, argv);
    
    QMainWindow window;
    QWidget* centralWidget = new QWidget(&window);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    
    // 创建 UiQuantity 组件
    UiQuantity* uiQuantity = new UiQuantity(centralWidget);
    layout->addWidget(uiQuantity);
    
    // 创建测试按钮
    QPushButton* testButton = new QPushButton("测试", centralWidget);
    layout->addWidget(testButton);
    
    QObject::connect(testButton, &QPushButton::clicked, [uiQuantity]() {
        Quantity q = uiQuantity->getQuantity();
        qDebug() << "Current quantity:" << q.toString().c_str();
    });


    QObject::connect(uiQuantity, &UiQuantity::quantityChanged, [uiQuantity]() {
        qDebug() << "after quantityChanged:" << uiQuantity->getQuantity().toString().c_str();
    });
    
    // 设置默认值
    uiQuantity->setQuantity(Quantity(100, units::m));
    
    window.setCentralWidget(centralWidget);
    window.setWindowTitle("UiQuantity Test");
    window.resize(400, 200);
    window.show();
    
    return app.exec();
}
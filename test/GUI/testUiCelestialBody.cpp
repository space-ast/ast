///
/// @file      testUiCelestialBody.cpp
/// @brief     测试 UiCelestialBody 类
/// @author    Aist
/// @date      2026-04-10
/// @copyright 版权所有 (C) 2026-present, ast项目.
///

#include "ast/UiCelestialBody.hpp"
#include "ast/CelestialBody.hpp"
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>

AST_USING_NAMESPACE

int main(int argc, char* argv[])
{
    printf("testUiCelestialBody.cpp\n");
    QApplication app(argc, argv);
    
    QMainWindow window;
    QWidget* centralWidget = new QWidget(&window);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    
    UiCelestialBody* uiCelestialBody = new UiCelestialBody(centralWidget);
    layout->addWidget(uiCelestialBody);
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* testButton = new QPushButton("测试", centralWidget);
    QPushButton* refreshButton = new QPushButton("刷新", centralWidget);
    buttonLayout->addWidget(testButton);
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addStretch();
    layout->addLayout(buttonLayout);
    
    // 创建天体
    CelestialBody* body = new CelestialBody("Earth");
    body->setJplSpiceId(399);
    uiCelestialBody->setCelestialBody(body);
    
    QObject::connect(testButton, &QPushButton::clicked, [uiCelestialBody]() {
        if (auto b = uiCelestialBody->getCelestialBody())
        {
            qDebug() << "Name:" << QString::fromStdString(b->getName());
            qDebug() << "SPICE ID:" << b->getJplSpiceId();
            qDebug() << "GM:" << b->getGM();
            qDebug() << "Radius:" << b->getRadius();
        }
    });
    
    QObject::connect(refreshButton, &QPushButton::clicked, [uiCelestialBody]() {
        uiCelestialBody->refreshUi();
    });
    
    window.setCentralWidget(centralWidget);
    window.setWindowTitle("UiCelestialBody Test");
    window.resize(500, 600);
    window.show();
    
    return app.exec();
}

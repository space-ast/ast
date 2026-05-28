#include "ast/UiTimePoint.hpp"
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>

AST_USING_NAMESPACE

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    
    QMainWindow window;
    QWidget* centralWidget = new QWidget(&window);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    
    // 创建 UiTimePoint 控件
    UiTimePoint* uiTimePoint = new UiTimePoint(centralWidget);
    layout->addWidget(uiTimePoint);
    
    // 创建测试按钮
    QPushButton* testButton = new QPushButton("测试", centralWidget);
    layout->addWidget(testButton);
    
    // 连接测试按钮的点击事件
    QObject::connect(testButton, &QPushButton::clicked, [uiTimePoint]() {
        // 获取当前时间点并打印
        const TimePoint& timePoint = uiTimePoint->getTimePoint();
        qDebug() << "Current TimePoint:" << QString::fromStdString(timePoint.toString());
    });
    
    window.setCentralWidget(centralWidget);
    window.setWindowTitle("UiTimePoint Test");
    window.resize(400, 200);
    window.show();
    
    return app.exec();
}

///
/// @file      main.cpp
/// @brief     崩溃报告程序入口
/// @details   由崩溃处理器启动，显示用户友好的崩溃报告对话框。
/// @author    axel
/// @date      1.7.2026
/// @copyright 版权所有 (C) 2026-present, ast项目.

#include "CrashDialog.hpp"

#include <QApplication>
#include <QCommandLineParser>
#include <QFileInfo>

int main(int argc, char* argv[])
{
    // 高 DPI 适配（必须在 QApplication 创建之前设置）
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);
    app.setApplicationName("AppCrashReporter");

    // 解析命令行参数
    QCommandLineParser parser;
    parser.setApplicationDescription(QString::fromUtf8("崩溃报告程序"));
    parser.addPositionalArgument("log", QString::fromUtf8("崩溃日志文件路径"));
    parser.addOption(QCommandLineOption("app", QString::fromUtf8("主程序路径（用于重启）"), "path"));
    parser.addOption(QCommandLineOption("upload", QString::fromUtf8("反馈上传URL"), "url"));
    parser.process(app);

    QStringList posArgs = parser.positionalArguments();
    QString logPath = posArgs.isEmpty() ? QString() : posArgs.first();

    // 验证日志文件存在
    if (logPath.isEmpty() || !QFileInfo::exists(logPath))
    {
        // 没有日志文件 — 这不应该发生，但也显示一个简单提示
        QLabel* fallbackLabel = new QLabel(QString::fromUtf8(
            "<p style='font-size:14px;'><b>程序遇到了问题。</b></p>"
            "<p style='font-size:12px; color:#666;'>未找到崩溃日志文件。</p>"));
        fallbackLabel->setWindowTitle(QString::fromUtf8("程序遇到问题"));
        fallbackLabel->setAlignment(Qt::AlignCenter);
        fallbackLabel->setMinimumSize(300, 150);
        fallbackLabel->show();
        app.exec();
        return 0;
    }

    QString appPath = parser.value("app");
    QString uploadUrl = parser.value("upload");

    CrashDialog dialog(logPath, appPath, uploadUrl);
    dialog.show();

    return app.exec();
}

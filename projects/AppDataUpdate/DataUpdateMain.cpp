///
/// @file      DataUpdateMain.cpp
/// @brief     数据更新独立应用程序
/// @details   提供独立的"动态数据更新"对话框，不依赖主界面即可管理数据文件。
/// @author    axel
/// @date      2026-06-30
/// @copyright 版权所有 (C) 2026-present, ast项目.

#include "AstUiDataUpdate/UiDataUpdate.hpp"
#include "AstCore/RunTime.hpp"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char* argv[])
{
    AST_USING_NAMESPACE

    // 高 DPI 适配（必须在 QApplication 构造之前设置）
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);

    // 初始化 Ast 数据上下文（加载配置、设置数据目录等）
    errc_t err = aInitialize();
    if (err)
    {
        QMessageBox::warning(nullptr,
            QString::fromUtf8("初始化失败"),
            QString::fromUtf8("无法加载数据配置。\n请检查程序目录下的 ast_startup_file.txt。\n错误码：%1").arg(err));
        return 1;
    }

    // 打开数据更新对话框
    UiDataUpdate dlg;
    dlg.exec();

    return 0;
}

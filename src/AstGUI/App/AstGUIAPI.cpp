///
/// @file      GuiAPI.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-18
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
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

#include "AstGUI/AstGUIAPI.hpp"
#include "AstGUI/UiMainWindow.hpp"
#include "AstUtil/GUI.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstCore/RunTime.hpp"
#include <QApplication>
#include <QFontDatabase>
#include <QStyleFactory>
#include <QTranslator>
#include <QDir>
#include <QDebug>

AST_NAMESPACE_BEGIN

bool aInitAppAttributes()
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);  // 共享OpenGL上下文
    return true;
}

static bool s_initAppAttributes = aInitAppAttributes();

errc_t aGUIInit()
{
    errc_t rc = 0;
    if(!qApp)
    {
        int argc = 0;
        char *argv[] = { nullptr };
        rc = aQAppInit(argc, argv);
    }
    return rc;
}


errc_t aQAppInit(int argc, char *argv[])
{
    if (aCanDisplayGUI()) {
        QApplication* app = new QApplication(argc, argv);
        {
            // 加载自带的中文字体（桌面平台作为备选，WASM 平台必需）
            #ifdef A_WASM
            // wasm 不会存在data目录和exe目录分离的情况，所以直接使用相对路径
            QString fontPath = QStringLiteral("data/fonts/NotoSansSC-Regular.ttf");
            #else
            // 其他平台需要通过aDataDir获取data目录路径，避免其他软件调用ast库时的路径错误
            QString fontPath = QString::fromStdString(aDataDir()) + "/fonts/NotoSansSC-Regular.ttf";
            #endif
            int fontId = QFontDatabase::addApplicationFont(fontPath);
            if (fontId != -1) {
                QStringList families = QFontDatabase::applicationFontFamilies(fontId);
                if (!families.isEmpty()) {
                    QApplication::setFont(QFont(families.first()));
                }
            }
            else
            {
                qDebug() << "Failed to load font from path:" << fontPath;
            }
        }
        (void)app;
    }else{
        QCoreApplication* app = new QCoreApplication(argc, argv);
        (void)app;
    }
    // 加载翻译文件
    {
        auto translator = new QTranslator(qApp);
        QString qmPath = QCoreApplication::applicationDirPath() + "/Ast_zh.qm";
        if (!translator->load(qmPath)) {
            qmPath = QString::fromStdString(aDataDir()) + "/Ast_zh.qm";
            translator->load(qmPath);
        }
        qApp->installTranslator(translator);
    }
    return 0;
}

QMainWindow *aUiNewMainWindow()
{
    return new UiMainWindow();
}

AST_NAMESPACE_END


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
#include <QApplication>

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
    int argc = 0;
    char *argv[] = { nullptr };
    rc = aQAppInit(argc, argv);
    return rc;
}


errc_t aQAppInit(int argc, char *argv[])
{
    if (aCanDisplayGUI()) {
        QApplication* app = new QApplication(argc, argv);
        (void)app;
    }else{
        QCoreApplication* app = new QCoreApplication(argc, argv);
        (void)app;
    }
    return 0;
}

QMainWindow *aUiNewMainWindow()
{
    return new UiMainWindow();
}

AST_NAMESPACE_END


///
/// @file      GuiAPI.cpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
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

#include "AstGui/AstGuiAPI.hpp"
#include "AstGui/UiMainWindow.hpp"
#include "AstUtil/GUI.hpp"
#include <QApplication>

AST_NAMESPACE_BEGIN

err_t aGuiInit()
{
    err_t err = 0;


    return err;
}


err_t aQAppInit(int argc, char *argv[])
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


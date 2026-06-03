///
/// @file      AppMain.cpp
/// @brief     AppAnalyzer 应用程序入口
/// @details   ~
/// @author    axel
/// @date      2026-05-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
///
///    http://www.apache.org/licenses/LICENSE-2.0
///
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "AppMain.hpp"
#include "AstGUI/UiAnalyzerMainWindow.hpp"
#include "AstGUI/UiBasicAnalyzer.hpp"
#include "AstGUI/AstGUIAPI.hpp"
#include "AstAnalyzer/BasicAnalyzer.hpp"
#include "AstCore/MainSequence.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    AST_USING_NAMESPACE

    aQAppInit(argc, argv);

    // 初始化预定义的任务序列和分析器对象，并将它们关联起来。
    auto* basicAnalyzer = new BasicAnalyzer();
    auto* mainSequence = aNewObject<MainSequence>(basicAnalyzer);
    basicAnalyzer->setRelatedCommand(mainSequence);

    auto* mainWindow = new UiAnalyzerMainWindow();
    mainWindow->basicAnalyzerEditor()->setBasicAnalyzer(basicAnalyzer);
    mainWindow->setWindowTitle("AstAnalyzer");
    mainWindow->showMaximized();

    return qApp->exec();
}

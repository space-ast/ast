///
/// @file      MissionAnalysisMain.cpp
/// @brief     AppMissionAnalysis 应用入口
/// @details   航天任务分析主界面（AppMissionAnalysis）。
///            独立于 AppGUI / UiMainWindow，两者并存。
/// @author    axel
/// @date      2026-09-03
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

#include "AstGUI/UiMissionAnalysisWindow.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    AST_USING_NAMESPACE

    // 创建 Qt 应用程序实例
    QApplication app(argc, argv);

    // 创建航天任务分析主窗口
    UiMissionAnalysisWindow mainWindow;

    // 显示主窗口
    mainWindow.showMaximized();

    // 运行应用程序事件循环
    return app.exec();
}

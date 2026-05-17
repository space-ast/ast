///
/// @file      testMissionPanel.cpp
/// @brief     测试 UiMissionPanel 任务序列编辑器
/// @details   创建演示任务序列，在窗口中显示编辑面板
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

#include "AstGlobal.h"
#include "AstGUI/Mission/UiMissionPanel.hpp"
#include "AstCore/MissionModerator.hpp"
#include "AstCore/Sequence.hpp"
#include "AstCore/InitialState.hpp"
#include "AstCore/Propagate.hpp"
#include "AstCore/Maneuver.hpp"
#include "AstCore/TargeterSequence.hpp"
#include "AstCore/SpacecraftState.hpp"
#include "AstCore/StateCartesian.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include <QApplication>
#include <QMainWindow>
#include <QCommandLineParser>
#include <QMessageBox>

using namespace ast;

/// @brief 构建演示任务序列
/// Sequence
///   ├── InitialState
///   ├── Propagate (3600s)
///   ├── Maneuver
///   ├── Propagate (86400s)
///   └── TargeterSequence
static void buildDemoSequence(MissionModerator& moderator)
{
    auto& rootSeq = moderator.getSequence();
    rootSeq.setName("DemoMission");

    // InitialState
    {
        auto* init = aNewObject<InitialState>();
        init->setName("Initial State");

        auto* scState = SpacecraftState::NewDefault();
        auto* cartState = StateCartesian::New();
        scState->setOrbitState(cartState);
        // 从默认 CartState 设置初始轨道值
        init->setInputState(scState);
        init->setName("Initial State");

        std::vector<HMissionCommand> cmds;
        cmds.push_back(HMissionCommand(init));
        rootSeq.setCommands(std::move(cmds));
    }

    // Propagate 1
    {
        auto* prop1 = aNewObject<Propagate>();
        prop1->setName("Propagate 1h");
        prop1->setMaxPropTime(3600);

        auto cmds = rootSeq.getCommands();
        cmds.push_back(HMissionCommand(prop1));
        rootSeq.setCommands(std::move(cmds));
    }

    // Maneuver
    {
        auto* mnv = aNewObject<Maneuver>();
        mnv->setName("Maneuver");

        auto cmds = rootSeq.getCommands();
        cmds.push_back(HMissionCommand(mnv));
        rootSeq.setCommands(std::move(cmds));
    }

    // Propagate 2
    {
        auto* prop2 = aNewObject<Propagate>();
        prop2->setName("Propagate 1d");
        prop2->setMaxPropTime(86400);

        auto cmds = rootSeq.getCommands();
        cmds.push_back(HMissionCommand(prop2));
        rootSeq.setCommands(std::move(cmds));
    }

    // TargeterSequence
    {
        auto* tgt = aNewObject<TargeterSequence>();
        tgt->setName("Targeter");

        auto cmds = rootSeq.getCommands();
        cmds.push_back(HMissionCommand(tgt));
        rootSeq.setCommands(std::move(cmds));
    }

    rootSeq.setRepeatCount(1);
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("MissionPanel Test");
    app.setApplicationVersion("1.0");

    // 命令行解析
    QCommandLineParser parser;
    parser.setApplicationDescription("Test for UiMissionPanel — Astrogator-style editor");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "Optional .MCSSegment file to open");
    parser.process(app);

    // 构建 moderator 和演示序列
    MissionModerator moderator;
    buildDemoSequence(moderator);

    // 主窗口
    QMainWindow mainWindow;
    mainWindow.setWindowTitle("Mission Panel Test — Astrogator MCS Editor");
    mainWindow.resize(1000, 650);

    // 创建编辑面板
    auto* missionPanel = new UiMissionPanel(&mainWindow);
    missionPanel->setModerator(&moderator);

    mainWindow.setCentralWidget(missionPanel);

    // 若有命令行文件参数，尝试加载
    if (!parser.positionalArguments().isEmpty())
    {
        // 命令行文件加载：直接调用 aLoadMissionCommand
        QString filePath = parser.positionalArguments().first();
        // (UI 面板的 Open 按钮也可通过菜单使用)
    }

    mainWindow.show();

    return app.exec();
}

///
/// @file      PilotCommander.cpp
/// @brief     命令调度器实现
/// @author    axel
/// @date      2026-06-12
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

#include "PilotCommander.hpp"
#include "PilotAgent.hpp"
#include "PilotSession.hpp"
#include "Recorder/PilotRecorder.hpp"
#include "Player/PilotPlayer.hpp"
#include "AstUtil/IO.hpp"
#include <QApplication>
#include <QThread>
#include <QMetaObject>
#include <iostream>
#include <sstream>

AST_NAMESPACE_BEGIN

// ============================================================
//  helpText
// ============================================================

const char* PilotCommander::helpText()
{
    return u8"命令:\n"
           "  snap / snapshot  — 查看界面快照\n"
           "  rec start        — 开始录制\n"
           "  rec stop         — 停止录制并导出\n"
           "  rec export <p>   — 导出到文件\n"
           "  rec polish       — LLM润色并导出\n"
           "  replay <path>    — 回放脚本\n"
           "  <自然语言>       — LLM 操控界面\n"
           "  quit / exit      — 退出\n";
}

// ============================================================
//  构造 / 析构
// ============================================================

PilotCommander::PilotCommander(PilotSession* session, PilotRecorder* recorder,
                                 QObject* parent)
    : QObject(parent)
    , session_(session)
    , recorder_(recorder)
{
}

PilotCommander::~PilotCommander()
{
    stop();
}

void PilotCommander::setOutputCallback(std::function<void(const std::string&)> cb)
{
    outputCb_ = std::move(cb);
}

// ============================================================
//  executeSafe — 线程安全入口
// ============================================================

std::string PilotCommander::executeSafe(const std::string& line)
{
    // 判断当前是否已在主线程
    if (qApp && QThread::currentThread() == qApp->thread())
        return dispatch(line);

    // 从其他线程调用 → marshal 到主线程
    std::string result;
    QMetaObject::invokeMethod(qApp, [&]() {
        result = dispatch(line);
    }, Qt::BlockingQueuedConnection);
    return result;
}

void PilotCommander::executeAsync(const std::string& line)
{
    if (qApp && QThread::currentThread() == qApp->thread())
    {
        dispatch(line);
    }
    else
    {
        std::string cmd = line;
        QMetaObject::invokeMethod(qApp, [this, cmd]() {
            dispatch(cmd);
        }, Qt::QueuedConnection);
    }
}

// ============================================================
//  execute — 便捷入口
// ============================================================

std::string PilotCommander::execute(const std::string& line)
{
    return executeSafe(line);
}

// ============================================================
//  dispatch — 命令分发（必须在主线程调用）
// ============================================================

std::string PilotCommander::dispatch(const std::string& line)
{
    if (line.empty()) return "";

    // ---- quit / exit ----
    if (line == "quit" || line == "exit")
    {
        if (recorder_ && recorder_->isRecording())
            recorder_->stop();
        emit quitRequested();
        return "bye";
    }

    // ---- snap / snapshot ----
    if (line == "snap" || line == "snapshot")
    {
        auto* agent = PilotAgent::instance();
        if (!agent) return "Agent 未初始化";
        return agent->snapshot();
    }

    // ---- rec start ----
    if (line == "rec start")
    {
        if (!recorder_) return "录制器不可用";
        recorder_->start();
        return "[录制] 已开始";
    }

    // ---- rec stop ----
    if (line == "rec stop")
    {
        if (!recorder_) return "录制器不可用";
        recorder_->stop();
        std::ostringstream oss;
        oss << "[录制] 已停止，共 " << recorder_->stepCount() << " 步\n";
        std::string path = "record_output.json";
        recorder_->saveToFile(path);
        oss << "[录制] 已导出到 " << path;
        return oss.str();
    }

    // ---- rec export <path> ----
    if (line.rfind("rec export ", 0) == 0)
    {
        if (!recorder_) return "录制器不可用";
        std::string path = line.substr(11);
        // trim leading spaces
        while (!path.empty() && path.front() == ' ') path.erase(0, 1);
        if (path.empty()) return "用法: rec export <文件路径>";
        if (recorder_->saveToFile(path))
            return "[录制] 已导出到 " + path;
        else
            return "[录制] 导出失败: " + path;
    }

    // ---- rec polish ----
    if (line == "rec polish")
    {
        if (!recorder_) return "录制器不可用";
        if (recorder_->stepCount() == 0)
            return "[录制] 没有步骤可润色";

        std::string json = recorder_->polish(session_);
        recorder_->saveToFile("record_polished.json");
        return "[录制] 润色完成，已导出到 record_polished.json\n" + json;
    }

    // ---- replay <path> ----
    if (line.rfind("replay ", 0) == 0)
    {
        std::string path = line.substr(7);
        while (!path.empty() && path.front() == ' ') path.erase(0, 1);
        if (path.empty()) return "用法: replay <脚本文件路径>";

        auto* player = new PilotPlayer(session_);  // will be cleaned up after playback
        if (!player->loadScript(path))
        {
            delete player;
            return "[回放] 无法加载脚本: " + path;
        }

        std::ostringstream oss;
        oss << "[回放] 共 " << player->totalSteps() << " 个步骤\n";

        QObject::connect(player, &PilotPlayer::stepStarted,
            [](int idx, const std::string& desc) {
                ast_printf("\n[步骤 %d] %s\n", idx + 1, desc.c_str());
            });

        QObject::connect(player, &PilotPlayer::stepCompleted,
            [](int, const std::string&) {
                ast_printf("  ✓\n");
            });

        QObject::connect(player, &PilotPlayer::stepFailed,
            [](int, const std::string& err) {
                ast_printf("  ✗ %s\n", err.c_str());
            });

        QObject::connect(player, &PilotPlayer::playbackFinished,
            [player]() {
                ast_printf("[回放] 完成\n");
                player->deleteLater();
            });

        player->play();
        return oss.str();
    }

    // ---- help ----
    if (line == "help" || line == "?" || line == "h")
        return helpText();

    // ---- 自然语言指令 ----
    if (!session_)
        return "会话未初始化";

    session_->chat(line);
    return {};
}

// ============================================================
//  startStdinLoop / stop
// ============================================================

void PilotCommander::startStdinLoop()
{
    if (stdinThread_.joinable()) return;

    running_ = true;

    stdinThread_ = std::thread([this]() {
        // 打印 banner
        std::string banner = std::string(
            "\n========================================\n"
            "[AstUiPilot] Agent 已就绪\n"
            "  snap              — 查看界面快照\n"
            "  rec start         — 开始录制\n"
            "  rec stop          — 停止录制并导出\n"
            "  rec export <path> — 导出到指定文件\n"
            "  rec polish        — LLM润色并导出\n"
            "  replay <path>     — 回放脚本\n"
            "  <自然语言>         — LLM 操控界面\n"
            "  quit              — 退出\n"
            "========================================\n\n");

        if (outputCb_)
            outputCb_(banner);
        else
            std::cout << banner << std::flush;

        std::string line;
        while (running_ && std::getline(std::cin, line))
        {
            if (line.empty()) continue;
            if (!running_) break;

            std::string result = executeSafe(line);

            if (!result.empty())
            {
                if (outputCb_)
                    outputCb_(result + "\n");
                else
                    std::cout << result << std::endl;

                emit outputReady(QString::fromUtf8(result.c_str()));
            }

            // quit 命令触发退出
            if (line == "quit" || line == "exit")
                break;
        }
    });
}

void PilotCommander::stop()
{
    running_ = false;
    if (stdinThread_.joinable())
    {
        stdinThread_.join();
    }
}

AST_NAMESPACE_END

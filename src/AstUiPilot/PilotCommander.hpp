///
/// @file      PilotCommander.hpp
/// @brief     命令调度器 — 线程安全的指令分发
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

#pragma once

#include "AstGlobal.h"
#include <QObject>
#include <string>
#include <atomic>
#include <thread>
#include <functional>

AST_NAMESPACE_BEGIN

class PilotSession;
class PilotRecorder;

/*!
    @addtogroup UiPilot
    @{
*/

/// @brief 命令调度器
/// @details 提供线程安全的命令执行接口，供 stdin 线程和内嵌控制台共用。
///          - 从主线程调用 → 直接执行
///          - 从其他线程调用 → marshal 到主线程
class AST_UIPILOT_API PilotCommander : public QObject
{
    Q_OBJECT

public:
    /// @brief 构造函数
    /// @param session LLM 对话会话
    /// @param recorder 录制器（可为 nullptr）
    /// @param parent 父对象
    explicit PilotCommander(PilotSession* session, PilotRecorder* recorder,
                            QObject* parent = nullptr);

    /// @brief 析构函数
    ~PilotCommander() override;

    /// @brief 执行命令字符串（线程安全）
    /// @param line 命令文本
    /// @return 执行结果输出
    std::string execute(const std::string& line);

    /// @brief 异步执行命令（不等待结果，适合自然语言指令）
    void executeAsync(const std::string& line);

    /// @brief 启动 stdin 命令监听循环（后台线程）
    void startStdinLoop();

    /// @brief 停止 stdin 循环
    void stop();

    /// @brief 是否正在运行
    bool isRunning() const { return running_; }

    /// @brief 获取帮助文本
    static const char* helpText();

    /// @brief 设置输出回调（用于 stdin 模式下的即时输出）
    void setOutputCallback(std::function<void(const std::string&)> cb);

Q_SIGNALS:
    /// @brief 有输出内容就绪
    void outputReady(const QString& text);

    /// @brief 请求退出应用
    void quitRequested();

private:
    /// @brief 命令分发实现（必须在主线程调用）
    std::string dispatch(const std::string& line);

    /// @brief 线程安全执行：自动判断是否需要 marshal
    std::string executeSafe(const std::string& line);

    PilotSession*  session_;
    PilotRecorder* recorder_;
    std::atomic<bool> running_{true};
    std::thread     stdinThread_;
    std::function<void(const std::string&)> outputCb_;
};

/*! @} */

AST_NAMESPACE_END

///
/// @file      PilotPipeServer.hpp
/// @brief     Named Pipe 服务端 — 在目标进程内接收外部命令
/// @author    axel
/// @date      2026-06-13
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
#include <string>
#include <atomic>
#include <thread>

AST_NAMESPACE_BEGIN

class PilotCommander;

/*!
    @addtogroup UiPilot
    @{
*/

/// @brief Named Pipe 服务端
/// @details 在目标进程中创建 named pipe，等待外部客户端（Injector）连接并发送命令。
///          命令格式：一行文本 + \n，响应以 \0 结尾。
///          Pipe 名称：\\.\pipe\AstUiPilot_{PID}
class AST_UIPILOT_API PilotPipeServer
{
public:
    /// @brief 构造函数
    /// @param commander 命令调度器
    /// @param pid 目标进程 PID（用于构造唯一的 pipe 名称）
    explicit PilotPipeServer(PilotCommander* commander, unsigned long pid);

    /// @brief 析构函数，自动停止服务
    ~PilotPipeServer();

    /// @brief 启动 pipe 服务（后台线程）
    void start();

    /// @brief 停止 pipe 服务
    void stop();

    /// @brief 是否正在运行
    bool isRunning() const { return running_; }

    /// @brief 构造 pipe 名称
    static std::string pipeName(unsigned long pid);

private:
    /// @brief 服务线程主循环
    void serverLoop();

    PilotCommander*    commander_;
    unsigned long      pid_;
    std::atomic<bool>  running_{false};
    std::thread        serverThread_;
};

/*! @} */

AST_NAMESPACE_END

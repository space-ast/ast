///
/// @file      PilotSession.hpp
/// @brief     LLM对话管理，系统提示词构建，工具注册
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

AST_NAMESPACE_BEGIN

class ChatSession;
class ChatTools;
class PilotAgent;

/*!
    @addtogroup UiPilot
    @{
*/

/// @brief AstUiPilot 对话会话
/// @details 持有 ChatSession 和 PilotAgent，负责注册所有工具并管理 LLM 交互循环。
class AST_UIPILOT_API PilotSession : public QObject
{
    Q_OBJECT

public:
    /// @brief 构造函数
    /// @param agent 控件代理（必须已初始化）
    /// @param parent 父对象
    explicit PilotSession(PilotAgent* agent, QObject* parent = nullptr);

    /// @brief 析构函数
    ~PilotSession() override;
    PilotSession(const PilotSession&) = delete;
    PilotSession& operator=(const PilotSession&) = delete;

    /// @brief 执行自然语言指令
    /// @param command 用户指令
    /// @return LLM 的最终文本响应
    std::string execute(const std::string& command);

    /// @brief 发送消息
    /// @param message 消息内容
    void chat(const std::string& message, int maxIterForToolCalls=100);

    /// @brief 获取 ChatSession（用于高级配置）
    ChatSession& session();

    /// @brief 获取 PilotAgent
    PilotAgent* agent() const { return agent_; }

    /// @brief 获取系统提示词
    static const char* systemPrompt();

    /// @brief 构建完整的系统提示词（含应用信息）
    std::string buildSystemPrompt() const;

private:
    /// @brief 注册所有工具到 ChatSession
    void registerTools();

    PilotAgent* agent_;
    ChatSession*  chatSession_;
};

/*! @} */

AST_NAMESPACE_END

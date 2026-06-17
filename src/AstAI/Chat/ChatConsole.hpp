///
/// @file      ChatConsole.hpp
/// @brief     终端输出事件处理器
/// @details   ChatEventHandler 的终端输出实现，将流式事件实时打印到 stdout。
///            对应 Python AutoGen 的 Console 输出模式。
/// @author    axel
/// @date      2026-06-15
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
#include "AstAI/ChatEventHandler.hpp"
// #include "AstUtil/Markdown.hpp"
#include "AstUtil/MarkdownRenderer.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Chat
    @{
*/

/// @brief 终端输出事件处理器
/// @details 将流式事件格式化输出到 stdout，支持 Markdown 渲染
class AST_AI_API ChatConsole : public ChatEventHandler
{
public:
    using RendererType = MarkdownRenderer;
    //using RendererType = Markdown;


    ChatConsole() = default;
    ~ChatConsole() = default;

    void onTextChunk(const std::string& text) override;

    void onThought(const std::string& thought) override;

    void onToolCallRequest(const std::string& /*toolCallId*/,
                           const std::string& functionName,
                           const std::string& arguments) override;

    void onToolCallResult(const std::string& /*toolCallId*/,
                          const std::string& functionName,
                          const std::string& result) override;

    void onComplete() override;

    void onError(const std::string& error) override;

    /// @brief 设置是否以原始 Markdown 原文输出（不做 ANSI 渲染）
    void setRawOutput(bool raw) { rawOutput_ = raw; }

    /// @brief 查询当前是否为原始输出模式
    bool isRawOutput() const { return rawOutput_; }

private:
    void outputText(const std::string& text);

    RendererType renderer_;
    bool     rawOutput_ = false;
};

/*! @} */

AST_NAMESPACE_END

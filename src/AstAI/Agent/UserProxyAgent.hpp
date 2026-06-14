///
/// @file      UserProxyAgent.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-14
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
#include "ChatAgent.hpp"
#include "AstUtil/StringView.hpp"
#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/

/// @brief 用户代理智能体
/// @details 从终端读取用户输入，支持退出关键词检测。
///          当用户输入匹配退出关键词时，向消息历史中添加 "EXIT"，
///          配合 TextMentionTermination 可实现优雅退出。
class AST_AI_API UserProxyAgent : public ChatAgent
{
public:
    UserProxyAgent() = default;
    ~UserProxyAgent() = default;

    errc_t run(ChatMessages& messages) override;

    /// @brief 设置输入提示词
    void setPrompt(StringView prompt) { prompt_ = std::string(prompt); }

    /// @brief 获取输入提示词
    const std::string& prompt() const { return prompt_; }

    // —— 退出关键词 ——

    /// @brief 设置退出关键词列表（完全替换现有列表）
    /// @param keywords 退出关键词列表（如 {"quit", "exit", "q"}）
    void setExitKeywords(const std::vector<std::string>& keywords) { exitKeywords_ = keywords; }

    /// @brief 添加一个退出关键词
    /// @param keyword 退出关键词
    void addExitKeyword(StringView keyword) { exitKeywords_.emplace_back(keyword); }

    /// @brief 获取退出关键词列表
    const std::vector<std::string>& exitKeywords() const { return exitKeywords_; }

private:
    std::string prompt_;                        ///< 输入提示词
    std::vector<std::string> exitKeywords_;     ///< 退出关键词列表（大小写不敏感）
};

/*! @} */

AST_NAMESPACE_END

///
/// @file      OpenAI.hpp
/// @brief     OpenAI规范API调用接口
/// @author    axel
/// @date      2026-04-23
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
#include "AstAI/LLMClient.hpp"
#include "AstAI/ChatMessage.hpp"
#include "AstAI/ChatTool.hpp"
#include "AstUtil/StringView.hpp"
#include <string>
#include <vector>
#include <map>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Chat
    @{
*/

class JsonValue;

/// @brief OpenAI客户端
class AST_AI_API OpenAI: public LLMClient {
public:
    /// @brief 默认构造函数
    /// 从环境变量中获取API密钥和基础URL
    OpenAI();

    /// @brief 构造函数
    /// @param apiKey API密钥
    /// @param baseUrl API基础URL
    OpenAI(const std::string& apiKey, const std::string& baseUrl);

    #if 0
    /// @brief 发送聊天请求
    /// @param model 模型名称
    /// @param messages 消息列表
    /// @param tools 工具列表
    /// @param temperature 温度参数
    /// @return 响应内容
    std::string chat(
        const std::string& model, const std::vector<ChatMessage>& messages, 
        const std::vector<ChatTool>& tools = {}, float temperature = 0.7f
    );
    #endif
public: // 底层json接口

    /// @brief 聊天
    /// @param request 请求参数
    /// @return 响应参数
    JsonValue chat(const JsonValue& request);

    /// @brief 聊天
    /// @param request 请求参数
    /// @param response 响应参数
    /// @return 错误码
    errc_t chat(const JsonValue& request, JsonValue& response);

    /// @brief 设置API密钥
    /// @param apiKey API密钥
    void setApiKey(StringView apiKey){apiKey_ = std::string(apiKey);}

    /// @brief 设置API基础URL
    /// @param baseUrl API基础URL
    void setBaseUrl(StringView baseUrl){baseUrl_ = std::string(baseUrl);}
protected:
    const std::string& apiKey() const { return apiKey_; }
    const std::string& baseUrl() const { return baseUrl_; }
private:
    std::string apiKey_;          ///< API密钥
    std::string baseUrl_;         ///< API基础URL
};

/*! @} */

AST_NAMESPACE_END
///
/// @file      LLMClient.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-26
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
#include "AstUtil/JsonValue.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Chat
    @{
*/

class JsonValue;

/// @brief     大语言模型客户端接口
/// @details   子类只需实现一个纯虚方法即可完成对接
class AST_AI_API LLMClient
{
public:
    LLMClient() = default;
    virtual ~LLMClient() = default;

    /// @brief 发送聊天请求（纯虚接口，子类必须实现）
    /// @param request 请求参数（JSON格式，包含messages、model、temperature等字段）
    /// @param response 响应参数
    /// @return 错误码，0表示成功
    virtual errc_t chat(const JsonValue& request, JsonValue& response) = 0;

    /// @brief 发送聊天请求（便捷封装，内部调用上面的虚方法）
    /// @param request 请求参数
    /// @return 响应内容，失败时返回包含error字段的JsonValue
    JsonValue chat(const JsonValue& request);
};

/*! @} */

AST_NAMESPACE_END

///
/// @file      NetworkResponse.hpp
/// @brief     
/// @details   
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
#include <string>
#include <map>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// 网络响应
/// @details   网络响应，包含状态码、体和头。
class NetworkResponse
{
public:
    NetworkResponse() = default;
    ~NetworkResponse() = default;
    
    /// 获取网络响应状态码
    int statusCode() const { return statusCode_; }
    
    /// 设置网络响应状态码
    void setStatusCode(int statusCode) { statusCode_ = statusCode; }
    
    /// 获取网络响应体
    const std::string& body() const { return body_; }
    
    /// 设置网络响应体
    void setBody(const std::string& body) { body_ = body; }

    /// 设置网络响应体为 JSON 格式
    AST_UTIL_API
    void setJson(const JsonValue& json);

    AST_UTIL_API
    errc_t toJson(JsonValue& json) const;
    
    /// 获取网络响应体为 JSON 格式
    /// @warning 内部会进行一次 JSON 解析操作
    AST_UTIL_API
    JsonValue toJson() const;
    
    /// 获取网络响应头
    const std::map<std::string, std::string>& headers() const { return headers_; }
    
    /// 设置网络响应头
    void setHeaders(const std::map<std::string, std::string>& headers) { headers_ = headers; }
    
    /// 添加网络响应头
    void addHeader(const std::string& key, const std::string& value) { headers_[key] = value; }

private:
    int statusCode_ = 0;                         ///< 网络响应状态码码
    std::string body_;                           ///< 网络响应体
    std::map<std::string, std::string> headers_; ///< 网络响应头
};

/*! @} */

AST_NAMESPACE_END
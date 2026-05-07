///
/// @file      NetworkRequest.cpp
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

#include "NetworkRequest.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/StringUtil.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

void NetworkRequest::setMethod(StringView method)
{
    if(aEqualsIgnoreCase(method, "GET"))
        method_ = ENetworkRequestMethod::eGet;
    else if(aEqualsIgnoreCase(method, "POST"))
        method_ = ENetworkRequestMethod::ePost;
    else if(aEqualsIgnoreCase(method, "PUT"))
        method_ = ENetworkRequestMethod::ePut;
    else if(aEqualsIgnoreCase(method, "DELETE"))
        method_ = ENetworkRequestMethod::eDelete;
    else if(aEqualsIgnoreCase(method, "OPTIONS"))
        method_ = ENetworkRequestMethod::eOptions;
    else if(aEqualsIgnoreCase(method, "TRACE"))
        method_ = ENetworkRequestMethod::eTrace;
    else if(aEqualsIgnoreCase(method, "CONNECT"))
        method_ = ENetworkRequestMethod::eConnect;
    else if(aEqualsIgnoreCase(method, "PATCH"))
        method_ = ENetworkRequestMethod::ePatch;
    else
        method_ = ENetworkRequestMethod::eUnknown;
}


void NetworkRequest::setJson(const JsonValue& json)
{
    body_ = json.toJsonString();
}

errc_t NetworkRequest::toJson(JsonValue& json) const
{
    return json.parseFromString(body_);
}

JsonValue NetworkRequest::toJson() const
{
    JsonValue json;
    errc_t rc = toJson(json);
    if(rc != 0){
        aError("failed to parse request body to json");
    }
    return json;
}


AST_NAMESPACE_END

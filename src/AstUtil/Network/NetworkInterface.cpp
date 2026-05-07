///
/// @file      NetworkInterface.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-24
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

#include "NetworkInterface.hpp"

AST_NAMESPACE_BEGIN


bool aMethodAllowsBody(const std::string& method)
{
    return (method == "POST" || method == "PUT" || method == "PATCH");
}

int aParsePort(const std::string& portStr, int defaultPort)
{
    if (portStr.empty()) return defaultPort;
    for (char c : portStr) {
        if (!std::isdigit(static_cast<unsigned char>(c))) return defaultPort;
    }
    int port = std::atoi(portStr.c_str());
    if (port <= 0 || port > 65535) return defaultPort;
    return (port);
}


AST_NAMESPACE_END

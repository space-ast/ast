///
/// @file      plugin.cpp
/// @brief     cpp-httplib 网络插件：注册入口
/// @details   本动态库向主程序注册基于 cpp-httplib 的 NetworkInterface 实现。
/// @author    axel
/// @date      2026-11-04
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

#include "AstGlobal.h"
#include "AstUtil/PluginInterface.hpp"
#include "CppHttpNetworkInterface.hpp"

AST_USING_NAMESPACE

AST_PLUGIN_METADATA(NetworkInterface, "CppHTTP")    ///< 插件接口 ID 与注册名
AST_PLUGIN_INSTANCE(CppHttpNetworkInterface)        ///< 插件实例

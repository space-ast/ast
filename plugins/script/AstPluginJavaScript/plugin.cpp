///
/// @file      plugin.cpp
/// @brief     JavaScript 脚本执行器插件的注册入口
/// @details   本动态库向主程序注册可移植的 JavaScript 执行器（Duktape 后端）
/// @author    axel
/// @date      2026-10-27
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
#include "AstScript/ScriptExecutorProvider.hpp"
#include "JavaScriptExecutor.hpp"

AST_USING_NAMESPACE

namespace {

/// @brief JavaScript 执行器提供者
/// @details 同时继承 PluginInstance 与 IScriptExecutorProvider，主程序用 aPluginCast 取后者
class JavaScriptExecutorProvider : public PluginInstance, public IScriptExecutorProvider
{
public:
    ScriptExecutor* create() const override
    {
        return new JavaScriptExecutor();
    }
};

} // namespace


AST_PLUGIN_IID(IScriptExecutorProvider)          ///< 插件接口 ID
AST_PLUGIN_KEYS("JavaScript")                    ///< 插件注册名
AST_PLUGIN_INSTANCE(JavaScriptExecutorProvider)  ///< 插件实例

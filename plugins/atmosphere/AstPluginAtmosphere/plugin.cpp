///
/// @file      plugin.cpp
/// @brief     大气模型插件：注册入口
/// @details   本动态库向主程序注册插件新增的 "Exponential" 大气模型。
/// @author    axel
/// @date      2026-10-14
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
#include "AstCore/AtmosphereProvider.hpp"
#include "AstCore/DragForce.hpp"
#include "AstCore/RunTimeSolarSystem.hpp"
#include "AstCore/CelestialBody.hpp"
#include "ExponentialAtmosphere.hpp"

AST_USING_NAMESPACE

namespace {

/// @brief 指数大气模型提供者
/// @details 同时继承 PluginInstance 与 IAtmosphereProvider，主程序用 aPluginCast 取后者
class ExponentialProvider : public PluginInstance, public IAtmosphereProvider
{
public:
    Atmosphere* create(ScopedPtr<SpaceWeatherProvider>& spaceWeatherProvider) const override
    {
        A_UNUSED(spaceWeatherProvider);
        auto earth = aGetEarth();
        if (!earth)
            return nullptr;
        return new ExponentialAtmosphere(earth->getFrameFixed(), earth->getShape());
    }
};

} // namespace


AST_PLUGIN_IID(IAtmosphereProvider)             ///< 插件接口 ID
AST_PLUGIN_KEYS("Exponential")                  ///< 插件注册名
AST_PLUGIN_INSTANCE(ExponentialProvider)        ///< 插件实例

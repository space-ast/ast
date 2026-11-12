///
/// @file      VisBackend.hpp
/// @brief     可视化渲染后端插件接口与宿主 API
/// @details   定义渲染后端提供者接口，以及按插件键创建/选择渲染器的宿主接口
/// @author    axel
/// @date      2026-11-12
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
#include "AstVisualizationGlobal.h"
#include "AstUtil/PluginInterface.hpp"
#include "AstUtil/StringView.hpp"
#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

class VisRenderer;

/*!
    @addtogroup Visualization
    @{
*/

/// @brief 可视化渲染后端提供者接口
/// @details 由渲染后端插件（AstPluginVTK 等）实现，
///          主程序通过插件注册表按索引键查找并创建渲染器实例
class AST_VISUALIZATION_API IVisRendererProvider
{
public:
    virtual ~IVisRendererProvider() = default;

    /// @brief 创建渲染器实例
    /// @return 渲染器实例；所有权归调用方
    virtual VisRenderer* createRenderer() const = 0;
};

/// @brief 渲染后端提供者接口的唯一 ID（插件元数据里的 "iid" 必须是此值）
#define AST_IID_IVisRendererProvider "space.ast.AstVisualization.IVisRendererProvider"

AST_DECLARE_INTERFACE(IVisRendererProvider, AST_IID_IVisRendererProvider)

/// @brief 按插件键创建渲染器
/// @param name 后端索引键，如 "vtk"
/// @return 渲染器实例；未找到插件时返回 nullptr。所有权归调用方
AST_VISUALIZATION_API VisRenderer* aNewVisRenderer(StringView name);

/// @brief 按插件键选择默认渲染器
/// @param name 后端索引键，如 "vtk"
/// @return 错误码
/// @note 失败时默认渲染器保持不变；选中的渲染器由进程持有，调用方不得释放
AST_VISUALIZATION_API errc_t aUseVisRenderer(StringView name);

/// @brief 列出已发现插件提供的渲染后端索引键
AST_VISUALIZATION_API std::vector<std::string> aVisRendererNames();

/*! @} */

AST_NAMESPACE_END

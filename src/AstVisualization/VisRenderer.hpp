///
/// @file      VisRenderer.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-09-06
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
#include "AstUtil/ObjectNamed.hpp"
#include "AstCore/TimePoint.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class AST_VISUALIZATION_API VisRenderer
{
public:
    /// @brief 获取默认渲染器
    /// @details 未显式设置时，自动从已加载插件中选择（优先 "vtk"）
    /// @return 默认渲染器；没有任何可用后端时返回 nullptr
    static VisRenderer* Default();

    /// @brief 设置默认渲染器
    /// @param renderer 渲染器实例；接管所有权，调用方不得再释放。nullptr 被忽略
    /// @note 替换默认渲染器会释放旧实例，此前由 Default() 取得的指针随之失效
    static void SetDefault(VisRenderer* renderer);

    virtual ~VisRenderer() = default;

    /// @brief 渲染一个视图
    /// @param view 可视化视图
    /// @param epoch 渲染时刻（用于定位随时间变化的天体等场景元素）
    /// @return 错误码
    virtual errc_t render(const VisView& view, const TimePoint& epoch) = 0;
};

/*! @} */

AST_NAMESPACE_END

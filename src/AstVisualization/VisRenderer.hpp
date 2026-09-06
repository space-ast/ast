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
    static VisRenderer* Default();
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

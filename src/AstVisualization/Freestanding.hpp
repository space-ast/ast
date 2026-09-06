///
/// @file      FreeStanding.hpp
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
#include "AstCore/TimePoint.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


namespace visualization
{

/// @brief 获取当前的可视化视图，如果当前没有可视化视图，则会新建一个
/// @return 
AST_VISUALIZATION_API VisView* gcv();


AST_VISUALIZATION_API VisTrajectory* trajectory(Point* point=nullptr);


AST_VISUALIZATION_API VisGroundTrack* groundTrack(Point* point=nullptr);


AST_VISUALIZATION_API VisCelestialBody* planet(Body* body);


/// @brief 显示当前视图（在指定时刻渲染，用于定位随时间变化的天体等元素）
/// @param epoch 渲染时刻
AST_VISUALIZATION_API void show(const TimePoint& epoch);

/// @brief 显示当前视图（在当前时刻渲染）
AST_VISUALIZATION_API void show();

}

/*! @} */

AST_NAMESPACE_END

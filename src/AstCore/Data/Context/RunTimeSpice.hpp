///
/// @file      RunTimeSpice.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-22
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
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

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Data
    @{
*/



AST_CORE_CAPI
errc_t aSpiceGetPosICRF(
    const TimePoint& time,
    int target,
    int referenceBody,
    Vector3d& pos
);


AST_CORE_CAPI
errc_t aSpiceGetPosVelICRF(
    const TimePoint& time,
    int target,
    int referenceBody,
    Vector3d& pos,
    Vector3d& vel
);


/// @brief 获取目标体的时间间隔
/// @param filepath SPK文件路径
/// @param target 目标体ID
/// @param interval 时间间隔
/// @return errc_t 错误码
/// @note 该函数会解析SPK文件，因此在调用前请确保文件存在
AST_CORE_CAPI
errc_t aSpiceGetInterval(
    StringView filepath,
    int target,
    TimeInterval& interval
);


/// @brief 检查SPICE是否包含指定的天体的星历
/// @param bodyId 天体ID
/// @return bool 是否包含
AST_CORE_CAPI
bool aSpiceHasBodyEphemeris(int bodyId);



/*! @} */

AST_NAMESPACE_END

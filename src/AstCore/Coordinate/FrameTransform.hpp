///
/// @file      FrameTransform.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-05
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
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

class TimePoint;
class Transform;
class Rotation;

/// @brief     将ECI转换为ECF的旋转矩阵
/// @details   ~
/// @param     point  时间点
/// @param     rotation  旋转矩阵
/// @return    err_t  错误码
AST_CORE_CAPI err_t aECIToECFRotation(const TimePoint& point, Rotation& rotation);


/// @brief     将ECI转换为ECF的旋转矩阵
/// @details   ~
/// @param     point  时间点
/// @param     matrix  旋转矩阵
/// @return    err_t  错误码
AST_CORE_CAPI err_t aECIToECFMatrix(const TimePoint& point, Matrix3d& matrix);





AST_NAMESPACE_END

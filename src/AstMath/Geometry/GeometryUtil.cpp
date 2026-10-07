///
/// @file      GeometryUtil.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-10-03
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

#include "GeometryUtil.hpp"

AST_NAMESPACE_BEGIN

void aVectorCross(
    const Vector3d& vector1, const Vector3d& velocity1,
    const Vector3d& vector2, const Vector3d& velocity2,
    Vector3d& result, Vector3d& resultVelocity)
{
    // d/dt (v1 × v2) = (dv1/dt) × v2 + v1 × (dv2/dt)
    result = vector1.cross(vector2);
    resultVelocity = velocity1.cross(vector2) + vector1.cross(velocity2);
}

AST_NAMESPACE_END


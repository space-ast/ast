///
/// @file      LocalHorizonalFrame.cpp
/// @brief     局部水平坐标转换实现
/// @details   提供大地坐标到NED坐标、ENU坐标转换函数的实现
/// @author    axel
/// @date      2026-06-01
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

#include "LocalHorizonalFrame.hpp"
#include "AstMath/MathOperator.hpp"
#include "AstMath/Rotation.hpp"
#include "AstMath/AttitudeConvert.hpp"
#include "AstUtil/Math.hpp"
#include <cmath>


AST_NAMESPACE_BEGIN
using namespace math;

void aGeodeticToNEDTransform(const GeodeticPoint& origin, Rotation& rot)
{
    const array2d angle = { origin.longitude(), -kHalfPI - origin.latitude() };
	aEuler32ToMatrix(angle, rot.getMatrix());
}

void aGeodeticToENUTransform(const GeodeticPoint& origin, Rotation& rot)
{
    const array2d angle = { kHalfPI + origin.longitude(), kHalfPI - origin.latitude() };
	aEuler31ToMatrix(angle, rot.getMatrix());
}

AST_NAMESPACE_END

///
/// @file      AttitudeECFVelRadial.cpp
/// @brief     ECF 速度对齐 + 径向约束姿态
/// @details
/// @author    axel
/// @date      2026-09-28
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

#include "AttitudeECFVelRadial.hpp"
#include "AstCore/BuiltinFrame.hpp"

AST_NAMESPACE_BEGIN

AttitudeECFVelRadial::AttitudeECFVelRadial()
{
    setAlignVector(EAttitudeVector::eVelocity);
    setAlignAxis(EAttitudeAxis::eX);
    setConstraintVector(EAttitudeVector::eRadial);
    setConstraintAxis(EAttitudeAxis::eZ);
    setOffsetAxis(EAttitudeAxis::eX);
    setOffsetSense(EAttitudeOffsetSense::eRightHanded);
}

Frame* AttitudeECFVelRadial::defaultFrame() const
{
    return aFrameECF();
}

AST_NAMESPACE_END

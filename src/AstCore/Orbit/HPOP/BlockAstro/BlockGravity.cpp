///
/// @file      BlockGravity.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-19
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

#include "BlockGravity.hpp"
#include "AstCore/FrameTransform.hpp"
#include "AstMath/Vector.hpp"
#include "AstMath/Matrix.hpp"


AST_NAMESPACE_BEGIN

BlockGravity::BlockGravity()
    : BlockAstro{}
    , posCBI{&vectorBuffer}
    , accGravity{&vectorBuffer}
    , vectorBuffer{}
{
}

err_t BlockGravity::evaluate(const SimTime& simTime)
{
    // @fixme
    // 现在只支持地球的计算
    // 后续会考虑对其他星球的计算
    Vector3d posCBF;
    Vector3d accCBF;
    auto& tp = simTime.timePoint();
    Matrix3d matrix;
    aICRFToECFMatrix(tp, matrix);
    posCBF = matrix * (*posCBI);
    gravityCalculator.calcTotalAcceleration(posCBF, accCBF);
    *accGravity = accCBF * matrix;
    return eNoError;
}


AST_NAMESPACE_END


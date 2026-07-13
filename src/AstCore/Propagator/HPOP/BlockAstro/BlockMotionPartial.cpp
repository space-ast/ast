///
/// @file      BlockMotionPartial.cpp
/// @brief     运动学偏导数函数块实现
/// @details   继承 BlockMotion，重写 run() 以同时计算运动学关系和偏导数
/// @author    axel
/// @date      2026-07-11
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

#include "BlockMotionPartial.hpp"
#include "AstCore/BlockAstro.hpp"
#include "AstUtil/IdentifierAPI.hpp"

AST_NAMESPACE_BEGIN

BlockMotionPartial::BlockMotionPartial()
    : BlockMotion{}
{
    init();
}

void BlockMotionPartial::init()
{
    static auto identifierAMatrix = aIdentifier(kIdentifierAMatrix);
    outputPorts_.push_back({identifierAMatrix, (signal_t*)&aMatrixPtr_, 36, DataPort::eDouble, DataPort::eAccumulate});
}

errc_t BlockMotionPartial::run(const SimTime& simTime)
{
    // 运动学关系：dr/dt = v（与基类相同）
    *positionDerivative_ = *velocity_;

    // 写入位置运动学偏导数：∂(dr/dt)/∂v = I
    // A 矩阵 rows 0-2, cols 3-5 置为单位矩阵
    Matrix6d& A = *aMatrixPtr_;
    for (int i = 0; i < 3; ++i)
        A(i, 3 + i) += 1.0;

    return eNoError;
}

AST_NAMESPACE_END

///
/// @file      BlockMotion.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-20
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
#include "AstCore/BlockDerivative.hpp"
#include "AstMath/Vector.hpp"

AST_NAMESPACE_BEGIN

/// @brief     运动学块
/// @details   求解位置的导数
class BlockMotion : public BlockDerivative
{
public:
    BlockMotion();

    err_t evaluate(const SimTime& simTime) final;

protected:
    Vector3d* velocity_{&vectorBuffer};            ///< 速度
    Vector3d* positionDerivative_{&vectorBuffer};  ///< 位置导数
    Vector3d vectorBuffer{};                       ///< 向量缓冲区
};

AST_NAMESPACE_END

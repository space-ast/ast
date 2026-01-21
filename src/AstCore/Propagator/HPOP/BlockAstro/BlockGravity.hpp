///
/// @file      BlockGravity.hpp
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

#pragma once

#include "AstGlobal.h"
#include "BlockAstro.hpp"
#include "AstCore/GravityCalculator.hpp"
#include "AstCore/BlockDerivative.hpp"
#include "AstMath/Vector.hpp"
#include "AstUtil/StringView.hpp"

AST_NAMESPACE_BEGIN

/// @brief 重力场函数块
class AST_CORE_API BlockGravity: public BlockDerivative
{
public:
    BlockGravity();
    BlockGravity(StringView gravityModel, int degree, int order);
    err_t run(const SimTime& simTime) final;
private:
    void init();
protected:
    Vector3d*                posCBI{&vectorBuffer};                 ///< 位置
    Vector3d*                accGravity{&vectorBuffer};             ///< 重力加速度
    Vector3d*                velocityDerivative_{&vectorBuffer};    ///< 速度导数
    Vector3d                 vectorBuffer{};                        ///< 向量缓冲区
    GravityCalculatorDefault gravityCalculator;                     ///< 重力计算器
};



AST_NAMESPACE_END

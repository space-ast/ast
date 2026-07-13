///
/// @file      AstroBlockFactory.hpp
/// @brief     天体力学函数块工厂
/// @details   根据是否使用 STM 选择对应的 block 类型，封装 new 的类型决策
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

#pragma once

#include "AstGlobal.h"
#include "AstCore/BlockDerivative.hpp"
#include "AstCore/BlockMotion.hpp"
#include "AstCore/BlockMotionPartial.hpp"
#include "AstCore/BlockTwoBody.hpp"
#include "AstCore/BlockTwoBodyPartial.hpp"
#include "AstCore/BlockGravity.hpp"
#include "AstCore/BlockGravityPartial.hpp"
#include "AstCore/BlockMass.hpp"
#include "AstCore/BlockDrag.hpp"
#include "AstCore/BlockDragPartial.hpp"
#include "AstCore/BlockSRP.hpp"
#include "AstCore/BlockSRPPartial.hpp"
#include "AstCore/BlockThirdBodyGravity.hpp"
#include "AstCore/BlockThirdBodyGravityPartial.hpp"
#include "AstCore/BlockThirdBodyPointMass.hpp"
#include "AstCore/BlockThirdBodyPointMassPartial.hpp"
#include "AstCore/BlockStateTransitionMatrix.hpp"
#include "AstCore/Axes.hpp"
#include "AstCore/Atmosphere.hpp"
#include "AstCore/EclipseCalculator.hpp"
#include "AstMath/Matrix.hpp"
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup BlockAstro
    @{
*/

/// @brief 天体力学函数块工厂
/// @details
/// 封装 new 不同类型的决策逻辑。变化点仅在于是否启用 STM：
///   - 非 STM：使用普通块（BlockGravity, BlockDrag, ...)
///   - STM：使用偏导数块（BlockGravityPartial, ...）
///
/// 使用方式：
///   AstroBlockFactory factory(forceModel.useSTM());
///   addBlock(factory.createGravityBlock(...));
class AstroBlockFactory
{
public:
    explicit AstroBlockFactory(bool useSTM = false) : useSTM_(useSTM) {}

    // ── 运动学 ──

    BlockMotion* createMotionBlock() { 
        if (useSTM_)
            return new BlockMotionPartial();
        else
            return new BlockMotion(); 
    }

    // ── 中心天体 ──

    BlockTwoBody* createTwoBodyBlock(double GM) { 
        if (useSTM_)
            return new BlockTwoBodyPartial(GM);
        else
            return new BlockTwoBody(GM); 
    }

    BlockGravity* createGravityBlock(GravityField&& gf, int degree, int order,
                                         Axes* gravityAxes, Axes* propAxes)
    {
        if (useSTM_)
            return new BlockGravityPartial(std::move(gf), degree, order, gravityAxes, propAxes);
        else
            return new BlockGravity(std::move(gf), degree, order, gravityAxes, propAxes);
    }

    // ── 质量 ──

    BlockMass* createMassBlock(double mass) { return new BlockMass(mass); }

    // ── 阻力 / SRP ──

    BlockDrag* createDragBlock(Atmosphere* atm, double cd, double area, Frame* frame)
    {
        if (useSTM_)
            return new BlockDragPartial(atm, cd, area, frame);
        else
            return new BlockDrag(atm, cd, area, frame);
    }

    BlockSRP* createSRPBlock(EclipseCalculator* ec, double cr, double area, Frame* frame)
    {
        if (useSTM_)
            return new BlockSRPPartial(ec, cr, area, frame);
        else
            return new BlockSRP(ec, cr, area, frame);
    }

    // ── 三体 ──

    BlockThirdBodyGravity* createThirdBodyGravityBlock(Point* ephemeris, GravityField&& gf,
                                                   int degree, int order,
                                                   Axes* gravityAxes, Frame* propFrame)
    {
        if (useSTM_)
            return new BlockThirdBodyGravityPartial(ephemeris, std::move(gf), degree, order, gravityAxes, propFrame);
        else
            return new BlockThirdBodyGravity(ephemeris, std::move(gf), degree, order, gravityAxes, propFrame);
    }

    BlockThirdBodyPointMass* createThirdBodyPointMassBlock(Point* ephemeris, double GM, Frame* frame)
    {
        if (useSTM_)
            return new BlockThirdBodyPointMassPartial(ephemeris, GM, frame);
        else
            return new BlockThirdBodyPointMass(ephemeris, GM, frame);
    }
    BlockStateTransitionMatrix* createStateTransitionMatrixBlock() { return new BlockStateTransitionMatrix(); }
private:
    bool useSTM_;
};

/*! @} */

AST_NAMESPACE_END

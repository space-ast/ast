///
/// @file      ScStateCalcLongDriftRate.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-02
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
#include "AstCore/ScStateCalcBodyRelated.hpp"

AST_NAMESPACE_BEGIN

/// @brief 经度漂移率
/// @details 航天器相对中心天体固连系的经度变化率（正值向东），即考虑J2长期项后的平经度变化率与中心天体自转角速率之差。
///          中心天体由本计算量自身的body配置决定（未配置时默认为地球），与输入状态所在坐标系无关。
class AST_CORE_API ScStateCalcLongDriftRate : public ScStateCalcBodyRelated
{
public:
    AST_OBJECT(ScStateCalcLongDriftRate)

    ScStateCalcLongDriftRate() = default;
    ~ScStateCalcLongDriftRate() = default;
public:
    errc_t calculate(const SpacecraftState& scState, double& result) override;
    Dimension getDimension() const override {return EDimension::eAngularVelocity;}
};

AST_NAMESPACE_END

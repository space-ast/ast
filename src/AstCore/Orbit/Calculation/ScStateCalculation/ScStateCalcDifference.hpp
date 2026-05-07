///
/// @file      ScStateCalcDifference.hpp
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
#include "ScStateCalculation.hpp"
#include "AstCore/Segment.hpp"

AST_NAMESPACE_BEGIN

enum class EDifferenceOrderToUse
{
    eCurrentMinusInitial,
    eInitialMinusCurrent,
};

class AST_CORE_API ScStateCalcDifference : public ScStateCalculation
{
public:
    AST_OBJECT(ScStateCalcDifference)

    errc_t calculate(const SpacecraftState& scState, double& result) override;

    /// @brief 获取计算对象
    ScStateCalculation* calculation() const{return calculation_.get();}
    
    /// @brief 设置计算对象
    void setCalculation(ScStateCalculation* calculation);

    /// @brief 获取差值顺序
    EDifferenceOrderToUse differenceOrderToUse() const{return differenceOrderToUse_;}

    /// @brief 设置差值顺序
    void setDifferenceOrderToUse(EDifferenceOrderToUse order){differenceOrderToUse_ = order;}

    bool isAngle() const override;
protected:
    Segment* getSegment();
private:
    WeakPtr<Segment> segment_;
    WeakPtr<ScStateCalculation> calculation_;
    EDifferenceOrderToUse differenceOrderToUse_{EDifferenceOrderToUse::eCurrentMinusInitial};
};

AST_NAMESPACE_END
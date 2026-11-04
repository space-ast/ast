///
/// @file      ScStateCalcDifferenceOtherSegment.hpp
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
#include "AstCore/ScStateCalculation.hpp"
#include "AstCore/Segment.hpp"

AST_NAMESPACE_BEGIN

/// @brief 另一轨道段参与计算的状态
/// @details 指定取另一轨道段的初始状态还是最终状态
enum class ESegmentStateToUse
{
    eInitial,   ///< 初始状态（段的输入状态）
    eFinal,     ///< 最终状态（段的输出状态，默认）
};

/// @brief 相对另一轨道段的差值顺序
/// @details 指定参与作差的两个值谁减谁
enum class ESegmentDifferenceOrder
{
    eCurrentMinusSegment,   ///< 当前值减另一轨道段的值（默认）
    eSegmentMinusCurrent,   ///< 另一轨道段的值减当前值
};

/// @brief 相对另一轨道段的差值计算量
/// @details 对参考计算量在两个状态下的取值作差：
///          一个状态是当前待计算的状态，
///          另一个状态是另一轨道段的初始状态或最终状态（由 segmentStateToUse 决定）。
///          差值顺序由 differenceOrderToUse 决定，可以是“当前值减另一轨道段的值”，
///          也可以是“另一轨道段的值减当前值”。结果的量纲与参考计算量相同。
class AST_CORE_API ScStateCalcDifferenceOtherSegment : public ScStateCalculation
{
public:
    AST_OBJECT(ScStateCalcDifferenceOtherSegment)
    AST_PROPERT(calculation)
    AST_PROPERT(otherSegment)
    /// @brief 计算参考计算量在当前状态与另一轨道段选定状态下的差值
    /// @param scState 当前状态
    /// @param result 差值结果
    /// @return 错误码
    errc_t calculate(const SpacecraftState& scState, double& result) override;

    /// @brief 获取量纲
    /// @details 差值不改变量纲，直接沿用参考计算量的量纲；
    ///          未配置参考计算量时返回无量纲
    Dimension getDimension() const override;

    /// @brief 获取差值顺序
    ESegmentDifferenceOrder differenceOrderToUse() const{return differenceOrderToUse_;}

    /// @brief 设置差值顺序
    void setDifferenceOrderToUse(ESegmentDifferenceOrder order){differenceOrderToUse_ = order;}

    /// @brief 获取另一轨道段参与计算的状态
    ESegmentStateToUse segmentStateToUse() const{return segmentStateToUse_;}

    /// @brief 设置另一轨道段参与计算的状态
    void setSegmentStateToUse(ESegmentStateToUse state){segmentStateToUse_ = state;}
PROPERTIES:
    /// @brief 获取计算对象
    ScStateCalculation* calculation() const{return calculation_.get();}

    /// @brief 设置计算对象
    /// @details 计算对象没有父作用域时，会将其挂到本对象下
    void setCalculation(ScStateCalculation* calculation);

    /// @brief 获取参与比较的另一轨道段
    Segment* otherSegment() const{return otherSegment_.get();}

    /// @brief 设置参与比较的另一轨道段
    void setOtherSegment(Segment* segment){otherSegment_ = segment;}
private:
    WeakPtr<ScStateCalculation> calculation_{};     ///< 参与作差的参考计算量
    WeakPtr<Segment> otherSegment_{};               ///< 参与比较的另一轨道段
    ESegmentDifferenceOrder differenceOrderToUse_{ESegmentDifferenceOrder::eCurrentMinusSegment};  ///< 差值顺序
    ESegmentStateToUse segmentStateToUse_{ESegmentStateToUse::eFinal};                             ///< 另一轨道段参与计算的状态
};

AST_NAMESPACE_END

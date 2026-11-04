///
/// @file      ScStateCalcDeltaFromMaster.hpp
/// @brief     相对主航天器的差值计算量
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
#include "AstCore/Point.hpp"

AST_NAMESPACE_BEGIN

/// @brief 相对主航天器的差值计算量
/// @details 对参考计算量在两个状态下的取值作差：
///          一个状态是当前待计算的状态，另一个状态是主航天器（参考航天器）
///          在同一历元下的状态。主航天器以其位置速度（星历）的形式给出，
///          计算时按当前状态的历元取主航天器的位置速度。
///          结果为“当前值减主航天器值”，量纲与参考计算量相同。
/// @note 主航天器只提供轨道状态，其航天器参数（质量、面积等）不参与比较。
class AST_CORE_API ScStateCalcDeltaFromMaster : public ScStateCalculation
{
public:
    AST_OBJECT(ScStateCalcDeltaFromMaster)
    AST_PROPERT(calculation)
    AST_PROPERT(reference)
    ScStateCalcDeltaFromMaster() = default;
    ~ScStateCalcDeltaFromMaster() = default;

    /// @brief 计算参考计算量在当前状态与主航天器同一历元状态下的差值
    /// @param scState 当前状态
    /// @param result 差值结果
    /// @return 错误码
    errc_t calculate(const SpacecraftState& scState, double& result) override;

    /// @brief 获取量纲
    /// @details 差值不改变量纲，直接沿用参考计算量的量纲；
    ///          未配置参考计算量时返回无量纲
    Dimension getDimension() const override;
PROPERTIES:
    /// @brief 获取计算对象
    ScStateCalculation* calculation() const{return calculation_.get();}

    /// @brief 设置计算对象
    /// @details 计算对象没有父作用域时，会将其挂到本对象下
    void setCalculation(ScStateCalculation* calculation);

    /// @brief 获取主航天器
    /// @return 主航天器（位置速度提供者）
    Point* reference() const{return reference_.get();}

    /// @brief 设置主航天器
    /// @param reference 主航天器（位置速度提供者）
    void setReference(Point* reference){reference_ = reference;}
private:
    WeakPtr<ScStateCalculation> calculation_{};     ///< 参与作差的参考计算量
    WeakPtr<Point> reference_{};                    ///< 主航天器（参考航天器）
};

AST_NAMESPACE_END

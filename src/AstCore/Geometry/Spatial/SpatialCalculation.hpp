///
/// @file      SpatialCalculation.hpp
/// @brief     空间计算基类
/// @author    axel
/// @date      2026-07-29
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
#include "AstUtil/ObjectNamed.hpp"
#include "AstMath/Vector.hpp"
#include "SpatialCalculationVisitor.hpp"

#include "DifferentiationSettings.hpp"

AST_NAMESPACE_BEGIN

/// @brief 空间计算基类
/// @details 空间计算是一个依赖于时间和位置的标量函数
/// @ingroup Spatial
class AST_CORE_API SpatialCalculation : public ObjectNamed
{
public:
    SpatialCalculation();
    ~SpatialCalculation() override = default;

    /// @brief 接受访问者
    virtual void accept(SpatialCalculationVisitor& visitor) = 0;

    /// @brief 在指定时间和位置评估计算
    /// @param time 评估时间
    /// @param position 评估位置
    /// @param[out] result 计算结果值
    /// @return eNoError 成功，其他值表示错误
    virtual errc_t evaluate(const TimePoint& time, const Vector3d& position, double& result) const = 0;

    /// @name 导数设置
    /// @{
    void setDifferentiation(const DifferentiationSettings& ds) { diffSettings_ = ds; }
    const DifferentiationSettings& getDifferentiation() const { return diffSettings_; }
    /// @}

protected:
    DifferentiationSettings diffSettings_{};  ///< 导数/时间差分设置
};

AST_NAMESPACE_END

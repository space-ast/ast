///
/// @file      FOVConical.hpp
/// @brief     复杂圆锥视场类
/// @details   实现复杂圆锥视场的属性和方法
/// @author    axel
/// @date      2026-04-09
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

#include "FieldOfView.hpp"

AST_NAMESPACE_BEGIN

/// @brief 复杂圆锥视场类
class AST_CORE_API FOVConical: public FieldOfView
{
public:
    /// @brief 构造函数
    FOVConical() = default;
    
    /// @brief 析构函数
    ~FOVConical() override = default;
    
    /// @brief 获取视场类型
    /// @return 视场类型
    EFOVType getFOVType() const override { return EFOVType::eConical; }
    
    /// @brief 接受访问者
    /// @param visitor 访问者
    void accept(FieldOfViewVisitor& visitor) override { visitor.visit(*this); }

    /// @brief 计算方向向量到 FOV 边界的角度余量
    /// @param direction 传感器体坐标系中的单位方向向量
    /// @return min(outerCone - angle, angle - innerCone, clockMargin)
    double angularMargin(const Vector3d& direction) const override;
    
    /// @brief 设置内锥角
    /// @param angle 内锥角（弧度）
    void setInnerConeAngle(double angle) { innerConeAngle_ = angle; }
    
    /// @brief 获取内锥角
    /// @return 内锥角（弧度）
    double getInnerConeAngle() const { return innerConeAngle_; }
    
    /// @brief 设置外锥角
    /// @param angle 外锥角（弧度）
    void setOuterConeAngle(double angle) { outerConeAngle_ = angle; }
    
    /// @brief 获取外锥角
    /// @return 外锥角（弧度）
    double getOuterConeAngle() const { return outerConeAngle_; }
    
    /// @brief 设置最小时钟角
    /// @param angle 最小时钟角（弧度）
    void setMinClockAngle(double angle) { minClockAngle_ = angle; }
    
    /// @brief 获取最小时钟角
    /// @return 最小时钟角（弧度）
    double getMinClockAngle() const { return minClockAngle_; }
    
    /// @brief 设置最大时钟角
    /// @param angle 最大时钟角（弧度）
    void setMaxClockAngle(double angle) { maxClockAngle_ = angle; }
    
    /// @brief 获取最大时钟角
    /// @return 最大时钟角（弧度）
    double getMaxClockAngle() const { return maxClockAngle_; }
    
private:
    double innerConeAngle_{0.0};  ///< 内锥角（弧度）
    double outerConeAngle_{0.0};  ///< 外锥角（弧度）
    double minClockAngle_{0.0};   ///< 最小时钟角（弧度）
    double maxClockAngle_{0.0};   ///< 最大时钟角（弧度）
};

AST_NAMESPACE_END
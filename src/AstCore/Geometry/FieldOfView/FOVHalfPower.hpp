///
/// @file      FOVHalfPower.hpp
/// @brief     半功率视场类
/// @details   实现半功率视场的属性和方法
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

/// @brief 半功率视场类
class AST_CORE_API FOVHalfPower: public FieldOfView
{
public:
    /// @brief 构造函数
    FOVHalfPower() = default;
    
    /// @brief 析构函数
    ~FOVHalfPower() override = default;
    
    /// @brief 获取视场类型
    /// @return 视场类型
    EFOVType getFOVType() const override { return EFOVType::eHalfPower; }
    
    /// @brief 接受访问者
    /// @param visitor 访问者
    void accept(FieldOfViewVisitor& visitor) override { visitor.visit(*this); }

    /// @brief 计算方向向量到 FOV 边界的角度余量
    /// @param direction 传感器体坐标系中的单位方向向量
    /// @return halfAngle_ - angle(direction, 视轴)
    double angularMargin(const Vector3d& direction) const override;
    
    /// @brief 设置半角
    /// @param angle 半角（弧度）
    void setHalfAngle(double angle) { halfAngle_ = angle; }
    
    /// @brief 获取半角
    /// @return 半角（弧度）
    double getHalfAngle() const { return halfAngle_; }
    
    /// @brief 设置频率
    /// @param frequency 频率（GHz）
    void setFrequency(double frequency) { frequency_ = frequency; }
    
    /// @brief 获取频率
    /// @return 频率（GHz）
    double getFrequency() const { return frequency_; }
    
    /// @brief 设置天线直径
    /// @param diameter 天线直径（米）
    void setAntennaDiameter(double diameter) { antennaDiameter_ = diameter; }
    
    /// @brief 获取天线直径
    /// @return 天线直径（米）
    double getAntennaDiameter() const { return antennaDiameter_; }
    
private:
    double halfAngle_{0.0};          ///< 半角（弧度）
    double frequency_{0.0};           ///< 频率（GHz）
    double antennaDiameter_{0.0};     ///< 天线直径（米）
};

AST_NAMESPACE_END
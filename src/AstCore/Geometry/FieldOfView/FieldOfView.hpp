///
/// @file      FieldOfView.hpp
/// @brief     视场基类
/// @details   定义视场的基本接口和属性
/// @author    axel
/// @date      2025-12-12
/// @copyright 版权所有 (C) 2025-present, SpaceAST项目.
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
#include "AstUtil/Object.hpp"
#include "AstUtil/ObjectNamed.hpp"
#include "FieldOfViewVisitor.hpp"

AST_NAMESPACE_BEGIN

/// @brief 视场类型枚举
enum class EFOVType
{
    eSimpleCone,   ///< 简单圆锥视场
    eConical,      ///< 复杂圆锥视场
    eHalfPower,    ///< 半功率视场
    eRectangular,  ///< 矩形视场
    eSAR,          ///< 合成孔径雷达视场
    eCustom        ///< 自定义视场
};

/// @brief 视场基类
class AST_CORE_API FieldOfView: public ObjectNamed
{
public:
    /// @brief 构造函数
    FieldOfView() = default;

    /// @brief 析构函数
    virtual ~FieldOfView() = default;

    /// @brief 获取视场类型
    /// @return 视场类型
    virtual EFOVType getFOVType() const = 0;

    /// @brief 接受访问者
    /// @param visitor 访问者
    virtual void accept(FieldOfViewVisitor& visitor) = 0;

    /// @brief 计算方向向量到 FOV 边界的角度余量（弧度）
    /// @param direction 传感器体坐标系中的方向向量（视轴 = +Z）
    /// @return 角度余量(rad)：>0 在视场内，<0 在视场外，==0 恰在边界
    virtual double angularMargin(const Vector3d& direction) const = 0;

    /// @brief 便捷方法：方向是否在视场内
    /// @param direction 传感器体坐标系中的方向向量
    /// @return 是否在视场内
    bool containsDirection(const Vector3d& direction) const {
        return angularMargin(direction) > 0.0;
    }

    /// @brief 便捷方法：目标点是否在视场内
    /// @param targetPosInSensorFrame 目标在传感器体坐标系中的位置
    /// @return 是否在视场内
    bool containsPoint(const Vector3d& targetPosInSensorFrame) const {
        return angularMargin(targetPosInSensorFrame) > 0.0;
    }

protected:
};

AST_NAMESPACE_END
///
/// @file      VectorCross.hpp
/// @brief     叉乘向量类，用于计算两个向量的叉积 (对应STK的 Cross Product)
/// @details   该类表示两个输入向量的叉积向量。
///            叉积结果的方向垂直于两个输入向量所在的平面，
///            遵循右手定则确定正方向。
///            对应STK (Systems Tool Kit) 中的 "Cross Product" 向量类型。
/// @author    YᴗYang
/// @date      2026-07-16
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
#include "AstCore/Vector.hpp"

AST_NAMESPACE_BEGIN

/// @brief 叉乘向量类 (Cross Product)
/// @details 计算两个输入向量的叉积 C = A × B。
///          叉积结果的方向遵循右手定则，大小等于
///          |A| * |B| * sin(θ)，其中 θ 为两向量之间的夹角。
///          当需要速度时，使用乘积法则计算：
///          dC/dt = (dA/dt × B) + (A × dB/dt)
class AST_CORE_API VectorCross : public Vector
{
public:
    AST_OBJECT(VectorCross)
    AST_PROPERT(Vector1)
    AST_PROPERT(Vector2)

    VectorCross() = default;
    ~VectorCross() override = default;

    /// @brief 获取叉积向量的参考坐标系（使用向量1的坐标系）
    /// @details 叉积运算要求两个输入向量在同一坐标系中。
    ///          返回向量1所在的参考轴系，计算时会通过 getVectorIn()
    ///          自动将向量2转换到向量1的坐标系下。
    ///          若向量1为空指针则返回 nullptr。
    /// @return 参考轴系指针
    Axes* getAxes() const override;

    /// @brief 获取叉积向量在指定时间点的值 C = A × B
    /// @param tp 时间点
    /// @param vec 输出参数，叉积向量的值
    /// @return 错误码
    errc_t getVector(const TimePoint& tp, Vector3d& vec) const override;

    /// @brief 获取叉积向量在指定时间点的值和速度
    /// @param tp 时间点
    /// @param vec 输出参数，叉积向量的值 C = A × B
    /// @param vel 输出参数，叉积向量的速度 dC/dt = (dA/dt × B) + (A × dB/dt)
    /// @return 错误码
    errc_t getVector(const TimePoint& tp, Vector3d& vec, Vector3d& vel) const override;

PROPERTIES:
    /// @brief 设置第一个输入向量 A
    /// @param vec 输入向量指针
    void setVector1(Vector* vec);

    /// @brief 获取第一个输入向量 A
    /// @return 输入向量指针
    Vector* getVector1() const;

    /// @brief 设置第二个输入向量 B
    /// @param vec 输入向量指针
    void setVector2(Vector* vec);

    /// @brief 获取第二个输入向量 B
    /// @return 输入向量指针
    Vector* getVector2() const;

private:
    WeakPtr<Vector> vector1_{ nullptr }; ///< 第一个输入向量 A
    WeakPtr<Vector> vector2_{ nullptr }; ///< 第二个输入向量 B
};

AST_NAMESPACE_END

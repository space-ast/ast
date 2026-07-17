///
/// @file      VectorFixedAtEpoch.hpp
/// @brief     历元固定向量类，用于表示在指定历元时刻从源向量捕获并固定于参考轴系中的向量 (对应STK的Fixed at Epoch)
/// @details   该类在指定的历元时刻从源向量捕获其值，转换到参考轴系后保持恒定。
///            源向量可以是时变的（如天体指向、轨道位置等），捕获后分量在参考轴系中不再变化。
///            对应STK (Systems Tool Kit) 中的 "Fixed at Epoch" 向量类型。
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
#include "AstCore/Axes.hpp"
#include "AstCore/TimePoint.hpp"

AST_NAMESPACE_BEGIN

/// @brief 历元固定向量类 (Fixed at Epoch)
/// @details 在指定历元时刻从源向量捕获其值，转换到参考轴系后保持恒定。
///          捕获后向量在参考轴系中指向保持不变，分量恒定，速度恒为零。
///          典型用途：在某个时刻捕获时变向量（如卫星-太阳方向）的快照，
///          使该方向在指定参考坐标系中固定不变。
///          @note 与VectorFixed的区别：本类的向量值来自源向量在历元时刻的快照；
///          VectorFixed则是直接指定常量分量，不需要源向量和历元。
class AST_CORE_API VectorFixedAtEpoch : public Vector
{
public:
    AST_OBJECT(VectorFixedAtEpoch)
    AST_PROPERT(SourceVector)
    AST_PROPERT(Axes)
    AST_PROPERT(Epoch)

    VectorFixedAtEpoch() = default;
    ~VectorFixedAtEpoch() override = default;

    /// @brief 获取向量所在的参考坐标系
    /// @return 参考轴系指针
    Axes* getAxes() const override;

    /// @brief 获取向量在指定时间点的值（始终返回历元时刻从源向量捕获的值）
    /// @param tp 时间点（本类不使用，值在捕获后恒定）
    /// @param vec 输出参数，捕获的向量值（已转换到参考轴系）
    /// @return 错误码
    errc_t getVector(const TimePoint& tp, Vector3d& vec) const override;

    /// @brief 获取向量在指定时间点的值和速度（速度始终为零）
    /// @param tp 时间点（本类不使用）
    /// @param vec 输出参数，捕获的向量值（已转换到参考轴系）
    /// @param vel 输出参数，速度向量（始终为零）
    /// @return 错误码
    errc_t getVector(const TimePoint& tp, Vector3d& vec, Vector3d& vel) const override;

PROPERTIES:
    /// @brief 设置源向量（在历元时刻从此向量捕获值）
    /// @param vec 源向量指针
    void setSourceVector(Vector* vec);

    /// @brief 获取源向量
    /// @return 源向量指针
    Vector* getSourceVector() const;

    /// @brief 设置参考轴系（捕获的向量值将转换到此坐标系中表达）
    /// @param axes 参考轴系指针
    void setAxes(Axes* axes);

    /// @brief 设置历元时刻（捕获源向量值的时刻）
    /// @param epoch 历元时刻
    void setEpoch(const TimePoint& epoch);

    /// @brief 获取历元时刻
    /// @return 历元时刻
    const TimePoint& getEpoch() const;

private:
    HVector       sourceVector_{  nullptr }; ///< 源向量（历元时刻从此捕获值）
    WeakPtr<Axes> referenceAxes_{ nullptr }; ///< 参考轴系（捕获后向量在此坐标系中保持恒定）
    TimePoint     epoch_{};                  ///< 历元时刻（捕获源向量的时刻）
};
AST_NAMESPACE_END

///
/// @file      VectorDisplacement.hpp
/// @brief     位移向量类，用于表示从原点到目标点的向量
/// @details   该类表示两个点之间的位移向量，通过原点(Origin)和目标点(Destination)定义。
///            位移向量的参考坐标系使用原点的坐标系。
/// @author    axel
/// @date      2026-05-14
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
#include "AstCore/Point.hpp"

AST_NAMESPACE_BEGIN

class AST_CORE_API VectorDisplacement : public Vector
{
public:
    AST_OBJECT(VectorDisplacement)
    AST_PROPERT(Origin)
    AST_PROPERT(Destination)
    VectorDisplacement() = default;
    ~VectorDisplacement() override = default;

    

    /// @brief 获取向量所在的参考坐标系（使用原点的坐标系）
    /// @return 向量所在的参考坐标系指针
    Axes* getAxes() const override;

    /// @brief 获取位移向量在指定时间点的值
    /// @param tp 时间点
    /// @param vec 输出参数，位移向量（Destination - Origin）
    /// @return 错误码
    errc_t getVector(const TimePoint& tp, Vector3d& vec) const override;

    /// @brief 获取位移向量在指定时间点的值和速度
    /// @param tp 时间点
    /// @param vec 输出参数，位移向量（Destination - Origin）
    /// @param vel 输出参数，位移速度向量
    /// @return 错误码
    errc_t getVector(const TimePoint& tp, Vector3d& vec, Vector3d& vel) const override;
PROPERTIES:
    /// @brief 设置原点
    /// @param origin 原点指针
    void setOrigin(Point* origin);

    /// @brief 获取原点
    /// @return 原点指针
    Point* getOrigin() const;

    /// @brief 设置目标点
    /// @param destination 目标点指针
    void setDestination(Point* destination);

    /// @brief 获取目标点
    /// @return 目标点指针
    Point* getDestination() const;
private:
    /// @brief 获取参考坐标系（返回原点的坐标系）
    /// @return 参考坐标系指针
    Frame* getReferenceFrame() const;

private:
    WeakPtr<Point> origin_{ nullptr };      ///< 原点
    WeakPtr<Point> destination_{ nullptr }; ///< 目标点
};

AST_NAMESPACE_END
///
/// @file      DataGroupEuler.hpp
/// @brief     欧拉角姿态数据组
/// @details   对应 Euler 服务，输出欧拉角 (A, B, C) 及角速率 (A dot, B dot, C dot)，转序 121
/// @author    axel
/// @date      2026-07-08
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
#include "AstReport/DataGroupTimeVar.hpp"
#include "AstUtil/Span.hpp"
#include "AstCore/Axes.hpp"
#include "AstMath/Euler.hpp"
#include "AstMath/Quaternion.hpp"
#include "AstMath/Vector.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/


void aQuatAngvelToEuler(const Quaternion& quat, const Vector3d& angvel, int seq, Euler& angles, Euler& rates);


/// @brief 欧拉角姿态数据组 — 欧拉角 A, B, C 及角速率 (转序 121)
class DataGroupEuler : public DataGroupTimeVar
{
public:
    struct Data
    {
        TimePoint time_;
        Euler     angles_;   ///< 欧拉角 A, B, C
        Euler     rates_;    ///< 欧拉角速率 A dot, B dot, C dot

        const TimePoint& getTime() const { return time_; }

        double getA() const { return angles_.angle1(); }
        double getB() const { return angles_.angle2(); }
        double getC() const { return angles_.angle3(); }

        double getADot() const { return rates_.angle1(); }
        double getBDot() const { return rates_.angle2(); }
        double getCDot() const { return rates_.angle3(); }
    };
    static DataElements Elements();

    DataGroupEuler() = default;
    ~DataGroupEuler() = default;
public:
    errc_t calculate(const TimeList& timeList, VariantVector& result) const override;
    const DataElements& getElements() const override;
public:
    errc_t calculate(const TimeList& timeList, std::vector<Data>& result) const;
    errc_t calculate(const TimeList& timeList, Span<Data> result) const;
public:
    Axes* getAxes() const { return axes_.get(); }
    Axes* getReferenceAxes() const { return referenceAxes_.get(); }
    void setAxes(Axes* a)          { axes_ = a; }
    void setReferenceAxes(Axes* a) { referenceAxes_ = a; }

    int  getRotationOrder() const { return rotationOrder_; }
    void setRotationOrder(int order) { rotationOrder_ = order; }
private:
    WeakPtr<Axes> axes_;
    WeakPtr<Axes> referenceAxes_;
    int rotationOrder_{Euler::eXYX};
};


/*! @} */

AST_NAMESPACE_END

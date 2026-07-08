///
/// @file      DataGroupSpherical.hpp
/// @brief     球坐标轨道根数数据组
/// @details   赤经、赤纬、地心距、水平航迹角、惯性航迹方位角、惯性速度
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
#include "AstCore/Point.hpp"
#include "AstCore/Frame.hpp"
#include "AstMath/Vector.hpp"
#include "AstUtil/Span.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/




/// @brief 球坐标轨道根数数据组 — 赤经、赤纬、地心距、水平航迹角、惯性航迹方位角、惯性速度
class DataGroupSpherical : public DataGroupTimeVar
{
public:
    struct Data
    {
        TimePoint time_;
        double    rightAscen_;
        double    declination_;
        double    radius_;
        double    horizFltPathAng_;
        double    inertialFltPathAzi_;
        double    inertialVel_;

        const TimePoint& getTime()                const { return time_; }
        double getRightAscen()                    const { return rightAscen_; }
        double getDeclination()                   const { return declination_; }
        double getRadius()                        const { return radius_; }
        double getHorizFltPathAng()               const { return horizFltPathAng_; }
        double getInertialFltPathAzi()            const { return inertialFltPathAzi_; }
        double getInertialVel()                   const { return inertialVel_; }
    };
    static DataElements Elements();

    DataGroupSpherical() = default;
    ~DataGroupSpherical() = default;
public:
    errc_t calculate(const TimeList& timeList, VariantVector& result) const override;
    const DataElements& getElements() const override;
public:
    errc_t calculate(const TimeList& timeList, std::vector<Data>& result) const;
    errc_t calculate(const TimeList& timeList, Span<Data> result) const;
public:
    Point* getScPoint() const { return scPoint_.get(); }
    Frame* getFrame()   const { return frame_.get(); }
private:
    WeakPtr<Point> scPoint_;
    WeakPtr<Frame> frame_;
};


/*! @} */

AST_NAMESPACE_END

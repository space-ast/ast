///
/// @file      DataGroupLLRState.hpp
/// @brief     LLR 球坐标状态数据组
/// @details   惯性系下的球坐标(纬度、经度、地心距)及变化率
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




/// @brief LLR 球坐标状态数据组 — 纬度、经度、地心距及变化率
class DataGroupLLRState : public DataGroupTimeVar
{
public:
    struct Data
    {
        TimePoint time_;
        double    lat_;
        double    lon_;
        double    rad_;
        double    latRate_;
        double    lonRate_;
        double    radRate_;

        const TimePoint& getTime()    const { return time_; }
        double           getLat()     const { return lat_; }
        double           getLon()     const { return lon_; }
        double           getRad()     const { return rad_; }
        double           getLatRate() const { return latRate_; }
        double           getLonRate() const { return lonRate_; }
        double           getRadRate() const { return radRate_; }
    };
    static DataElements Elements();

    DataGroupLLRState() = default;
    ~DataGroupLLRState() = default;
public:
    errc_t calculate(const TimeList& timeList, VariantVector& result) const override;
    const DataElements& getElements() const override;
public:
    errc_t calculate(const TimeList& timeList, std::vector<Data>& result) const;
    errc_t calculate(const TimeList& timeList, Span<Data> result) const;
public:
    Point* getPoint() const { return scPoint_.get(); }
    Frame* getFrame()   const { return frame_.get(); }
    void setPoint(Point* p) { scPoint_ = p; }
    void setFrame(Frame* f)   { frame_ = f; }
private:
    WeakPtr<Point> scPoint_;
    WeakPtr<Frame> frame_;
};


/*! @} */

AST_NAMESPACE_END

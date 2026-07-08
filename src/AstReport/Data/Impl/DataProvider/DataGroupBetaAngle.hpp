///
/// @file      DataGroupBetaAngle.hpp
/// @brief     贝塔角数据组
/// @details   太阳贝塔角及月球贝塔角 — 天体方向矢量与轨道面的夹角
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




/// @brief 贝塔角数据组 — 太阳贝塔角及月球贝塔角
/// @details beta = arcsin(r_third · (r × v)/|r × v|)
class DataGroupBetaAngle : public DataGroupTimeVar
{
public:
    struct Data
    {
        TimePoint time_;
        double    betaAngle_;
        double    lunarBetaAngle_;

        const TimePoint& getTime()           const { return time_; }
        double           getBetaAngle()      const { return betaAngle_; }
        double           getLunarBetaAngle() const { return lunarBetaAngle_; }
    };
    static DataElements Elements();

    DataGroupBetaAngle() = default;
    ~DataGroupBetaAngle() = default;
public:
    errc_t calculate(const TimeList& timeList, VariantVector& result) const override;
    const DataElements& getElements() const override;
public:
    errc_t calculate(const TimeList& timeList, std::vector<Data>& result) const;
    errc_t calculate(const TimeList& timeList, Span<Data> result) const;
public:
    Point* getPoint()   const { return scPoint_.get(); }
    Frame* getFrame()     const { return frame_.get(); }
    Point* getSunPoint()  const { return sunPoint_.get(); }
    Point* getMoonPoint() const { return moonPoint_.get(); }
    void setPoint(Point* p)   { scPoint_ = p; }
    void setFrame(Frame* f)     { frame_ = f; }
    void setSunPoint(Point* p)  { sunPoint_ = p; }
    void setMoonPoint(Point* p) { moonPoint_ = p; }
private:
    WeakPtr<Point> scPoint_;
    WeakPtr<Frame> frame_;
    WeakPtr<Point> sunPoint_;
    WeakPtr<Point> moonPoint_;
};


/*! @} */

AST_NAMESPACE_END

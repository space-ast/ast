///
/// @file      DataGroupModOrbElem.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-07-07
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
#include "AstCore/Frame.hpp"
#include "AstCore/Point.hpp"
#include "AstCore/OrbitElement.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class DataGroupModOrbElem : public DataGroupTimeVar
{
public:
    struct Data
    {
        ModOrbElem modOrbElem_;
        double bodyRadius_{};
        double gm_{};
        TimePoint time_{};

        const TimePoint& getTime() const { return time_; }
        double getSMA() const { return modOrbElem_.getSMA(); }
        double getEcc() const { return modOrbElem_.getEcc(); }
        double getInc() const { return modOrbElem_.getInc(); }
        double getRAAN() const { return modOrbElem_.getRAAN(); }
        double getArgPeri() const { return modOrbElem_.getArgPeri(); }
        double getTrueAnomaly() const { return modOrbElem_.getTrueAnomaly(); }
        double getMeanAnomaly() const { return modOrbElem_.getMeanAnomaly(); }
        double getEccAnomaly() const { return modOrbElem_.getEccAnomaly(); }
        double getArgLat() const { return modOrbElem_.getArgLat(); }
        double getApoAlt() const { return modOrbElem_.getApoAlt(bodyRadius_); }
        double getApoRad() const { return modOrbElem_.getApoRad(); }
        double getPeriAlt() const { return modOrbElem_.getPeriAlt(bodyRadius_); }
        double getPeriRad() const { return modOrbElem_.getPeriRad(); }
        double getMeanMotion() const { return modOrbElem_.getMeanMotion(gm_); }
        // double getLAN() const { return modOrbElem_.getLAN(); }
        double getTimePastAscNode() const { return modOrbElem_.getTimePastAscNode(gm_); }
        double getTimePastPeri() const { return modOrbElem_.getTimePastPeri(gm_); }
        double getPeriod() const { return modOrbElem_.getPeriod(gm_); }
        double getLongitudeOfPeri() const { return modOrbElem_.getLongitudeOfPeri(); }
        double getMeanLongitude() const { return modOrbElem_.getMeanLongitude(); }
    };
    static DataElements Elements();
    DataGroupModOrbElem() = default;
    ~DataGroupModOrbElem() = default;
public:
    const DataElements &getElements() const override;
    errc_t calculate(const TimeList& timeList, VariantVector& result) const override;
public:
    errc_t calculate(const TimeList& timeList, std::vector<Data>& result) const;
    errc_t calculate(const TimeList& timeList, Span<Data> result) const;
public:
    Point* getPoint() const { return point_.get(); }
    Frame* getFrame() const { return frame_.get(); }
    void setPoint(Point* p) { point_ = p; }
    void setFrame(Frame* f) { frame_ = f; }
private:
    WeakPtr<Point> point_;
    WeakPtr<Frame> frame_;
};


/*! @} */

AST_NAMESPACE_END

///
/// @file      DataGroupEquinElem.hpp
/// @brief     春分点根数数据组
/// @details   非奇异轨道根数：半长轴、h、k、p、q、平经度及平均角速度
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
#include "AstCore/Frame.hpp"
#include "AstCore/Point.hpp"
#include "AstCore/OrbitElement.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/


/// @brief 春分点根数数据组 — 半长轴、h、k、p、q、平经度及平均角速度
class DataGroupEquinElem : public DataGroupTimeVar
{
public:
    struct Data
    {
        EquinElem equinElem_{};
        double    gm_{};
        TimePoint time_;

        const TimePoint& getTime()           const { return time_; }
        double getSemiMajorAxis()            const { return equinElem_.a(); }
        double getH()                        const { return equinElem_.h(); }
        double getK()                        const { return equinElem_.k(); }
        double getP()                        const { return equinElem_.p(); }
        double getQ()                        const { return equinElem_.q(); }
        double getMeanLon()                  const { return equinElem_.lambda(); }
        double getMeanMotion()               const { return equinElem_.a() > 0.0 ? aSMAToMeanMotion(equinElem_.a(), gm_) : 0.0; }

        // 以下元素需要额外基础设施支持，暂未实现：
        // const char* getDirection() const;  // DataType 2 (string), "Prograde"/"Retrograde"
        // double getSMARate()          const; // DataType 3, 需要传播器状态
        // double getHRate()            const; // DataType 3
        // double getKRate()            const; // DataType 3
        // double getPRate()            const; // DataType 3
        // double getQRate()            const; // DataType 3
        // double getMeanLonRate()      const; // DataType 3
        // double getMeanLonPerturbRate() const; // DataType 3
    };
    static DataElements Elements();

    DataGroupEquinElem() = default;
    ~DataGroupEquinElem() = default;
public:
    errc_t calculate(const TimeList& timeList, VariantVector& result) const override;
    const DataElements& getElements() const override;
public:
    errc_t calculate(const TimeList& timeList, std::vector<Data>& result) const;
    errc_t calculate(const TimeList& timeList, Span<Data> result) const;
public:
    Point* getPoint() const { return point_.get(); }
    Frame* getFrame() const { return frame_.get(); }
private:
    WeakPtr<Point> point_;
    WeakPtr<Frame> frame_;
};


/*! @} */

AST_NAMESPACE_END

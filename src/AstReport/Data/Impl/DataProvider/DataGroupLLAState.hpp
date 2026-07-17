///
/// @file      DataGroupLLAState.hpp
/// @brief     LLA 状态数据组
/// @details   天体固连系下的大地坐标(经纬高)、地心坐标及变化率
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
#include "AstCore/CelestialBody.hpp"
#include "AstCore/GeodeticPoint.hpp"
#include "AstMath/Vector.hpp"
#include "AstUtil/Span.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/




/// @brief LLA 状态数据组 — 大地坐标(经纬高)、地心经纬度及变化率
class DataGroupLLAState : public DataGroupTimeVar
{
public:
    struct Data
    {
        TimePoint     time_{};
        GeodeticPoint detic_{};
        double        centricLat_{};
        double        centricLon_{};
        double        latRate_{};
        double        lonRate_{};
        double        altRate_{};

        const TimePoint& getTime()       const { return time_; }
        double           getLat()        const { return detic_.latitude(); }
        double           getLon()        const { return detic_.longitude(); }
        double           getAlt()        const { return detic_.altitude(); }
        double           getLatRate()    const { return latRate_; }
        double           getLonRate()    const { return lonRate_; }
        double           getAltRate()    const { return altRate_; }
        double           getCentricLat() const { return centricLat_; }
        double           getCentricLon() const { return centricLon_; }

        // 以下元素需要额外基础设施支持，暂未实现：
        // double getAltAGL()      const; // 需要地形高程数据
        // double getTerrainAlt()  const; // 需要地形高程数据
        // double getAltMSL()      const; // 需要大地水准面模型
        // int    getUTMZone()     const; // 需要 UTM 投影转换
        // double getUTMEasting()  const; // 需要 UTM 投影转换
        // double getUTMNorthing() const; // 需要 UTM 投影转换
        // const std::string& getMGRSCell() const; // 需要 MGRS 转换
    };
    static DataElements Elements();

    DataGroupLLAState() = default;
    ~DataGroupLLAState() = default;
public:
    errc_t calculate(const TimeList& timeList, VariantVector& result) const override;
    const DataElements& getElements() const override;
public:
    errc_t calculate(const TimeList& timeList, std::vector<Data>& result) const;
    errc_t calculate(const TimeList& timeList, Span<Data> result) const;
public:
    Point* getPoint() const { return scPoint_.get(); }
    Body*  getBody()    const { return body_.get(); }
    void setPoint(Point* p) { scPoint_ = p; }
    void setBody(Body* b)     { body_ = b; }
private:
    WeakPtr<Point> scPoint_{};
    WeakPtr<Body>  body_{};
};


/*! @} */

AST_NAMESPACE_END

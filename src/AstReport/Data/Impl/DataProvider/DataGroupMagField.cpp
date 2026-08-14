///
/// @file      DataGroupMagField.cpp
/// @brief     地磁场数据组实现
/// @details   IGRF 地磁场 19 个元素的批量计算
/// @author    axel
/// @date      2026-08-11
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

#include "DataGroupMagField.hpp"
#include "AstUtil/VariantVector.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Dimension.hpp"
#include "AstUtil/Constants.h"
#include "AstCore/BodyShape.hpp"
#include "AstCore/GeoCoordinate.hpp"
#include "AstCore/SpheroidShape.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/DateTime.hpp"
#include "AstCore/EarthFrame.hpp"
#include "AstWeather/igrf.hpp"

AST_NAMESPACE_BEGIN

namespace
{

/// @brief 将时间点转换为小数年份（含闰年处理）
void aCalcDecimalYear(const TimePoint& tp, double& year)
{
    DateTime dttm;
    aTimePointToUTC(tp, dttm);
    double doy = static_cast<double>(dttm.dayOfYear())
               + dttm.hour() / 24.0
               + dttm.minute() / 1440.0
               + dttm.second() / 86400.0;
    double daysInYear = dttm.date().isLeapYear() ? 366.0 : 365.0;
    year = static_cast<double>(dttm.year()) + (doy - 1.0) / daysInYear;
}

/// @brief 偶极子 L 壳参数
/// @details 利用磁倾角反推磁纬：tan(λm) = 0.5·tan(I)，再求 L = (r/Re) / cos²λm
/// @param rMeters  地心距 [m]
/// @param incRad   磁倾角 I [rad]（向下为正）
/// @param reMeters 地球参考半径 [m]（IGRF ERA = 6371.2 km）
/// @param ldip     偶极子 L [无量纲]
void aCalcDipoleL(double rMeters, double incRad, double reMeters, double& ldip)
{
    double tanLam = 0.5 * std::tan(incRad);
    double lamM   = std::atan(tanLam);
    double cosLam = std::cos(lamM);
    double cos2   = cosLam * cosLam;
    if (cos2 > 1e-15)
        ldip = (rMeters / reMeters) / cos2;
    else
        ldip = 0.0;
}

/// @brief IGRF 地球参考半径（与 shellig.c initize_ 中的 ERA 一致）
constexpr double kIGRF_ERA_M = 6371.2e3;

} // namespace

DataElements DataGroupMagField::Elements()
{
    DataElements elements;

    // -- 自变量 --
    elements.addElement<Data, const TimePoint&, &Data::getTime>
        ("Time", Dimension::DateTime());

    // -- NEZ 当地分量 --
    elements.addElement<Data, double, &Data::getTotalIntensityF>
        ("Total intensity F", Dimension::Unit());
    elements.addElement<Data, double, &Data::getNorthIntensityX>
        ("North intensity X", Dimension::Unit());
    elements.addElement<Data, double, &Data::getEastIntensityY>
        ("East intensity Y", Dimension::Unit());
    elements.addElement<Data, double, &Data::getVerticalIntensityZ>
        ("Vertical intensity Z", Dimension::Unit());

    // -- 推导标量 --
    elements.addElement<Data, double, &Data::getHorizontalIntensityH>
        ("Horizontal intensity H", Dimension::Unit());
    elements.addElement<Data, double, &Data::getDeclinationD>
        ("Magnetic declination D", Dimension::Angle());
    elements.addElement<Data, double, &Data::getInclinationI>
        ("Magnetic inclination I", Dimension::Angle());

    // -- L 壳参数 --
    elements.addElement<Data, double, &Data::getDipoleL>
        ("Dipole L-shell parameter", Dimension::Unit());
    elements.addElement<Data, double, &Data::getMcIlwainL>
        ("McIlwain L-shell parameter", Dimension::Unit());
    elements.addElement<Data, double, &Data::getAbsMcIlwainL>
        ("McIlwain Abs(L-shell) parameter", Dimension::Unit());

    // -- 磁镜比 --
    elements.addElement<Data, double, &Data::getBOverBeq>
        ("B/Beq", Dimension::Unit());
    elements.addElement<Data, double, &Data::getBeq>
        ("Beq", Dimension::Unit());

    // -- ECF 矢量分量 --
    elements.addElement<Data, double, &Data::getEcfX>
        ("B Field - ECF x", Dimension::Unit());
    elements.addElement<Data, double, &Data::getEcfY>
        ("B Field - ECF y", Dimension::Unit());
    elements.addElement<Data, double, &Data::getEcfZ>
        ("B Field - ECF z", Dimension::Unit());

    // -- ECI 矢量分量 --
    elements.addElement<Data, double, &Data::getEciX>
        ("B Field - ECI x", Dimension::Unit());
    elements.addElement<Data, double, &Data::getEciY>
        ("B Field - ECI y", Dimension::Unit());
    elements.addElement<Data, double, &Data::getEciZ>
        ("B Field - ECI z", Dimension::Unit());

    return elements;
}

DataGroupMagField::DataGroupMagField()
    : point_{}
    , body_{aGetEarth()}
{
}

errc_t DataGroupMagField::calculate(const TimeList& timeList, VariantVector& result) const
{
    size_t size = timeList.size();
    result.reset<Data>(size);
    return calculate(timeList, result.asSpan<Data>());
}

const DataElements& DataGroupMagField::getElements() const
{
    static DataElements elements = Elements();
    return elements;
}

errc_t DataGroupMagField::calculate(const TimeList& timeList, std::vector<Data>& result) const
{
    size_t size = timeList.size();
    result.resize(size);
    return calculate(timeList, Span<Data>(result));
}

errc_t DataGroupMagField::calculate(const TimeList& timeList, Span<Data> result) const
{
    Point* scPoint = this->getPoint();
    Body*  body    = this->getBody();
    if (!scPoint || !body)
    {
        aError("DataGroupMagField: scPoint or body is null");
        return eErrorNullPtr;
    }

    Frame* bodyFixed = body->getFrameFixed();
    BodyShape* shape = body->getShape();
    if (!bodyFixed || !shape)
    {
        aError("DataGroupMagField: bodyFixed or shape is null");
        return eErrorNullPtr;
    }

    size_t size = result.size();
    if (size != timeList.size())
    {
        aError("DataGroupMagField: result size must equal timeList size");
        return eErrorInvalidParam;
    }


    errc_t rc = eNoError;
    for (size_t i = 0; i < size; i++)
    {
        Data& data = result[i];
        data.time_ = timeList[i];

        // ---- 1. 获取 ECF 位置 → 大地坐标 ----
        Vector3d pos, vel;
        errc_t err = scPoint->getPosVelIn(bodyFixed, data.time_, pos, vel);
        if (err != eNoError) { 
            rc = err; 
            continue; 
        }

        GeodeticPoint detic;
        shape->transform(pos, detic);

        // ---- 2. 小数年份 ----
        double year;
        aCalcDecimalYear(data.time_, year);

        // ---- 3. 调用 IGRF ----
        double bnorth, beast, bdown, babs;
        aIGRFField(
            detic.longitude(), detic.latitude(), detic.altitude(), 
            year, bnorth, beast, bdown, babs
        );

        // ---- 4. IGRF 入参转换 (rad→deg, m→km) ----
        double latDeg = detic.latitude()  * kRadToDeg;
        double lonDeg = detic.longitude() * kRadToDeg;
        double altKm  = detic.altitude()  * 1e-3;

        double xl, bbx;
        igrf(lonDeg, latDeg, altKm, year, xl, bbx);

        double X = bnorth;  // North  [T]
        double Y = beast ;  // East   [T]
        double Z = bdown ;  // Down   [T]
        double F = babs  ;  // Total  [T]

        double H = std::sqrt(X * X + Y * Y);  // Horizontal intensity  [T]
        double D = std::atan2(Y, X);           // Declination          [rad]
        double I = std::atan2(Z, H);           // Inclination          [rad]

        data.totalIntensityF_     = F;
        data.northIntensityX_     = X;
        data.eastIntensityY_      = Y;
        data.verticalIntensityZ_  = Z;
        data.horizontalIntensityH_ = H;
        data.declinationD_        = D;
        data.inclinationI_        = I;

        data.mcIlwainL_    = xl;
        data.absMcIlwainL_ = std::fabs(xl);
        data.bOverBeq_     = bbx;
        data.beq_          = (bbx > 0.0) ? (F / bbx) : 0.0;   // Beq = F / (B/Beq)  [T]

        double rMeters = pos.norm();
        aCalcDipoleL(rMeters, I, kIGRF_ERA_M, data.dipoleL_);

        // ---- 6. NEZ → ECF ----
        GeoCoordinate geo;
        geo.setLatitude(detic.latitude());
        geo.setLongitude(detic.longitude());

        Vector3d north = geo.getNorth();
        Vector3d east  = geo.getEast();
        Vector3d nadir = geo.getNadir();  // -zenth, 向下

        Vector3d B_ECF = X * north + Y * east + Z * nadir;
        data.ecfX_ = B_ECF.x();
        data.ecfY_ = B_ECF.y();
        data.ecfZ_ = B_ECF.z();

        // ---- 7. ECF → ECI ----
        Vector3d B_ECI;
        aECFToECI(data.time_, B_ECF, B_ECI);
        data.eciX_ = B_ECI.x();
        data.eciY_ = B_ECI.y();
        data.eciZ_ = B_ECI.z();
    }
    return rc;
}


AST_NAMESPACE_END

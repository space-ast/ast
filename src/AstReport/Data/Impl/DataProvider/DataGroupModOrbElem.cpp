///
/// @file      DataGroupModOrbElem.cpp
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

#include "DataGroupModOrbElem.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstUtil/Dimension.hpp"

AST_NAMESPACE_BEGIN


DataElements DataGroupModOrbElem::Elements()
{
    DataElements elements;
    // 1. Time (自变量)
    elements.addElement<Data, const TimePoint&, &Data::getTime>
    ("Time", Dimension::DateTime());
    // 2. Semi-major Axis
    elements.addElement<Data, double, &Data::getSMA>
    ("Semi-major Axis", Dimension::Length());
    // 3. Eccentricity
    elements.addElement<Data, double, &Data::getEcc>
    ("Eccentricity", Dimension::Unit());
    // 4. Inclination
    elements.addElement<Data, double, &Data::getInc>
    ("Inclination", Dimension::Angle());
    // 5. RAAN
    elements.addElement<Data, double, &Data::getRAAN>
    ("RAAN", Dimension::Angle());
    // 6. Arg of Perigee
    elements.addElement<Data, double, &Data::getArgPeri>
    ("Arg of Perigee", Dimension::Angle());
    // 7. True Anomaly
    elements.addElement<Data, double, &Data::getTrueAnomaly>
    ("True Anomaly", Dimension::Angle());
    // 8. Mean Anomaly
    elements.addElement<Data, double, &Data::getMeanAnomaly>
    ("Mean Anomaly", Dimension::Angle());
    // 9. Arg of Latitude
    elements.addElement<Data, double, &Data::getArgLat>
    ("Arg of Latitude", Dimension::Angle());
    // 10. Apogee Altitude
    elements.addElement<Data, double, &Data::getApoAlt>
    ("Apogee Altitude", Dimension::Length());
    // 11. Apogee Radius
    elements.addElement<Data, double, &Data::getApoRad>
    ("Apogee Radius", Dimension::Length());
    // 12. Perigee Altitude
    elements.addElement<Data, double, &Data::getPeriAlt>
    ("Perigee Altitude", Dimension::Length());
    // 13. Perigee Radius
    elements.addElement<Data, double, &Data::getPeriRad>
    ("Perigee Radius", Dimension::Length());
    // 14. Mean Motion (Revs/Day)
    elements.addElement<Data, double, &Data::getMeanMotion>
    ("Mean Motion (Revs/Day)", Dimension::Unit());
    // 15. Lon Ascn Node — 需要 Axes 参数，暂不支持
    // elements.addElement<Data, double, &Data::getLAN>
    // ("Lon Ascn Node", Dimension::Angle());
    // 16. Eccentric Anomaly
    elements.addElement<Data, double, &Data::getEccAnomaly>
    ("Eccentric Anomaly", Dimension::Angle());
    // 17. Time Past AN
    elements.addElement<Data, double, &Data::getTimePastAscNode>
    ("Time Past AN", Dimension::Time());
    // 18. Time Past Perigee
    elements.addElement<Data, double, &Data::getTimePastPeri>
    ("Time Past Perigee", Dimension::Time());
    // 19. Period
    elements.addElement<Data, double, &Data::getPeriod>
    ("Period", Dimension::Time());
    // 20. Longitude of Perigee
    elements.addElement<Data, double, &Data::getLongitudeOfPeri>
    ("Longitude of Perigee", Dimension::Angle());
    // 21. Mean Longitude
    elements.addElement<Data, double, &Data::getMeanLongitude>
    ("Mean Longitude", Dimension::Angle());
    return elements;
}


const DataElements &DataGroupModOrbElem::getElements() const
{
    static DataElements elements = Elements();
    return elements;
}

errc_t DataGroupModOrbElem::calculate(const TimeList &timeList, VariantVector &result) const
{
    size_t size = timeList.size();
    result.reset<Data>(size);
    return calculate(timeList, result.asSpan<Data>());
}

errc_t DataGroupModOrbElem::calculate(const TimeList &timeList, std::vector<Data> &result) const
{
    size_t size = timeList.size();
    result.resize(size);
    return calculate(timeList, Span<Data>(result));
}

errc_t DataGroupModOrbElem::calculate(const TimeList &timeList, Span<Data> result) const
{
    auto point = this->getPoint();
    auto frame = this->getFrame();
    if(!point || !frame)
    {
        aError("Point or Frame is null");
        return eErrorNullPtr;
    }
    auto body = frame->getBody();
    if(!body)
    {
        aError("Body is null");
        return eErrorNullPtr;
    }
    size_t size = result.size();
    if(size != timeList.size())
    {
        aError("result size must be equal to timeList size");
        return eErrorInvalidParam;
    }

    double bodyRadius = body->getRadius();
    double gm = body->getGM();

    errc_t rc = eNoError;
    for(size_t i = 0; i < size; ++i)
    {
        Data& data = result[i];
        data.bodyRadius_ = bodyRadius;
        data.gm_ = gm;
        data.time_ = timeList[i];
        Vector3d pos, vel;
        errc_t err = point->getPosVelIn(frame, data.time_, pos, vel);
        if(err != eNoError) {
            rc = err;
            continue;
        }
        err = aCartToModOrbElem(pos, vel, gm, data.modOrbElem_);
        if(err != eNoError) 
            rc = err;
    }

    return rc;
}

AST_NAMESPACE_END



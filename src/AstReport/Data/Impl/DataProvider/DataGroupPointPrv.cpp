///
/// @file      DataGroupPointPrv.cpp
/// @brief     点位置数据组 (派生量) 实现
/// @details   点的位置分量及派生量：模、方向余弦、球面角、方向角、速度
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

#include "DataGroupPointPrv.hpp"
#include "AstUtil/VariantVector.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Dimension.hpp"

AST_NAMESPACE_BEGIN

DataElements DataGroupPointPrv::Elements()
{
    DataElements elements;
    elements.addElement<Data, const TimePoint&, &Data::getTime>
    ("Time", Dimension::DateTime());
    elements.addElement<Data, double, &Data::getX>
    ("x", Dimension::Length());
    elements.addElement<Data, double, &Data::getY>
    ("y", Dimension::Length());
    elements.addElement<Data, double, &Data::getZ>
    ("z", Dimension::Length());
    elements.addElement<Data, double, &Data::getMagnitude>
    ("Magnitude", Dimension::Length());
    elements.addElement<Data, double, &Data::getXOverMag>
    ("x/Magnitude", Dimension::Unit());
    elements.addElement<Data, double, &Data::getYOverMag>
    ("y/Magnitude", Dimension::Unit());
    elements.addElement<Data, double, &Data::getZOverMag>
    ("z/Magnitude", Dimension::Unit());
    elements.addElement<Data, double, &Data::getRightAscension>
    ("RightAscension", Dimension::Angle());
    elements.addElement<Data, double, &Data::getDeclination>
    ("Declination", Dimension::Angle());
    elements.addElement<Data, double, &Data::getCoDeclination>
    ("Co-Declination", Dimension::Angle());
    elements.addElement<Data, double, &Data::getNegativeDeclination>
    ("NegativeDeclination", Dimension::Angle());
    elements.addElement<Data, double, &Data::getDirectionAngleX>
    ("DirectionAngle x", Dimension::Angle());
    elements.addElement<Data, double, &Data::getDirectionAngleY>
    ("DirectionAngle y", Dimension::Angle());
    elements.addElement<Data, double, &Data::getDirectionAngleZ>
    ("DirectionAngle z", Dimension::Angle());
    elements.addElement<Data, double, &Data::getVelocityX>
    ("Velocity x", Dimension::Speed());
    elements.addElement<Data, double, &Data::getVelocityY>
    ("Velocity y", Dimension::Speed());
    elements.addElement<Data, double, &Data::getVelocityZ>
    ("Velocity z", Dimension::Speed());
    elements.addElement<Data, double, &Data::getVelocityMagnitude>
    ("Velocity Magnitude", Dimension::Speed());
    // Detic 坐标 — 需要 BodyShape，暂不支持
    // elements.addElement<Data, double, &Data::getDeticLatitude>("Detic Latitude", ...);
    // elements.addElement<Data, double, &Data::getDeticLongitude>("Detic Longitude", ...);
    // elements.addElement<Data, double, &Data::getDeticAltitude>("Detic Altitude", ...);
    return elements;
}

errc_t DataGroupPointPrv::calculate(const TimeList& timeList, VariantVector& result) const
{
    size_t size = timeList.size();
    result.reset<Data>(size);
    return calculate(timeList, result.asSpan<Data>());
}

const DataElements& DataGroupPointPrv::getElements() const
{
    static DataElements elements = Elements();
    return elements;
}

errc_t DataGroupPointPrv::calculate(const TimeList& timeList, std::vector<Data>& result) const
{
    size_t size = timeList.size();
    result.resize(size);
    return calculate(timeList, Span<Data>(result));
}

errc_t DataGroupPointPrv::calculate(const TimeList& timeList, Span<Data> result) const
{
    Point* point = this->getPoint();
    Frame* frame = this->getFrame();
    if(!point || !frame)
    {
        aError("Point or Frame is null");
        return eErrorNullPtr;
    }

    size_t size = result.size();
    if(size != timeList.size())
    {
        aError("result size must be equal to timeList size");
        return eErrorInvalidParam;
    }

    errc_t rc = eNoError;
    for(size_t i = 0; i < size; i++)
    {
        Data& data = result[i];
        data.time_ = timeList[i];
        errc_t err = point->getPosVelIn(frame, data.time_, data.pos_, data.vel_);
        if(err != eNoError)
            rc = err;
    }
    return rc;
}


AST_NAMESPACE_END

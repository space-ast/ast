///
/// @file      DataGroupSpherical.cpp
/// @brief     球坐标轨道根数数据组实现
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

#include "DataGroupSpherical.hpp"
#include "AstUtil/VariantVector.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Dimension.hpp"
#include "AstUtil/Math.hpp"

AST_NAMESPACE_BEGIN

namespace
{

/// @brief 计算水平航迹角和惯性航迹方位角
/// @todo 待测试验证
/// @param pos 位置矢量
/// @param vel 速度矢量
/// @param flightPathAngle 输出：水平航迹角 (弧度), asin(v_radial/|v|)
/// @param flightPathAzimuth 输出：惯性航迹方位角 (弧度), 从惯性北向东量
void aCalcFlightPathAngles(const Vector3d& pos, const Vector3d& vel,
                           double& flightPathAngle, double& flightPathAzimuth)
{
    double r = pos.norm();
    double v = vel.norm();
    if(r < 1e-12 || v < 1e-12)
    {
        flightPathAngle   = 0.0;
        flightPathAzimuth = 0.0;
        return;
    }

    // 水平航迹角 = asin(v_radial / v)
    double vRadial = pos.dot(vel) / r;
    flightPathAngle = asinSafe(vRadial / v);

    // 惯性当地方向矢量
    Vector3d rHat = pos / r;
    Vector3d k = {0.0, 0.0, 1.0};  // ICRF Z轴 (天球北极)

    // 惯性东: k × rHat (归一化)
    Vector3d eHat = k.cross(rHat);
    double eNorm = eHat.norm();
    if(eNorm < 1e-12)
    {
        // 在天球北极/南极，方位角退化
        flightPathAzimuth = 0.0;
        return;
    }
    eHat = eHat / eNorm;

    // 惯性北: rHat × eHat (完成右手系，指向赤纬增大方向)
    Vector3d nHat = rHat.cross(eHat);

    // 航迹方位角 = atan2(v_east, v_north), 从惯性北向东量
    flightPathAzimuth = std::atan2(vel.dot(eHat), vel.dot(nHat));
}

} // namespace

DataElements DataGroupSpherical::Elements()
{
    DataElements elements;
    elements.addElement<Data, const TimePoint&, &Data::getTime>
    ("Time", Dimension::DateTime());
    elements.addElement<Data, double, &Data::getRightAscen>
    ("Right Ascen", Dimension::Angle());
    elements.addElement<Data, double, &Data::getDeclination>
    ("Declination", Dimension::Angle());
    elements.addElement<Data, double, &Data::getRadius>
    ("Radius", Dimension::Length());
    elements.addElement<Data, double, &Data::getHorizFltPathAng>
    ("Horiz Flt Path Ang", Dimension::Angle());
    elements.addElement<Data, double, &Data::getInertialFltPathAzi>
    ("Inertial Flt Path Azi", Dimension::Angle());
    elements.addElement<Data, double, &Data::getInertialVel>
    ("Inertial Vel", Dimension::Speed());
    return elements;
}

errc_t DataGroupSpherical::calculate(const TimeList& timeList, VariantVector& result) const
{
    size_t size = timeList.size();
    result.reset<Data>(size);
    return calculate(timeList, result.asSpan<Data>());
}

const DataElements& DataGroupSpherical::getElements() const
{
    static DataElements elements = Elements();
    return elements;
}

errc_t DataGroupSpherical::calculate(const TimeList& timeList, std::vector<Data>& result) const
{
    size_t size = timeList.size();
    result.resize(size);
    return calculate(timeList, Span<Data>(result));
}

errc_t DataGroupSpherical::calculate(const TimeList& timeList, Span<Data> result) const
{
    Point* scPoint = this->getScPoint();
    Frame* frame = this->getFrame();
    if(!scPoint || !frame)
    {
        aError("scPoint or frame is null");
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

        Vector3d pos, vel;
        errc_t err = scPoint->getPosVelIn(frame, data.time_, pos, vel);
        if(err != eNoError)
            { rc = err; continue; }

        double r = pos.norm();
        data.radius_ = r;
        data.rightAscen_ = (r < 1e-12) ? 0.0 : std::atan2(pos.y(), pos.x());
        data.declination_ = (r < 1e-12) ? 0.0 : asinSafe(pos.z() / r);

        data.inertialVel_ = vel.norm();

        aCalcFlightPathAngles(pos, vel,
                              data.horizFltPathAng_, data.inertialFltPathAzi_);
    }
    return rc;
}


AST_NAMESPACE_END

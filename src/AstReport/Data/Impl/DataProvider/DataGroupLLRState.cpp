///
/// @file      DataGroupLLRState.cpp
/// @brief     LLR 球坐标状态数据组实现
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

#include "DataGroupLLRState.hpp"
#include "AstUtil/VariantVector.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Dimension.hpp"
#include "AstUtil/Math.hpp"

AST_NAMESPACE_BEGIN

namespace
{

/// @brief 笛卡尔坐标转球坐标 (LLR) 及变化率
/// @todo 待测试验证
/// @param pos 位置矢量
/// @param vel 速度矢量
/// @param lat 输出：纬度 (弧度)
/// @param lon 输出：经度 (弧度)
/// @param rad 输出：地心距
/// @param latRate 输出：纬度变化率 (弧度/秒)
/// @param lonRate 输出：经度变化率 (弧度/秒)
/// @param radRate 输出：径向速度
void aCartesianToSpherical(const Vector3d& pos, const Vector3d& vel,
                           double& lat, double& lon, double& rad,
                           double& latRate, double& lonRate, double& radRate)
{
    double x = pos.x(), y = pos.y(), z = pos.z();
    double r2 = x * x + y * y;
    double r = std::sqrt(r2 + z * z);

    rad = r;
    lon = std::atan2(y, x);
    lat = (r < 1e-12) ? 0.0 : asinSafe(z / r);

    // 径向速度
    if(r < 1e-12)
    {
        radRate = 0.0;
        latRate = 0.0;
        lonRate = 0.0;
        return;
    }
    radRate = pos.dot(vel) / r;

    // 经度变化率
    if(r2 < 1e-12)
    {
        lonRate = 0.0;
        latRate = 0.0;
        return;
    }
    lonRate = (x * vel.y() - y * vel.x()) / r2;

    // 纬度变化率: dφ/dt = (vz*r - dr/dt*z) / (r * sqrt(x²+y²))
    double rho = std::sqrt(r2);
    if(rho < 1e-12)
    {
        latRate = 0.0;
        return;
    }
    latRate = (vel.z() * r - radRate * z) / (r * rho);
}

} // namespace

DataElements DataGroupLLRState::Elements()
{
    DataElements elements;
    elements.addElement<Data, const TimePoint&, &Data::getTime>
    ("Time", Dimension::DateTime());
    elements.addElement<Data, double, &Data::getLat>
    ("Lat", Dimension::Angle());
    elements.addElement<Data, double, &Data::getLon>
    ("Lon", Dimension::Angle());
    elements.addElement<Data, double, &Data::getRad>
    ("Rad", Dimension::Length());
    elements.addElement<Data, double, &Data::getLatRate>
    ("Lat Rate", Dimension::AngularVelocity());
    elements.addElement<Data, double, &Data::getLonRate>
    ("Lon Rate", Dimension::AngularVelocity());
    elements.addElement<Data, double, &Data::getRadRate>
    ("Rad Rate", Dimension::Speed());
    return elements;
}

errc_t DataGroupLLRState::calculate(const TimeList& timeList, VariantVector& result) const
{
    size_t size = timeList.size();
    result.reset<Data>(size);
    return calculate(timeList, result.asSpan<Data>());
}

const DataElements& DataGroupLLRState::getElements() const
{
    static DataElements elements = Elements();
    return elements;
}

errc_t DataGroupLLRState::calculate(const TimeList& timeList, std::vector<Data>& result) const
{
    size_t size = timeList.size();
    result.resize(size);
    return calculate(timeList, Span<Data>(result));
}

errc_t DataGroupLLRState::calculate(const TimeList& timeList, Span<Data> result) const
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

        aCartesianToSpherical(pos, vel,
                              data.lat_, data.lon_, data.rad_,
                              data.latRate_, data.lonRate_, data.radRate_);
    }
    return rc;
}


AST_NAMESPACE_END

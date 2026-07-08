///
/// @file      DataGroupLLAState.cpp
/// @brief     LLA 状态数据组实现
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

#include "DataGroupLLAState.hpp"
#include "AstUtil/VariantVector.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Dimension.hpp"
#include "AstCore/BodyShape.hpp"
#include "AstCore/GeoCoordinate.hpp"
#include "AstCore/SpheroidShape.hpp"
#include "AstCore/SphereShape.hpp"

AST_NAMESPACE_BEGIN

/// @brief 计算 LLA 变化率
/// @todo 待测试验证
/// @param lat 大地纬度 (弧度)
/// @param lon 大地经度 (弧度)
/// @param alt 大地高度
/// @param vel 天体固连系中的速度矢量
/// @param majorAxis 椭球体长半轴
/// @param flatFactor 扁率因子 (0 表示球体)
/// @param latRate 输出：纬度变化率 (弧度/秒)
/// @param lonRate 输出：经度变化率 (弧度/秒)
/// @param altRate 输出：高度变化率
void aCalcLLARates(double lat, double lon, double alt, const Vector3d& vel,
                   double majorAxis, double flatFactor,
                   double& latRate, double& lonRate, double& altRate)
{
    // 局部方向矢量
    GeoCoordinate geo;
    geo.setLatitude(lat);
    geo.setLongitude(lon);
    // alt 已默认 0，无需设置
    (void)alt; // alt 仅影响位置，不影响单位方向矢量

    Vector3d zenith = geo.getZenith();
    Vector3d east   = geo.getEast();
    Vector3d north  = geo.getNorth();

    // 高度变化率 = 速度在天顶方向的投影
    altRate = vel.dot(zenith);

    // 曲率半径
    double e2 = 2.0 * flatFactor - flatFactor * flatFactor;  // 第一偏心率平方
    double sinLat = std::sin(lat);
    double sinLat2 = sinLat * sinLat;
    double w = std::sqrt(1.0 - e2 * sinLat2);

    double N = majorAxis / w;                         // 卯酉圈曲率半径
    double M = majorAxis * (1.0 - e2) / (w * w * w);  // 子午圈曲率半径

    // 经度变化率 = 东向速度分量 / ((N + h) * cos(lat))
    double cosLat = std::cos(lat);
    if(std::abs(cosLat) > 1e-12)
        lonRate = vel.dot(east) / ((N + alt) * cosLat);
    else
        lonRate = 0.0;

    // 纬度变化率 = -北向速度分量 / (M + h)
    latRate = -vel.dot(north) / (M + alt);
}

DataElements DataGroupLLAState::Elements()
{
    DataElements elements;
    elements.addElement<Data, const TimePoint&, &Data::getTime>
    ("Time", Dimension::DateTime());
    elements.addElement<Data, double, &Data::getLat>
    ("Lat", Dimension::Angle());
    elements.addElement<Data, double, &Data::getLon>
    ("Lon", Dimension::Angle());
    elements.addElement<Data, double, &Data::getAlt>
    ("Alt", Dimension::Length());
    elements.addElement<Data, double, &Data::getLatRate>
    ("Lat Rate", Dimension::AngularVelocity());
    elements.addElement<Data, double, &Data::getLonRate>
    ("Lon Rate", Dimension::AngularVelocity());
    elements.addElement<Data, double, &Data::getAltRate>
    ("Alt Rate", Dimension::Speed());
    elements.addElement<Data, double, &Data::getCentricLat>
    ("Centric Lat", Dimension::Angle());
    elements.addElement<Data, double, &Data::getCentricLon>
    ("Centric Lon", Dimension::Angle());

    // 以下元素需要额外基础设施支持，暂未注册：
    // "Alt AGL"       — 需要地形高程数据
    // "Terrain Alt"   — 需要地形高程数据
    // "Alt MSL"       — 需要大地水准面模型
    // "UTM Zone"      — 需要 UTM 投影转换
    // "UTM Easting"   — 需要 UTM 投影转换
    // "UTM Northing"  — 需要 UTM 投影转换
    // "MGRS Cell"     — 需要 MGRS 转换
    return elements;
}

errc_t DataGroupLLAState::calculate(const TimeList& timeList, VariantVector& result) const
{
    size_t size = timeList.size();
    result.reset<Data>(size);
    return calculate(timeList, result.asSpan<Data>());
}

const DataElements& DataGroupLLAState::getElements() const
{
    static DataElements elements = Elements();
    return elements;
}

errc_t DataGroupLLAState::calculate(const TimeList& timeList, std::vector<Data>& result) const
{
    size_t size = timeList.size();
    result.resize(size);
    return calculate(timeList, Span<Data>(result));
}

errc_t DataGroupLLAState::calculate(const TimeList& timeList, Span<Data> result) const
{
    Point* scPoint = this->getScPoint();
    Body* body = this->getBody();
    if(!scPoint || !body)
    {
        aError("scPoint or body is null");
        return eErrorNullPtr;
    }

    Frame* bodyFixed = body->getFrameFixed();
    BodyShape* shape = body->getShape();
    if(!bodyFixed || !shape)
    {
        aError("bodyFixed or shape is null");
        return eErrorNullPtr;
    }

    double majorAxis = shape->majorAxis();

    // 获取扁率因子（仅 SpheroidShape 提供，其他形状默认为 0 即球体）
    // @todo 处理三轴椭球体形状的情况
    double flatFactor = 0.0;
    if(auto* spheroid = aobject_cast<SpheroidShape*>(shape))
        flatFactor = spheroid->flatFactor();

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
        errc_t err = scPoint->getPosVelIn(bodyFixed, data.time_, pos, vel);
        if(err != eNoError)
            { rc = err; continue; }

        // 笛卡尔 → 大地坐标
        shape->transform(pos, data.detic_);

        // 地心经纬度
        double r = std::sqrt(pos.x() * pos.x() + pos.y() * pos.y());
        data.centricLat_ = std::atan2(pos.z(), r);
        data.centricLon_ = std::atan2(pos.y(), pos.x());

        // LLA 变化率
        aCalcLLARates(data.detic_.latitude(), data.detic_.longitude(), data.detic_.altitude(),
                      vel, majorAxis, flatFactor,
                      data.latRate_, data.lonRate_, data.altRate_);
    }
    return rc;
}


AST_NAMESPACE_END

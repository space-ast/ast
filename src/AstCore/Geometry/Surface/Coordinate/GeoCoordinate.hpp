///
/// @file      GeoCoordinate.hpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-12
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
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
#include "LatLonAlt.hpp"
#include "AstMath/Vector.hpp"
#include "AstUtil/Math.hpp"
#include <math.h>

AST_NAMESPACE_BEGIN

class GeocentricPoint;
class GeodeticPoint;



/// @brief 地理坐标
class GeoCoordinate
{
public:
    GeoCoordinate() noexcept {};

    /// @brief 从纬度、经度、高度坐标构造
    constexpr GeoCoordinate(double lat, double lon, double alt = 0.0) noexcept
        : latLonAlt_{lat, lon, alt} {}
    
    /// @brief 从纬度、经度、高度坐标构造
    constexpr explicit GeoCoordinate(const LatLonAlt& latLonAlt) noexcept
        : latLonAlt_{latLonAlt} {}

public: // get and set methods

    /// @brief 获取纬度、经度、高度坐标
    constexpr const LatLonAlt& latLonAlt() const noexcept { return latLonAlt_; }
    /// @brief 设置纬度、经度、高度坐标
    constexpr void setLatLonAlt(const LatLonAlt& latLonAlt) noexcept{ latLonAlt_ = latLonAlt; }

    /// @brief 获取纬度
    double latitude() const { return latLonAlt_.latitude(); }
    /// @brief 获取经度
    double longitude() const { return latLonAlt_.longitude(); }
    /// @brief 获取高度
    double altitude() const { return latLonAlt_.altitude(); }
    /// @brief 设置纬度
    double& latitude() { return latLonAlt_.latitude(); }
    /// @brief 设置经度
    double& longitude() { return latLonAlt_.longitude(); }
    /// @brief 设置高度
    double& altitude() { return latLonAlt_.altitude(); }
    /// @brief 设置纬度
    void setLatitude(double lat) { latLonAlt_.setLatitude(lat); }
    /// @brief 设置经度
    void setLongitude(double lon) { latLonAlt_.setLongitude(lon); }
    /// @brief 设置高度
    void setAltitude(double alt) { latLonAlt_.setAltitude(alt); }

public: // as methods

    /// @brief 作为地心坐标使用
    GeocentricPoint& asGeocentricPoint() { return (GeocentricPoint&)*this; }
    /// @brief 作为大地坐标使用
    GeodeticPoint& asGeodeticPoint() { return (GeodeticPoint&)*this; }
public:
    /// @brief 获取天顶方向向量
    Vector3d getZenith() const;
    /// @brief 获取天底方向向量
    Vector3d getNadir() const;
    /// @brief 获取东方向向量
    Vector3d getEast() const;
    /// @brief 获取西方向向量
    Vector3d getWest() const;
    /// @brief 获取北方向向量
    Vector3d getNorth() const;
    /// @brief 获取南方向向量
    Vector3d getSouth() const;

public:
    LatLonAlt latLonAlt_;  ///< 纬度、经度、高度坐标
};

inline Vector3d GeoCoordinate::getZenith() const
{
    double lat = latitude();
    double lon = longitude();
    double sLat, cLat, sLon, cLon;
    sincos(lat, &sLat, &cLat);
    sincos(lon, &sLon, &cLon);
    return {cLon * cLat, sLon * cLat, sLat};    
}

inline Vector3d GeoCoordinate::getNadir() const
{
    return -getZenith();
}
            
inline Vector3d GeoCoordinate::getEast() const
{
    double sLon, cLon;
    sincos(longitude(), &sLon, &cLon);
    return {-sLon, cLon, 0};
}

inline Vector3d GeoCoordinate::getWest() const
{
    return -getEast();
}

inline Vector3d GeoCoordinate::getNorth() const
{
    double lat = latitude();
    double lon = longitude();
    double sLat, cLat, sLon, cLon;
    sincos(lat, &sLat, &cLat);
    sincos(lon, &sLon, &cLon);
    return {-cLon * sLat, -sLon * sLat, cLat};
}

inline Vector3d GeoCoordinate::getSouth() const
{
    return -getNorth();
}

/// @brief 将天体表面的地心坐标转换为大地坐标
/// @warning 只适用于天体表面的点，不适用于远离天体的点
/// @param centric 地心坐标
/// @param flatFact 扁率因子
/// @param detic 大地坐标
/// @return 
AST_CORE_CAPI void aSurfaceCentricToDetic(const GeocentricPoint& centric, double flatFact, GeodeticPoint& detic);


/// @brief 将天体表面的大地坐标转换为地心坐标
/// @warning 只适用于天体表面的点，不适用于远离天体的点
/// @param detic 大地坐标
/// @param flatFact 扁率因子
/// @param centric 地心坐标
/// @return 
AST_CORE_CAPI void aSurfaceDeticToCentric(const GeodeticPoint& detic, double flatFact, GeocentricPoint& centric);



/// @brief 将天体表面的地心纬度转换为大地纬度
/// @warning 只适用于天体表面的点，不适用于远离天体的点
/// @param centricLat 地心纬度
/// @param flatFact 扁率因子
/// @return 大地纬度
AST_CORE_CAPI double aSurfaceCentricToDeticLat(double centricLat, double flatFact);


/// @brief 将天体表面的地心纬度转换为大地纬度
/// @warning 只适用于天体表面的点，不适用于远离天体的点
/// @param centricLat 地心纬度
/// @param flatFact 扁率因子
/// @return 大地纬度
A_ALWAYS_INLINE double aSurfaceCentricToDetic(double centricLat, double flatFact)
{
	return aSurfaceCentricToDeticLat(centricLat, flatFact);
}

/// @brief 将天体表面的大地纬度转换为地心纬度
/// @warning 只适用于天体表面的点，不适用于远离天体的点
/// @param deticLat 大地纬度
/// @param flatFact 扁率因子
/// @return 地心纬度
AST_CORE_CAPI double aSurfaceDeticToCentricLat(double deticLat, double flatFact);


/// @brief 将天体表面的大地纬度转换为地心纬度
/// @warning 只适用于天体表面的点，不适用于远离天体的点
/// @param deticLat 大地纬度
/// @param flatFact 扁率因子
/// @return 地心纬度
A_ALWAYS_INLINE double aSurfaceDeticToCentric(double deticLat, double flatFact)
{
	return aSurfaceDeticToCentricLat(deticLat, flatFact);
}


AST_NAMESPACE_END

///
/// @file      LocalHorizonalFrame.hpp
/// @brief     局部水平坐标转换
/// @details   提供大地坐标到NED坐标、ENU坐标转换函数
/// @author    axel
/// @date      2026-06-01
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
#include "AstCore/GeodeticPoint.hpp"
#include "AstMath/Vector.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/

/// @brief 大地坐标(天体固连系)转NED坐标系的旋转变换
AST_CORE_CAPI void aGeodeticToNEDTransform(const GeodeticPoint& origin, Rotation& rot);

/// @brief 大地坐标(天体固连系)转ENU坐标系的旋转变换
AST_CORE_CAPI void aGeodeticToENUTransform(const GeodeticPoint& origin, Rotation& rot);


/// @brief 大地坐标转天体固连系坐标
/// @param[in] point 大地坐标
/// @param[out] bodyFixed 输出的天体固连系坐标
/// @param[in] radius 扁球体赤道半径
/// @param[in] flatFact 扁率
AST_CORE_API void aGeodeticToBodyFixed(const GeodeticPoint& point, Vector3d& bodyFixed, double radius, double flatFact);

/// @brief 天体固连系坐标转大地坐标
/// @param[in] bodyFixed 天体固连系坐标
/// @param[out] point 输出的大地坐标
/// @param[in] radius 扁球体赤道半径
/// @param[in] flatFact 扁率
AST_CORE_API void aBodyFixedToGeodetic(const Vector3d& bodyFixed, GeodeticPoint& point, double radius, double flatFact);


/// @brief 大地坐标转天体固连系坐标
/// @param[in] point 大地坐标
/// @param[out] bodyFixed 输出的天体固连系坐标
/// @param[in] radius 圆球半径
AST_CORE_API void aGeodeticToBodyFixed(const GeodeticPoint& point, Vector3d& bodyFixed, double radius);


/// @brief 天体固连系坐标转大地坐标
/// @param[in] bodyFixed 天体固连系坐标
/// @param[out] point 输出的大地坐标
/// @param[in] radius 圆球半径
AST_CORE_API void aBodyFixedToGeodetic(const Vector3d& bodyFixed, GeodeticPoint& point, double radius);


/// @brief 大地坐标转天体固连系坐标
/// @param[in] point 大地坐标
/// @param[out] bodyFixed 输出的天体固连系坐标
/// @param[in] xRadius 椭球体x轴半径
/// @param[in] yRadius 椭球体y轴半径
/// @param[in] zRadius 椭球体z轴半径
AST_CORE_API void aGeodeticToBodyFixed(const GeodeticPoint& point, Vector3d& bodyFixed, double xRadius, double yRadius, double zRadius);


/// @brief 天体固连系坐标转大地坐标
/// @param[in] bodyFixed 天体固连系坐标
/// @param[out] point 输出的大地坐标
/// @param[in] xRadius 椭球体x轴半径
/// @param[in] yRadius 椭球体y轴半径
/// @param[in] zRadius 椭球体z轴半径
AST_CORE_API void aBodyFixedToGeodetic(const Vector3d& bodyFixed, GeodeticPoint& point, double xRadius, double yRadius, double zRadius);


/// @brief 大地坐标及其变化率转天体固连系的位置与速度
/// @param[in] point 大地坐标
/// @param[in] rate 大地坐标变化率, 依次为经度率[rad/s]、纬度率[rad/s]、高度率[m/s]
/// @param[out] bodyFixed 输出的天体固连系位置
/// @param[out] vel 输出的天体固连系速度
/// @param[in] radius 扁球体赤道半径
/// @param[in] flatFact 扁率
/// @return 错误码，成功返回eNoError
/// @note rate 复用了 @ref LatLonAlt 的字段顺序, 但其纬度、经度、高度三个分量依次为
///       纬度率、经度率、高度率, 量纲也相应为角速度、角速度、速度;
///       扁率为0时退化为圆球, 与 @ref aGeodeticToBodyFixed(const GeodeticPoint&, Vector3d&, double) 等价;
///       圆球可视为零扁率的扁球体, 故不再单列圆球重载(避免与三轴椭球重载混淆)。
AST_CORE_API errc_t aGeodeticToBodyFixed(const GeodeticPoint& point, const LatLonAlt& rate,
                                         Vector3d& bodyFixed, Vector3d& vel,
                                         double radius, double flatFact);


/// @brief 天体固连系的位置与速度转大地坐标及其变化率
/// @param[in] bodyFixed 天体固连系位置
/// @param[in] vel 天体固连系速度
/// @param[out] point 输出的大地坐标
/// @param[out] rate 输出的大地坐标变化率, 依次为经度率[rad/s]、纬度率[rad/s]、高度率[m/s]
/// @param[in] radius 扁球体赤道半径
/// @param[in] flatFact 扁率
/// @return 错误码，成功返回eNoError
/// @note 速度在天体固连系下按当地东、北、天方向分解:
///       经度率 = v_e / ((N+h)cos(lat)), 纬度率 = v_n / (M+h), 高度率 = v_u,
///       其中 N、M 为卯酉圈与子午圈曲率半径;
///       极点(lat=±90°)处经度率奇异, 取0(约定);
///       位置为零矢量或退化到参考椭球中心附近时返回eErrorInvalidParam。
AST_CORE_API errc_t aBodyFixedToGeodetic(const Vector3d& bodyFixed, const Vector3d& vel,
                                         GeodeticPoint& point, LatLonAlt& rate,
                                         double radius, double flatFact);


/// @brief 天体固连系坐标转NED坐标
/// @param posInBodyFixed 天体固连系坐标
/// @param origin 局部NED系原点
/// @param ned 输出的NED坐标
AST_CORE_API void aGeodeticToNED(const Vector3d& posInBodyFixed, const GeodeticPoint& origin, Vector3d& ned, BodyShape* bodyShape);

/// @brief 大地坐标转NED坐标
/// @param point 大地坐标
/// @param origin 局部NED系原点
/// @param ned 输出的NED坐标
AST_CORE_API void aGeodeticToNED(const GeodeticPoint& point, const GeodeticPoint& origin, Vector3d& ned, BodyShape* bodyShape);

/// @brief NED坐标转大地坐标
/// @param ned NED坐标
/// @param origin 局部NED系原点
/// @param point 输出的大地坐标
AST_CORE_API void aNEDToGeodetic(const Vector3d& ned, const GeodeticPoint& origin, GeodeticPoint& point, BodyShape* bodyShape);

/// @brief 天体固连系坐标转ENU坐标
/// @param posInBodyFixed 天体固连系坐标
/// @param origin 局部ENU系原点
/// @param enu 输出的ENU坐标
AST_CORE_API void aGeodeticToENU(const Vector3d& posInBodyFixed, const GeodeticPoint& origin, Vector3d& enu, BodyShape* bodyShape);

/// @brief 大地坐标转ENU坐标
/// @param point 大地坐标
/// @param origin 局部ENU系原点
/// @param enu 输出的ENU坐标
AST_CORE_API void aGeodeticToENU(const GeodeticPoint& point, const GeodeticPoint& origin, Vector3d& enu, BodyShape* bodyShape);

/// @brief ENU坐标转大地坐标
/// @param enu ENU坐标
/// @param origin 局部ENU系原点
/// @param point 输出的大地坐标
AST_CORE_API void aENUToGeodetic(const Vector3d& enu, const GeodeticPoint& origin, GeodeticPoint& point, BodyShape* bodyShape);


// ----------------------------------
// MATLAB Navigation Toolbox 同名函数
// ----------------------------------


A_ALWAYS_INLINE Vector3d lla2ned(const GeodeticPoint& lla, const GeodeticPoint& lla0, BodyShape* bodyShape)
{
    Vector3d ned;
    aGeodeticToNED(lla, lla0, ned, bodyShape);
    return ned;
}

A_ALWAYS_INLINE GeodeticPoint ned2lla(const Vector3d& ned, const GeodeticPoint& lla0, BodyShape* bodyShape)
{
    GeodeticPoint target;
    aNEDToGeodetic(ned, lla0, target, bodyShape);
    return target;
}

A_ALWAYS_INLINE Vector3d lla2enu(const GeodeticPoint& lla, const GeodeticPoint& lla0, BodyShape* bodyShape)
{
    Vector3d enu;
    aGeodeticToENU(lla, lla0, enu, bodyShape);
    return enu;
}

A_ALWAYS_INLINE GeodeticPoint enu2lla(const Vector3d& enu, const GeodeticPoint& lla0, BodyShape* bodyShape)
{
    GeodeticPoint target;
    aENUToGeodetic(enu, lla0, target, bodyShape);
    return target;
}


/*! @} */

AST_NAMESPACE_END

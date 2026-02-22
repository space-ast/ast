///
/// @file      MoonFrame.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-29
/// @copyright 版权所有 (C) 2026-present, ast项目.
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

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Coordinate
    @{
*/

class Rotation;
class KinematicRotation;

/// @brief 计算地球ICRF坐标系到月球ICRF坐标系的转换
/// @note 该函数依赖DE星历数据
/// @param tp 时间点
/// @param posInEarthICRF 地球ICRF坐标系下的位置向量
/// @param posInMoonICRF 月球ICRF坐标系下的位置向量
/// @return err_t 错误码
AST_CORE_API err_t aEarthICRFToMoonICRF(
    const TimePoint& tp, 
    const Vector3d&  posInEarthICRF, 
    Vector3d&        posInMoonICRF
);


/// @brief 计算地球ICRF坐标系到月球ICRF坐标系的转换
/// @note 该函数依赖DE星历数据
/// @param tp 时间点
/// @param posInEarthICRF 地球ICRF坐标系下的位置向量
/// @param velInEarthICRF 地球ICRF坐标系下的速度向量
/// @param posInMoonICRF 月球ICRF坐标系下的位置向量
/// @param velInMoonICRF 月球ICRF坐标系下的速度向量
/// @return err_t 错误码
AST_CORE_API err_t aEarthICRFToMoonICRF(
    const TimePoint& tp, 
    const Vector3d&  posInEarthICRF, 
    const Vector3d&  velInEarthICRF,
    Vector3d&        posInMoonICRF,  
    Vector3d&        velInMoonICRF
);


/// @brief 计算ICRF到月球主轴系(PA)的转换
/// @note 该函数依赖DE星历数据里的月球平动角
/// @param tp 时间点
/// @param rotation 旋转矩阵
/// @return err_t 错误码
AST_CORE_API err_t aICRFToMoonPrincipalAxesTransform(
    const TimePoint& tp, 
    Rotation&        rotation
);


/// @brief 计算ICRF到月球主轴系(PA)的转换
/// @note 该函数依赖DE星历数据里的月球平动角
/// @todo 还未实现该函数，需要实现旋转角速度的计算
/// @param tp 时间点
/// @param rotation 旋转矩阵
/// @return err_t 错误码
AST_CORE_API err_t aICRFToMoonPrincipalAxesTransform(
    const TimePoint& tp, 
    KinematicRotation& rotation
);


/// @brief 计算月球主轴系(PA)到月球MeanEarth系的转换
/// @note 该函数依赖DE星历数据
/// @param rotation 旋转
/// @return err_t 错误码
AST_CORE_CAPI err_t aMoonPAToMeanEarthTransform(
    Rotation&        rotation
);


/// @brief 计算月球主轴系(PA430)到月球MeanEarth系的转换
/// @param rotation 旋转
/// @return err_t 错误码
AST_CORE_CAPI void aMoonPA430ToMeanEarthTransform(
    Rotation&        rotation
);


/// @brief 计算月球主轴系(PA421)到月球MeanEarth系的转换
/// @param rotation 旋转
/// @return err_t 错误码
AST_CORE_CAPI void aMoonPA421ToMeanEarthTransform(
    Rotation& rotation
);


/// @brief 计算月球主轴系(PA418)到月球MeanEarth系的转换
/// @param rotation 旋转
/// @return err_t 错误码
AST_CORE_CAPI void aMoonPA418ToMeanEarthTransform(
    Rotation&        rotation
);


/// @brief 计算月球主轴系(PA403)到月球MeanEarth系的转换
/// @param rotation 旋转
/// @return err_t 错误码
AST_CORE_CAPI void aMoonPA403ToMeanEarthTransform(
    Rotation& rotation
);

/// @brief 计算月球ICRF坐标系到月球MeanEarth系的转换(依赖DE星历)
/// @note 该函数依赖DE星历数据
/// @param tp 时间点
/// @param rotation 旋转
/// @return err_t 错误码
AST_CORE_CAPI err_t aICRFToMoonMeanEarthTransform_DE(
    const TimePoint& tp, 
    Rotation&        rotation
);


/// @brief 计算月球ICRF坐标系到月球MeanEarth系的转换(依赖IAU旋转系数)
/// @todo 还未实现该函数
/// @param tp 
/// @param rotation 
/// @return 
AST_CORE_CAPI err_t aICRFToMoonMeanEarthTransform_IAU(
    const TimePoint& tp, 
    Rotation&        rotation
);

/// @brief 计算月球ICRF坐标系到月球MeanEarth系的转换(默认为DE实现)
/// @param tp 时间点
/// @param rotation 旋转
/// @return err_t 错误码
AST_CORE_CAPI err_t aICRFToMoonMeanEarthTransform(
    const TimePoint& tp, 
    Rotation&        rotation
);

/*! @} */


AST_NAMESPACE_END

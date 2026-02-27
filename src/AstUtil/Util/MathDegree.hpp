///
/// @file      MathDegree.hpp
/// @brief     角度量相关函数
/// @details   
/// @author    axel
/// @date      2026-02-27
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
#include "AstUtil/Constants.h"
#include <cmath>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Util
    @{
*/

/// @brief 将弧度转换为角度
/// @param x 输入弧度
/// @return 转换后的角度值
A_ALWAYS_INLINE double rad2deg(double x)
{
    return x * kRadToDeg;
}

/// @brief 将角度转换为弧度
/// @param x 输入角度
/// @return 转换后的弧度值
A_ALWAYS_INLINE double deg2rad(double x)
{
    return x * kDegToRad;
}


/// @brief 计算正弦值(以度为单位)
/// @param x 角度(以度为单位)
/// @return 正弦值
A_ALWAYS_INLINE double sind(double x)
{
    return sin(deg2rad(x));
}

/// @brief 计算余弦值(以度为单位)
/// @param x 角度(以度为单位)
/// @return 余弦值
A_ALWAYS_INLINE double cosd(double x)
{
    return cos(deg2rad(x));
}

/// @brief 计算正切值(以度为单位)
/// @param x 角度(以度为单位)
/// @return 正切值
A_ALWAYS_INLINE double tand(double x)
{
    return tan(deg2rad(x));
}


/// @brief 计算反正弦(以度为单位)
/// @param x 正弦值(范围[-1, 1])
/// @return 角度(以度为单位)
A_ALWAYS_INLINE double asind(double x)
{
    return rad2deg(asin(x));
}


/// @brief 计算反余弦(以度为单位)
/// @param x 余弦值(范围[-1, 1])
/// @return 角度(以度为单位)
A_ALWAYS_INLINE double acosd(double x)
{
    return rad2deg(acos(x));
}


/// @brief 计算反正切(以度为单位)
/// @param x 正切值
/// @return 角度(以度为单位)
A_ALWAYS_INLINE double atand(double x)
{
    return rad2deg(atan(x));
}


/// @brief 计算四象限反正切(以度为单位)
/// @param y 垂直坐标
/// @param x 水平坐标
/// @return 角度(以度为单位)
A_ALWAYS_INLINE double atan2d(double y, double x)
{
    return rad2deg(atan2(y, x));
}



/*! @} */

AST_NAMESPACE_END

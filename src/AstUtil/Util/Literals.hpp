///
/// @file      Literals.hpp
/// @brief     提供各种物理量单位的自定义字面量支持
/// @details   本文件定义了一系列用于单位转换的自定义字面量运算符，
///            包括角度单位、长度单位、时间单位和天文单位等，使代码中的
///            单位转换更加直观和可读。所有转换均基于Constants.h中定义的常量。
/// @author    axel
/// @date      21.11.2025
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
 
#include "Constants.h"

AST_NAMESPACE_BEGIN

/// @namespace ast::literals
/// @brief 包含自定义字面量运算符，包含角度单位、长度单位、时间单位和天文单位等
/// @ingroup Util
namespace literals
{
    // 角度单位 - 基于弧度的单位制. 所有角度转换最终表示为弧度

    /// @brief 将长双精度浮点数值作为弧度值处理
    /// @param val 弧度值
    /// @return 弧度值（直接返回，因为内部计算使用弧度）
    inline constexpr double operator ""_rad(long double val)
    {
        return val;
    }
    
    /// @brief 将无符号长整数值作为弧度值处理
    /// @param val 弧度值
    /// @return 弧度值（直接返回，因为内部计算使用弧度）
    inline constexpr double operator ""_rad(unsigned long long val)
    {
        return static_cast<double>(val);
    }
    
    /// @brief 将长双精度浮点数值从角度转换为弧度
    /// @param val 角度值
    /// @return 对应的弧度值（角度值 * kDegToRad）
    inline constexpr double operator ""_deg(long double val)
    {
        return val * kDegToRad;
    }
    
    /// @brief 将无符号长整数值从角度转换为弧度
    /// @param val 角度值
    /// @return 对应的弧度值（角度值 * kDegToRad）
    inline constexpr double operator ""_deg(unsigned long long val)
    {
        return val * kDegToRad;
    }

    /// @brief 将长双精度浮点数值从弧秒转换为弧度
    /// @param val 弧秒值
    /// @return 对应的弧度值（弧秒值 * kArcSecToRad）
    inline constexpr double operator ""_arcsec(long double val)
    {
        return val * kArcSecToRad;
    }
    
    /// @brief 将无符号长整数值从弧秒转换为弧度
    /// @param val 弧秒值
    /// @return 对应的弧度值（弧秒值 * kArcSecToRad）
    inline constexpr double operator ""_arcsec(unsigned long long val)
    {
        return val * kArcSecToRad;
    }

    // 长度单位 - 基于米的单位制. 所有长度转换最终表示为米

    /// @brief 将长双精度浮点数值从千米转换为米
    /// @param val 千米值
    /// @return 对应的米值（千米值 * kKilometerToMeter）
    inline constexpr double operator ""_km(long double val)
    {
        return val * kKilometerToMeter;
    }
    
    /// @brief 将无符号长整数值从千米转换为米
    /// @param val 千米值
    /// @return 对应的米值（千米值 * kKilometerToMeter）
    inline constexpr double operator ""_km(unsigned long long val)
    {
        return val * kKilometerToMeter;
    }
    
    /// @brief 将长双精度浮点数值作为米值处理
    /// @param val 米值
    /// @return 米值（直接返回，因为内部计算使用米）
    inline constexpr double operator ""_m(long double val)
    {
        return val;
    }
    
    /// @brief 将无符号长整数值作为米值处理
    /// @param val 米值
    /// @return 米值（直接返回，因为内部计算使用米）
    inline constexpr double operator ""_m(unsigned long long val)
    {
        return static_cast<double>(val);
    }
    
    /// @brief 将长双精度浮点数值从厘米转换为米
    /// @param val 厘米值
    /// @return 对应的米值（厘米值 * kCentimeterToMeter）
    inline constexpr double operator ""_cm(long double val)
    {
        return val * kCentimeterToMeter;
    }
    
    /// @brief 将无符号长整数值从厘米转换为米
    /// @param val 厘米值
    /// @return 对应的米值（厘米值 * kCentimeterToMeter）
    inline constexpr double operator ""_cm(unsigned long long val)
    {
        return val * kCentimeterToMeter;
    }
    
    /// @brief 将长双精度浮点数值从毫米转换为米
    /// @param val 毫米值
    /// @return 对应的米值（毫米值 * kMillimeterToMeter）
    inline constexpr double operator ""_mm(long double val)
    {
        return val * kMillimeterToMeter;
    }
    
    /// @brief 将无符号长整数值从毫米转换为米
    /// @param val 毫米值
    /// @return 对应的米值（毫米值 * kMillimeterToMeter）
    inline constexpr double operator ""_mm(unsigned long long val)
    {
        return val * kMillimeterToMeter;
    }
    
    /// @brief 将长双精度浮点数值从微米转换为米
    /// @param val 微米值
    /// @return 对应的米值（微米值 * kMicroMeterToMeter）
    inline constexpr double operator ""_um(long double val)
    {
        return val * kMicroMeterToMeter;
    }
    
    /// @brief 将无符号长整数值从微米转换为米
    /// @param val 微米值
    /// @return 对应的米值（微米值 * kMicroMeterToMeter）
    inline constexpr double operator ""_um(unsigned long long val)
    {
        return val * kMicroMeterToMeter;
    }
    
    /// @brief 将长双精度浮点数值从纳米转换为米
    /// @param val 纳米值
    /// @return 对应的米值（纳米值 * kNanometerToMeter）
    inline constexpr double operator ""_nm(long double val)
    {
        return val * kNanometerToMeter;
    }
    
    /// @brief 将无符号长整数值从纳米转换为米
    /// @param val 纳米值
    /// @return 对应的米值（纳米值 * kNanometerToMeter）
    inline constexpr double operator ""_nm(unsigned long long val)
    {
        return val * kNanometerToMeter;
    }

    /// @brief 将长双精度浮点数值从天文学单位转换为米
    /// @param val 天文单位值
    /// @return 对应的米值（天文单位值 * kAU）
    inline constexpr double operator ""_au(long double val)
    {
        return val * kAU;
    }
    
    /// @brief 将无符号长整数值从天文学单位转换为米
    /// @param val 天文单位值
    /// @return 对应的米值（天文单位值 * kAU）
    inline constexpr double operator ""_au(unsigned long long val)
    {
        return val * kAU;
    }


    // 时间单位 - 基于秒的单位制. 所有时间转换最终表示为秒
    
    /// @brief 将长双精度浮点数值作为秒值处理
    /// @param val 秒值
    /// @return 秒值（直接返回，因为内部计算使用秒）
    inline constexpr double operator ""_s(long double val)
    {
        return val;
    }
    
    /// @brief 将无符号长整数值作为秒值处理
    /// @param val 秒值
    /// @return 秒值（直接返回，因为内部计算使用秒）
    inline constexpr double operator ""_s(unsigned long long val)
    {
        return static_cast<double>(val);
    }
    
    /// @brief 将长双精度浮点数值从分钟转换为秒
    /// @param val 分钟值
    /// @return 对应的秒值（分钟值 * kMinToSec）
    inline constexpr double operator ""_min(long double val)
    {
        return val * kMinToSec;
    }
    
    /// @brief 将无符号长整数值从分钟转换为秒
    /// @param val 分钟值
    /// @return 对应的秒值（分钟值 * kMinToSec）
    inline constexpr double operator ""_min(unsigned long long val)
    {
        return val * kMinToSec;
    }
    
    /// @brief 将长双精度浮点数值从小时转换为秒
    /// @param val 小时值
    /// @return 对应的秒值（小时值 * kHourToSec）
    inline constexpr double operator ""_h(long double val)
    {
        return val * kHourToSec;
    }
    
    /// @brief 将无符号长整数值从小时转换为秒
    /// @param val 小时值
    /// @return 对应的秒值（小时值 * kHourToSec）
    inline constexpr double operator ""_h(unsigned long long val)
    {
        return val * kHourToSec;
    }
    
    /// @brief 将长双精度浮点数值从天转换为秒
    /// @param val 天数值
    /// @return 对应的秒值（天数值 * kDayToSec）
    inline constexpr double operator ""_day(long double val)
    {
        return val * kDayToSec;
    }
    
    /// @brief 将无符号长整数值从天转换为秒
    /// @param val 天数值
    /// @return 对应的秒值（天数值 * kDayToSec）
    inline constexpr double operator ""_day(unsigned long long val)
    {
        return val * kDayToSec;
    }
    
    /// @brief 将长双精度浮点数值从毫秒转换为秒
    /// @param val 毫秒值
    /// @return 对应的秒值（毫秒值 * kMilliSecToSec）
    inline constexpr double operator ""_ms(long double val)
    {
        return val * kMilliSecToSec;
    }
    
    /// @brief 将无符号长整数值从毫秒转换为秒
    /// @param val 毫秒值
    /// @return 对应的秒值（毫秒值 * kMilliSecToSec）
    inline constexpr double operator ""_ms(unsigned long long val)
    {
        return val * kMilliSecToSec;
    }
    
    /// @brief 将长双精度浮点数值从微秒转换为秒
    /// @param val 微秒值
    /// @return 对应的秒值（微秒值 * kMicroSecToSec）
    inline constexpr double operator ""_us(long double val)
    {
        return val * kMicroSecToSec;
    }
    
    /// @brief 将无符号长整数值从微秒转换为秒
    /// @param val 微秒值
    /// @return 对应的秒值（微秒值 * kMicroSecToSec）
    inline constexpr double operator ""_us(unsigned long long val)
    {
        return val * kMicroSecToSec;
    }
    
    /// @brief 将长双精度浮点数值从纳秒转换为秒
    /// @param val 纳秒值
    /// @return 对应的秒值（纳秒值 * kNanoSecToSec）
    inline constexpr double operator ""_ns(long double val)
    {
        return val * kNanoSecToSec;
    }
    
    /// @brief 将无符号长整数值从纳秒转换为秒
    /// @param val 纳秒值
    /// @return 对应的秒值（纳秒值 * kNanoSecToSec）
    inline constexpr double operator ""_ns(unsigned long long val)
    {
        return val * kNanoSecToSec;
    }
    
    /// @brief 将长双精度浮点数值从皮秒转换为秒
    /// @param val 皮秒值
    /// @return 对应的秒值（皮秒值 * kPicoSecToSec）
    inline constexpr double operator ""_ps(long double val)
    {
        return val * kPicoSecToSec;
    }
    
    /// @brief 将无符号长整数值从皮秒转换为秒
    /// @param val 皮秒值
    /// @return 对应的秒值（皮秒值 * kPicoSecToSec）
    inline constexpr double operator ""_ps(unsigned long long val)
    {
        return val * kPicoSecToSec;
    }


    // 速度单位 - 基于m/s的单位制
    
    /// @brief 将长双精度浮点数值作为米/秒值处理
    /// @param val 米/秒值
    /// @return 米/秒值（直接返回，因为内部计算使用米/秒）
    inline constexpr double operator ""_m_s(long double val)
    {
        return val;
    }
    
    /// @brief 将无符号长整数值作为米/秒值处理
    /// @param val 米/秒值
    /// @return 米/秒值（直接返回，因为内部计算使用米/秒）
    inline constexpr double operator ""_m_s(unsigned long long val)
    {
        return static_cast<double>(val);
    }
    
    /// @brief 将长双精度浮点数值从千米/秒转换为米/秒
    /// @param val 千米/秒值
    /// @return 对应的米/秒值（千米/秒值 * kKilometerToMeter）
    inline constexpr double operator ""_km_s(long double val)
    {
        return val * kKilometerToMeter;
    }
    
    /// @brief 将无符号长整数值从千米/秒转换为米/秒
    /// @param val 千米/秒值
    /// @return 对应的米/秒值（千米/秒值 * kKilometerToMeter）
    inline constexpr double operator ""_km_s(unsigned long long val)
    {
        return val * kKilometerToMeter;
    }
    
    /// @brief 将长双精度浮点数值从千米/小时转换为米/秒
    /// @param val 千米/小时值
    /// @return 对应的米/秒值（千米/小时值 * kKilometerToMeter / kHourToSec）
    inline constexpr double operator ""_km_h(long double val)
    {
        return val * kKilometerToMeter / kHourToSec;
    }
    
    /// @brief 将无符号长整数值从千米/小时转换为米/秒
    /// @param val 千米/小时值
    /// @return 对应的米/秒值（千米/小时值 * kKilometerToMeter / kHourToSec）
    inline constexpr double operator ""_km_h(unsigned long long val)
    {
        return val * kKilometerToMeter / kHourToSec;
    }

    // 角速度单位 - 基于 rad/s的单位制
    
    /// @brief 将长双精度浮点数值作为弧度/秒值处理
    /// @param val 弧度/秒值
    /// @return 弧度/秒值（直接返回，因为内部计算使用弧度/秒）
    inline constexpr double operator ""_rad_s(long double val)
    {
        return val;
    }
    
    /// @brief 将无符号长整数值作为弧度/秒值处理
    /// @param val 弧度/秒值
    /// @return 弧度/秒值（直接返回，因为内部计算使用弧度/秒）
    inline constexpr double operator ""_rad_s(unsigned long long val)
    {
        return static_cast<double>(val);
    }
    
    /// @brief 将长双精度浮点数值从角度/秒转换为弧度/秒
    /// @param val 角度/秒值
    /// @return 对应的弧度/秒值（角度/秒值 * kDegToRad）
    inline constexpr double operator ""_deg_s(long double val)
    {
        return val * kDegToRad;
    }
    
    /// @brief 将无符号长整数值从角度/秒转换为弧度/秒
    /// @param val 角度/秒值
    /// @return 对应的弧度/秒值（角度/秒值 * kDegToRad）
    inline constexpr double operator ""_deg_s(unsigned long long val)
    {
        return val * kDegToRad;
    }

    //----------------
    // unit literals
    //----------------

    constexpr double deg = 1_deg;
    constexpr double rad = 1_rad;
    constexpr double arcsec = 1_arcsec;

    constexpr double m = 1_m;
    constexpr double km = 1_km;
    constexpr double cm = 1_cm;
    constexpr double au = 1_au;
    

    constexpr double s = 1_s;
    constexpr double sec = 1_s;
    constexpr double ms = 1_ms;
    constexpr double min = 1_min;
    constexpr double h = 1_h;
}

// 导入字面量命名空间到ast命名空间，便于使用
using namespace literals;

AST_NAMESPACE_END

/// @example
/// 以下是如何使用这些自定义字面量的示例：
/// 
/// @code
/// // 角度转换示例
/// double angle1 = 90.0_deg;      // 90度转换为弧度
/// double angle2 = 3.14159_rad;   // 弧度值
/// 
/// // 长度转换示例
/// double distance1 = 5.5_km;     // 5.5公里转换为米
/// double distance2 = 1000_cm;    // 1000厘米转换为米
/// double distance3 = 1.0_au;     // 1天文单位转换为米
/// 
/// // 时间转换示例
/// double time1 = 24_h;           // 24小时转换为秒
/// double time2 = 30_min;         // 30分钟转换为秒
/// double time3 = 500_ms;         // 500毫秒转换为秒
/// 
/// // 速度转换示例
/// double speed1 = 300000_km_s;   // 300000千米/秒转换为米/秒
/// double speed2 = 100_km_h;      // 100千米/小时转换为米/秒
/// 
/// // 角速度转换示例
/// double angSpeed1 = 30_deg_s;   // 30度/秒转换为弧度/秒
/// double angSpeed2 = 0.1_rad_s;  // 0.1弧度/秒
/// 
/// // 在计算中的使用
/// double speed = 100_km / 1_h;   // 计算速度（米/秒）
/// double frequency = 1.0 / 1_ms; // 计算频率（Hz）
/// @endcode
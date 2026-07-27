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
#include "AstGlobal.h"
#include "Constants.h"

AST_NAMESPACE_BEGIN

/// @brief 包含自定义字面量运算符，包含角度单位、长度单位、时间单位和天文单位等
/// @ingroup Util
namespace literals
{
    // 角度单位 - 基于弧度的单位制. 所有角度转换最终表示为弧度

    /// @param val 弧度值
    inline constexpr double operator ""_rad(long double val)
    {
        return val;
    }
    
    /// @param val 弧度值
    inline constexpr double operator ""_rad(unsigned long long val)
    {
        return static_cast<double>(val);
    }
    
    /// @param val 角度值
    inline constexpr double operator ""_deg(long double val)
    {
        return val * kDegToRad;
    }
    
    /// @param val 角度值
    inline constexpr double operator ""_deg(unsigned long long val)
    {
        return val * kDegToRad;
    }

    /// @param val 弧秒值
    inline constexpr double operator ""_arcsec(long double val)
    {
        return val * kArcSecToRad;
    }
    
    /// @param val 弧秒值
    inline constexpr double operator ""_arcsec(unsigned long long val)
    {
        return val * kArcSecToRad;
    }

    /// @param val 周期数
    inline constexpr double operator ""_revs(long double val)
    {
        return val * kRevolutionToRad;
    }
    
    /// @param val 周期数
    inline constexpr double operator ""_revs(unsigned long long val)
    {
        return val * kRevolutionToRad;
    }

    // 长度单位 - 基于米的单位制. 所有长度转换最终表示为米

    /// @param val 千米值
    inline constexpr double operator ""_km(long double val)
    {
        return val * kKilometerToMeter;
    }
    
    /// @param val 千米值
    inline constexpr double operator ""_km(unsigned long long val)
    {
        return val * kKilometerToMeter;
    }
    
    /// @param val 米值
    inline constexpr double operator ""_m(long double val)
    {
        return val;
    }
    
    /// @param val 米值
    inline constexpr double operator ""_m(unsigned long long val)
    {
        return static_cast<double>(val);
    }
    
    /// @param val 厘米值
    inline constexpr double operator ""_cm(long double val)
    {
        return val * kCentimeterToMeter;
    }
    
    /// @param val 厘米值
    inline constexpr double operator ""_cm(unsigned long long val)
    {
        return val * kCentimeterToMeter;
    }
    
    /// @param val 毫米值
    inline constexpr double operator ""_mm(long double val)
    {
        return val * kMillimeterToMeter;
    }
    
    /// @param val 毫米值
    inline constexpr double operator ""_mm(unsigned long long val)
    {
        return val * kMillimeterToMeter;
    }
    
    /// @param val 微米值
    inline constexpr double operator ""_um(long double val)
    {
        return val * kMicroMeterToMeter;
    }
    
    /// @param val 微米值
    inline constexpr double operator ""_um(unsigned long long val)
    {
        return val * kMicroMeterToMeter;
    }
    
    /// @param val 纳米值
    inline constexpr double operator ""_nm(long double val)
    {
        return val * kNanometerToMeter;
    }
    
    /// @param val 纳米值
    inline constexpr double operator ""_nm(unsigned long long val)
    {
        return val * kNanometerToMeter;
    }

    /// @param val 天文单位值
    inline constexpr double operator ""_au(long double val)
    {
        return val * kAU;
    }
    
    /// @param val 天文单位值
    inline constexpr double operator ""_au(unsigned long long val)
    {
        return val * kAU;
    }


    // 时间单位 - 基于秒的单位制. 所有时间转换最终表示为秒
    
    /// @param val 秒值
    inline constexpr double operator ""_s(long double val)
    {
        return val;
    }
    
    /// @param val 秒值
    inline constexpr double operator ""_s(unsigned long long val)
    {
        return static_cast<double>(val);
    }
    
    /// @param val 分钟值
    inline constexpr double operator ""_min(long double val)
    {
        return val * kMinToSec;
    }
    
    /// @param val 分钟值
    inline constexpr double operator ""_min(unsigned long long val)
    {
        return val * kMinToSec;
    }
    
    /// @param val 小时值
    inline constexpr double operator ""_h(long double val)
    {
        return val * kHourToSec;
    }
    
    /// @param val 小时值
    inline constexpr double operator ""_h(unsigned long long val)
    {
        return val * kHourToSec;
    }

    /// @param val 小时值
    inline constexpr double operator ""_hour(long double val)
    {
        return val * kHourToSec;
    }

    /// @param val 小时值
    inline constexpr double operator ""_hour(unsigned long long val)
    {
        return static_cast<double>(val) * kHourToSec;
    }
    
    /// @param val 天数值
    inline constexpr double operator ""_day(long double val)
    {
        return val * kDayToSec;
    }
    
    /// @param val 天数值
    inline constexpr double operator ""_day(unsigned long long val)
    {
        return val * kDayToSec;
    }
    
    /// @param val 毫秒值
    inline constexpr double operator ""_ms(long double val)
    {
        return val * kMilliSecToSec;
    }
    
    /// @param val 毫秒值
    inline constexpr double operator ""_ms(unsigned long long val)
    {
        return val * kMilliSecToSec;
    }
    
    /// @param val 微秒值
    inline constexpr double operator ""_us(long double val)
    {
        return val * kMicroSecToSec;
    }
    
    /// @param val 微秒值
    inline constexpr double operator ""_us(unsigned long long val)
    {
        return val * kMicroSecToSec;
    }
    
    /// @param val 纳秒值
    inline constexpr double operator ""_ns(long double val)
    {
        return val * kNanoSecToSec;
    }
    
    /// @param val 纳秒值
    inline constexpr double operator ""_ns(unsigned long long val)
    {
        return val * kNanoSecToSec;
    }
    
    /// @param val 皮秒值
    inline constexpr double operator ""_ps(long double val)
    {
        return val * kPicoSecToSec;
    }
    
    /// @param val 皮秒值
    inline constexpr double operator ""_ps(unsigned long long val)
    {
        return val * kPicoSecToSec;
    }

    // 质量单位 - 基于kg的单位制

    /// @param val 千克值
    inline constexpr double operator ""_kg(long double val)
    {
        return val;
    }

    /// @param val 千克值
    inline constexpr double operator ""_kg(unsigned long long val)
    {
        return static_cast<double>(val);
    }

    /// @param val 克值
    inline constexpr double operator ""_g(long double val)
    {
        return val * 1e-3;
    }

    /// @param val 克值
    inline constexpr double operator ""_g(unsigned long long val)
    {
        return val * 1e-3;
    }



    // 速度单位 - 基于m/s的单位制
    
    /// @param val 米/秒值
    inline constexpr double operator ""_m_s(long double val)
    {
        return val;
    }
    
    /// @param val 米/秒值
    inline constexpr double operator ""_m_s(unsigned long long val)
    {
        return static_cast<double>(val);
    }
    
    /// @param val 千米/秒值
    inline constexpr double operator ""_km_s(long double val)
    {
        return val * kKilometerToMeter;
    }
    
    /// @param val 千米/秒值
    inline constexpr double operator ""_km_s(unsigned long long val)
    {
        return val * kKilometerToMeter;
    }
    
    /// @param val 千米/小时值
    inline constexpr double operator ""_km_h(long double val)
    {
        return val * kKilometerToMeter / kHourToSec;
    }
    
    /// @param val 千米/小时值
    inline constexpr double operator ""_km_h(unsigned long long val)
    {
        return val * kKilometerToMeter / kHourToSec;
    }

    // 角速度单位 - 基于 rad/s的单位制
    
    /// @param val 弧度/秒值
    inline constexpr double operator ""_rad_s(long double val)
    {
        return val;
    }
    
    /// @param val 弧度/秒值
    inline constexpr double operator ""_rad_s(unsigned long long val)
    {
        return static_cast<double>(val);
    }
    
    /// @param val 角度/秒值
    inline constexpr double operator ""_deg_s(long double val)
    {
        return val * kDegToRad;
    }
    
    /// @param val 角度/秒值
    inline constexpr double operator ""_deg_s(unsigned long long val)
    {
        return val * kDegToRad;
    }

    // 面积单位 - 基于m2的单位制

    /// @param val 平方米值
    inline constexpr double operator ""_m2(long double val)
    {
        return val;
    }
    
    /// @param val 平方米值
    inline constexpr double operator ""_m2(unsigned long long val)
    {
        return static_cast<double>(val);
    }

    /// @param val 平方千米值
    inline constexpr double operator ""_km2(long double val)
    {
        return val * kKilometerToMeter * kKilometerToMeter;
    }
    
    /// @param val 平方千米值
    inline constexpr double operator ""_km2(unsigned long long val)
    {
        return static_cast<double>(val) * kKilometerToMeter * kKilometerToMeter;
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
    constexpr double day = 1_day;
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
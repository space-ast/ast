///
/// @file      Constants.hpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      15.11.2025
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
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。
 
#pragma once
 
#include "AstGlobal.h"
#include <limits>
 
AST_NAMESPACE_BEGIN


/*!
    @addtogroup Util
	@{
*/

// 时间系统常量

constexpr double kJ1900Epoch      = 2415020.0;        ///< J1900历元
constexpr double kJ1950Epoch      = 2433282.5;        ///< J1950历元
constexpr double kJ2000Epoch      = 2451545.0;        ///< J2000历元
constexpr double kJ2100Epoch      = 2488070.0;        ///< J2100历元

constexpr double kB1900Epoch      = 2415020.31352;    ///< B1900历元
// 原本定义为 2433282.4234591, 后改为SPICE中的定义 2433282.42345905
constexpr double kB1950Epoch      = 2433282.42345905; ///< B1950历元


constexpr double kTTMinusTAI      = 32.184;           ///< 地球时相对原子时的偏置[s]
constexpr double kGPSMinusTAI     = 19.0;             ///< GPS 时间相对原子时的偏置[s]
constexpr double kTTMinusTAIInDay = 32.184/86400;     ///< 地球时相对原子时的偏置[day]
constexpr double kMJDRefEpoch     = 2400000.5;        ///< 简约儒略日的参考历元


// pi相关常量

constexpr double kPI         =  3.141592653589793238462643383279502884197169399375105;      ///< PI
constexpr double kTwoPI      =  6.283185307179586476925286766558	                 ;	    ///< 2*PI
constexpr double kFourPI     =  12.566370614359172953850573533116	                 ;	    ///< 4*PI
constexpr double kHalfPI     =  1.5707963267948966192313216916395	                 ;	    ///< 0.5*kAsPI
constexpr double kThirdPI    =  1.047197551196597746154214461093	                 ;	    ///< PI/3
constexpr double kQuarterPI  =  0.78539816339744830961566084581975	                 ;	    ///< PI/4
constexpr double kSqrPI      =  9.869604401089358618834490999873	                 ;	    ///< PI*PI, PI squared


// 其他数学运算常量
constexpr double kNaN        =  std::numeric_limits<double>::quiet_NaN();                   ///< NaN 常量
constexpr double kInf        = +std::numeric_limits<double>::infinity();                    ///< Infinity 常量
constexpr double kNegInf     = -std::numeric_limits<double>::infinity();                    ///< Negative Infinity 常量



// 天文常量

constexpr double kAU = 1.49597870691e11;		                                            ///< 天文单位 (IAU 1976)[m]
constexpr double kLightSpeed = 2.99792458e8;			                                    ///< 光速 (IAU 1976 value) [m/s]		
constexpr double kGrav = 6.673e-11;				                                            ///< 万有引力常数 [m^3/kg*s^2] 


// 地球相关常量

constexpr double kEarthGrav = 3.986004418e14;		                                        ///< 地球引力常数 (WGS84) [m^3/s^2]
constexpr double kEarthGrav_EGM2008 = 3.986004415e14;                                       ///< 地球引力常数 (EGM2008) [m^3/s^2]
constexpr double kEarthRadius = 6378137.0;				                                    ///< 地球赤道半径 (WGS84)[m]
constexpr double kEarthMinRadius = 6356752.31424;		                                    ///< 地球最小(南北极)半径 (WGS84)[m]
constexpr double kEarthFlatFact = 3.35281066475E-3;		                                    ///< 地球扁率 (WGS84), f=1-b/a
constexpr double kEarthSiderealDay  = 86164.09054;                                          ///< 地球平恒星日 [s]
constexpr double kEarthSiderealYear = 365.25636;                                            ///< 地球平恒星年 [day]

// constexpr double kEarthAngVel = 7.2921151467e-5;		                                    ///< 地球标称平均角速度 (WGS84) [rad/s](kTwoPI/86164.0919)
constexpr double kEarthAngVel = 7.292115146706979e-5;		                                ///< 地球标称平均角速度 [rad/s](kTwoPI/86164.0919)

// @fixme 这里应该使用恒星日吧
// constexpr double kEarthMeanMotion = 1.9910643985790994404796035026544e-7;	            ///< 地球公转平均角速度 (2pi/365.2420897/86400) rad/sec
// constexpr double kEarthMeanMotion = kTwoPI / (kEarthSiderealYear * 86400);


// WGS84相关常量
constexpr double kEarthRadius_WGS84        = 6378137.0;
constexpr double kEarthMinRadius_WGS84     = 6356752.31424;
constexpr double kEarthFlatFact_WGS84      = 3.35281066475E-3; // (1/298.257223563)


// CGCS2000相关常量
constexpr double kEarthRadius_CGCS2000        = 6378137.0;
constexpr double kEarthMinRadius_CGCS2000     = 6356752.31414;
constexpr double kEarthFlatFact_CGCS2000      = 1/298.257222101;

// 潮汐相关常量
// 参考: IERS Conventions (2010), Technical Note 36 (TN36)
constexpr double kEarthK20LoveNumber = 0.30190;                  ///< 地球2阶位Love数 k20 — IERS 2010 TN36 第6章 第6.2.1节 表6.3 (p.83, 滞弹性地球名义值)


// 月球相关常量

constexpr double kMoonGrav = 4.9028029535968e+12;		         ///< 月球引力常数 [m^3/s^2]. （GLGM2）
constexpr double kMoonRadius = 1737400.0;				         ///< 月球赤道半径 [m]. (IAU2000);
constexpr double kMoonMinRadius = 1737400.0;			         ///< 月球最小半径(m)
constexpr double kMoonAngVel = 2.661699e-6;				         ///< 月球自转角速度 [rad/s]


// 木星相关常量

constexpr double kJupiterGrav = 1.26712000000000e+017;          ///< 木星引力常数
constexpr double kJupiterRadius = 7.14920000000000e+007;        ///< 木星赤道半径
constexpr double kJupiterMinRadius = 6.68540000000000e+007;     ///< 木星最小半径

// 火星相关常量

constexpr double kMarsGrav = 4.28283719012840e+013;	          ///< 火星引力常数(GMM2B)
constexpr double kMarsRadius = 3.39700000000000e+006;         ///< 火星赤道半径
constexpr double kMarsMinRadius = 3.37500000000000e+006;      ///< 火星最小半径


// 水星相关常量

constexpr double kMercuryGrav = 2.20320800000000e+013;        ///<  水星引力常数
constexpr double kMercuryRadius = 2.43970000000000e+006;      ///<  水星赤道半径
constexpr double kMercuryMinRadius = 2.43970000000000e+006;   ///<  水星最小半径


// 海王星相关常量 

constexpr double kNeptuneGrav = 6.87130000000000e+015;         ///< 海王星引力常数
constexpr double kNeptuneRadius = 2.52690000000000e+007;       ///< 海王星赤道半径
constexpr double kNeptuneMinRadius = 2.48000000000000e+007;    ///< 海王星最小半径


// 冥王星相关常量

constexpr double kPlutoGrav = 1.00907600000000e+012;           ///< 冥王星引力常数
constexpr double kPlutoRadius = 1.16200000000000e+006;         ///< 冥王星赤道半径
constexpr double kPlutoMinRadius = 1.16200000000000e+006;      ///< 冥王星最小半径


// 土星相关常量

constexpr double kSaturnGrav = 3.79340000000000e+016;          ///< 土星引力常数
constexpr double kSaturnRadius = 6.02680000000000e+007;        ///< 土星赤道半径
constexpr double kSaturnMinRadius = 5.43640000000000e+007;     ///< 土星最小半径


// 太阳相关常量

constexpr double kSunGrav = 1.327122E20;				        ///< 太阳引力常数 [m^3/s^2]
constexpr double kSunRadius = 695990000.0;				        ///< 太阳半径 [m]
constexpr double kSunMinRadius = 695990000.0;                   ///< 太阳最小半径
constexpr double kSunLuminosity = 3.839E26;                     ///< 太阳光度 [W] (Carrol & Ostlie, 2007) An Introduction to Modern Astrophysics
constexpr double kSolarConstant = 1367.0;                       ///< 太阳常数（1AU处总太阳辐照度）[W/m²]                                                                     
                                                                     
                                                                     
// 天王星相关常量                                                    
                                                                     
constexpr double kUranusGrav = 5.80320000000000e+015;          ///< 天王星引力常数
constexpr double kUranusRadius = 2.55590000000000e+007;        ///< 天王星半径
constexpr double kUranusMinRadius = 2.49730000000000e+007;     ///< 天王星最小半径
                                                                     

// 金星相关常量

constexpr double kVenusGrav = 3.24858800000000e+014;            ///< 金星引力常数
constexpr double kVenusRadius = 6.05190000000000e+006;          ///< 金星半径
constexpr double kVenusMinRadius = 6.05190000000000e+006;       ///< 金星最小半径



// 长度单位转换常数

constexpr double kMeterToKilometer = 1.0e-03;             ///< 米到千米
constexpr double kMeterToCentimeter = 1.0e+02;            ///< 米到厘米
constexpr double kMeterToMillimeter = 1.0e+03;            ///< 米到毫米
constexpr double kMeterToMicroMeter = 1.0e+06;            ///< 米到微米
constexpr double kMeterToNanometer = 1.0e+09;             ///< 米到纳米

constexpr double kKilometerToMeter = 1.0e03;              ///< 千米到米
constexpr double kCentimeterToMeter = 1.e-02;             ///< 厘米到米
constexpr double kDecimeterToMeter = 1.e-01;              ///< 分米到米
constexpr double kMillimeterToMeter = 1.e-03;             ///< 毫米到米
constexpr double kMicroMeterToMeter = 1.e-06;             ///< 微米到米
constexpr double kNanometerToMeter = 1.e-09;              ///< 纳米到米

constexpr double kInchToMeter = 0.0254;                   ///< 英寸到米
constexpr double kFootToMeter = 0.3048;                   ///< 英尺到米
constexpr double kYardToMeter = 0.9144;                   ///< 码到米
constexpr double kMileToMeter = 1609.344;                 ///< 英里到米

// 时间单位转换常数

constexpr double kSecToMin = 0.0166666666666666667;       ///< 秒到分钟
constexpr double kSecToHour = 0.000277777777777777778;    ///< 秒到小时
constexpr double kSecToDay = 0.0000115740740740740741;    ///< 秒到天

constexpr double kMinToSec = 60.0;                        ///< 分钟到秒
constexpr double kMinToHour = 0.0166666666666666667;      ///< 分钟到小时
constexpr double kMinToDay = 0.000694444444444444444;     ///< 分钟到天

constexpr double kHourToSec = 3600.0;                     ///< 小时到秒
constexpr double kHourToMin = 60.0;                       ///< 小时到分钟
constexpr double kHourToDay = 0.0416666666666666667;      ///< 小时到天

constexpr double kDayToSec = 8.64e04;                     ///< 天到秒
constexpr double kDayToMin = 1440.0;                      ///< 天到分钟
constexpr double kDayToHour = 24.0;                       ///< 天到小时

constexpr double kMilliSecToSec = 1.0e-03;                ///< 毫秒到秒
constexpr double kMicroSecToSec = 1.0e-06;                ///< 微秒到秒
constexpr double kNanoSecToSec = 1.0e-09;                 ///< 纳秒到秒
constexpr double kPicoSecToSec = 1.0e-12;                 ///< 皮秒(微微秒)到秒

constexpr double kSecToMilliSec = 1.0e+03;                ///< 秒到毫秒
constexpr double kSecToMicroSec = 1.0e+06;                ///< 秒到微秒
constexpr double kSecToNanoSec = 1.0e+09;                 ///< 秒到纳秒
constexpr double kSecToPicoSec = 1.0e+12;                 ///< 秒到皮秒

// 天数、秒数相关常量

constexpr double kDaysPerJulianCentury = 36525.0;          ///< 儒略世纪天数
constexpr double kDaysPerJulianYear = 365.25;              ///< 儒略年天数
constexpr double kSecondsPerDay   = 86400.0;               ///< 一天的秒数
constexpr double kSecondsPerHour = 3600.0;                 ///< 一小时的秒数
constexpr double kSecondsPerMinute = 60.0;                 ///< 一分钟的秒数
constexpr double kSecondsPerJulianYear = 31557600.0;       ///< 儒略年秒数(kDaysPerJulianYear * kSecondsPerDay)

// 质量单位转换常数

constexpr double kGramToKilogram = 1.0e-03;                ///< 克到千克
constexpr double kMilligramToKilogram = 1.0e-06;           ///< 毫克到千克
constexpr double kPoundToKilogram = 0.45359237;            ///< 磅到千克

// 体积单位转换常数

constexpr double kLiterToCubicMeter = 1.0e-03;             ///< 升到立方米

// 无量纲单位转换常数

constexpr double kPercentToUnit = 1.0e-02;                 ///< 百分比到单位

// 角度单位转换常数

constexpr double kDegToRad      = 0.017453292519943295769236907684886;	///< 角度到弧度 kPI/180.0
constexpr double kArcSecToRad   = 4.848136811095359935899141e-6;        ///< 弧秒到弧度 kDegToRad/3600.0 度分秒的秒
constexpr double kArcMinToRad   = 2.9088820866572159615394846141477e-4;	///< 弧分到弧度 kDegToRad/60.0 度分秒的分
constexpr double kTimeSecToRad  = 7.2722052166430399038487115353692e-5;	///< 时间秒到弧度，时分秒的秒
constexpr double kTimeMinToRad  = 0.0043633231299858239423092269212215;	///< 时间分到弧度，时分秒的分
constexpr double kTimeHourToRad = kDegToRad * 15.0;	                    ///< 时间时到弧度，时分秒的时
constexpr double kRevolutionToRad = kTwoPI;                             ///< 周期数到弧度 2.0 * kPI

constexpr double kRadToDeg      = 57.2957795130823208767;		        ///< 弧度到角度 180.0/kPI
constexpr double kRadToTimeHour = kRadToDeg / 15.0;	                    ///< 弧度到时间时，度分秒的时

// 磁感应强度(磁通密度)单位转换常数

constexpr double kNanoteslaToTesla = 1.0e-9;                          ///< 纳特斯拉到特斯拉
constexpr double kGaussToTesla     = 1.0e-4;                          ///< 高斯到特斯拉


// 温度单位转换常数
// 仿射式换算: SI = value * scale + offset（乘子用 kXToY，偏移量用 kXToYOffset）
constexpr double kCelsiusToKelvinOffset    = 273.15;                            ///< 摄氏度到开尔文偏移[K](SI = v*1 + 273.15)
constexpr double kFahrenheitToKelvinScale  = 5.0 / 9.0;                         ///< 华氏度到开尔文缩放（乘子）
constexpr double kFahrenheitToKelvinOffset = 459.67 * 5.0 / 9.0;                ///< 华氏度到开尔文偏移[K]
constexpr double kRankineToKelvin          = kFahrenheitToKelvinScale;          ///< 兰氏度到开尔文缩放（乘子）

// 分贝(dB)/贝尔(B)相关常数
// 对数式换算: SI = reference * 10^(value / factor)
constexpr double kDecibelPowerFactor = 10.0;                          ///< 功率型分贝系数(10·log10)
constexpr double kBelFactor          = 1.0;                           ///< 贝尔对数系数(1B = 10dB，即 log10 直接表示)
constexpr double kMilliwattToWatt    = 1e-3;                          ///< 毫瓦到瓦(dBm参考值)

/// @brief 获取真空中的光速
/// @details 这个函数接口是为了应对未来可能允许在软件运行时修改光速的情况
/// @return 光速
A_ALWAYS_INLINE constexpr double aLightSpeed() noexcept
{
    return kLightSpeed;
}


/*! @} */


AST_NAMESPACE_END

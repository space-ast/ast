///
/// @file      Constants.h
/// @brief     
/// @details   ~
/// @author    jinke18
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
 
AST_NAMESPACE_BEGIN
 

// 时间系统常量

constexpr const double kJ2000Epoch      = 2451545.0;        ///< J2000历元
constexpr const double kB1950Epoch      = 2433282.4234591;  ///< B1950历元
constexpr const double kJ1950Epoch      = 2433282.5;        ///< J1950历元
constexpr const double kTTMinusTAI      = 32.184;           ///< 地球时相对原子时的偏置[s]
constexpr const double kGPSMinusTAI     = 19.0;             ///< GPS 时间相对原子时的偏置[s]
constexpr const double kTTMinusTAIInDay = 32.184/86400;     ///< 地球时相对原子时的偏置[day]
constexpr const double kMJDRefEpoch     = 2400000.5;        ///< 简约儒略日的参考历元


// pi相关常量

constexpr const double kPI         =  3.141592653589793238462643383279502884197169399375105;    ///< PI
constexpr const double kTwoPI      =  6.283185307179586476925286766558	                 ;	    ///< 2*PI
constexpr const double kFourPI     =  12.566370614359172953850573533116	                 ;	    ///< 4*PI
constexpr const double kHalfPI     =  1.5707963267948966192313216916395	                 ;	    ///< 0.5*kAsPI
constexpr const double kThirdPI    =  1.047197551196597746154214461093	                 ;	    ///< PI/3
constexpr const double kQuarterPI  =  0.78539816339744830961566084581975	             ;	    ///< PI/4
constexpr const double kSqrPI      =  9.869604401089358618834490999873	                 ;	    ///< PI*PI, PI squared


// 天文常量

constexpr const double kAU = 1.49597870691e11;		                                            ///< 天文单位 (IAU 1976)[m]
constexpr const double kLightSpeed = 2.99792458e8;			                                    ///< 光速 (IAU 1976 value) [m/s]		
constexpr const double kGrav = 6.673e-11;				                                        ///< 万有引力常数 [m^3/kg*s^2] 


// 地球相关常量

constexpr const double kEarthGrav = 3.986004418e14;		                                        ///< 地球引力常数 (WGS84) [m^3/s^2]
constexpr const double kEarthRadius = 6378137.0;				                                ///< 地球赤道半径 (WGS84)[m]
constexpr const double kEarthMinRadius = 6.35675231424E6;		                                ///< 地球最小(南北极)半径 (WGS84)[m]
constexpr const double kEarthFlatFact = 3.35281066475E-3;		                                ///< 地球扁率 (WGS84), f=1-b/a
constexpr const double kEarthAngVel = 7.2921151467e-5;		                                    ///< 地球标称平均角速度 (WGS84) [rad/s](kTwoPI/86164.0919)
constexpr const double kEarthMeanMotion = 1.9910643985790994404796035026544e-7;	                ///< 地球公转平均角速度 (2pi/365.2420897/86400) rad/sec



// 月球相关常量

constexpr const double kMoonGrav = 4.9028029535968e+12;		         ///< 月球引力常数 [m^3/s^2]. （GLGM2）
constexpr const double kMoonRadius = 1737400.0;				         ///< 月球赤道半径 [m]. (IAU2000);
constexpr const double kMoonMinRadius = 1737400.0;			         ///< 月球最小半径(m)


// 木星相关常量

constexpr const double kJupiterGrav = 1.26712000000000e+017;          ///< 木星引力常数
constexpr const double kJupiterRadius = 7.14920000000000e+007;        ///< 木星赤道半径
constexpr const double kJupiterMinRadius = 6.68540000000000e+007;     ///< 木星最小半径

// 火星相关常量

constexpr const double kMarsGrav = 4.28283719012840e+013;	        ///< 火星引力常数(GMM2B)
constexpr const double kMarsRadius = 3.39700000000000e+006;         ///< 火星赤道半径
constexpr const double kMarsMinRadius = 3.37500000000000e+006;      ///< 火星最小半径


// 水星相关常量

constexpr const double kMercuryGrav = 2.20320800000000e+013;        ///<  水星引力常数
constexpr const double kMercuryRadius = 2.43970000000000e+006;      ///<  水星赤道半径
constexpr const double kMercuryMinRadius = 2.43970000000000e+006;   ///<  水星最小半径


// 海王星相关常量 

constexpr const double kNeptuneGrav = 6.87130000000000e+015;         ///< 海王星引力常数
constexpr const double kNeptuneRadius = 2.52690000000000e+007;       ///< 海王星赤道半径
constexpr const double kNeptuneMinRadius = 2.48000000000000e+007;    ///< 海王星最小半径


// 冥王星相关常量

constexpr const double kPlutoGrav = 1.00907600000000e+012;           ///< 冥王星引力常数
constexpr const double kPlutoRadius = 1.16200000000000e+006;         ///< 冥王星赤道半径
constexpr const double kPlutoMinRadius = 1.16200000000000e+006;      ///< 冥王星最小半径


// 土星相关常量

constexpr const double kSaturnGrav = 3.79340000000000e+016;          ///< 土星引力常数
constexpr const double kSaturnRadius = 6.02680000000000e+007;        ///< 土星赤道半径
constexpr const double kSaturnMinRadius = 5.43640000000000e+007;     ///< 土星最小半径


// 太阳相关常量

constexpr const double kSunGrav = 1.327122E20;				         ///< 太阳引力常数 [m^3/s^2]
constexpr const double kSunRadius = 695990000.0;				     ///< 太阳半径 [m]
constexpr const double kSunMinRadius = 695990000.0;                  ///< 太阳最小半径
                                                                     
                                                                     
                                                                     
// 天王星相关常量                                                    
                                                                     
constexpr const double kUranusGrav = 5.80320000000000e+015;          ///< 天王星引力常数
constexpr const double kUranusRadius = 2.55590000000000e+007;        ///< 天王星半径
constexpr const double kUranusMinRadius = 2.49730000000000e+007;     ///< 天王星最小半径
                                                                     

// 金星相关常量

constexpr const double kVenusGrav = 3.24858800000000e+014;            ///< 金星引力常数
constexpr const double kVenusRadius = 6.05190000000000e+006;          ///< 金星半径
constexpr const double kVenusMinRadius = 6.05190000000000e+006;       ///< 金星最小半径



// 长度单位转换常数

constexpr const double kMeterToKilometer = 1.0e-03;             ///< 米到千米
constexpr const double kMeterToCentimeter = 1.0e+02;            ///< 米到厘米
constexpr const double kMeterToMillimeter = 1.0e+03;            ///< 米到毫米
constexpr const double kMeterToMicroMeter = 1.0e+06;            ///< 米到微米
constexpr const double kMeterToNanometer = 1.0e+09;             ///< 米到纳米

constexpr const double kKilometerToMeter = 1.0e03;              ///< 千米到米
constexpr const double kCentimeterToMeter = 1.e-02;             ///< 厘米到米
constexpr const double kMillimeterToMeter = 1.e-03;             ///< 毫米到米
constexpr const double kMicroMeterToMeter = 1.e-06;             ///< 微米到米
constexpr const double kNanometerToMeter = 1.e-09;              ///< 纳米到米

// 时间单位转换常数

constexpr const double kSecToMin = 0.0166666666666666667;       ///< 秒到分钟
constexpr const double kSecToHour = 0.000277777777777777778;    ///< 秒到小时
constexpr const double kSecToDay = 0.0000115740740740740741;    ///< 秒到天

constexpr const double kMinToSec = 60.0;                        ///< 分钟到秒
constexpr const double kMinToHour = 0.0166666666666666667;      ///< 分钟到小时
constexpr const double kMinToDay = 0.000694444444444444444;     ///< 分钟到天

constexpr const double kHourToSec = 3600.0;                     ///< 小时到秒
constexpr const double kHourToMin = 60.0;                       ///< 小时到分钟
constexpr const double kHourToDay = 0.0416666666666666667;      ///< 小时到天

constexpr const double kDayToSec = 8.64e04;                     ///< 天到秒
constexpr const double kDayToMin = 1440.0;                      ///< 天到分钟
constexpr const double kDayToHour = 24.0;                       ///< 天到小时

constexpr const double kMilliSecToSec = 1.0e-03;                ///< 毫秒到秒
constexpr const double kMicroSecToSec = 1.0e-06;                ///< 微秒到秒
constexpr const double kNanoSecToSec = 1.0e-09;                 ///< 纳秒到秒
constexpr const double kPicoSecToSec = 1.0e-12;                 ///< 皮秒(微微秒)到秒

constexpr const double kSecToMilliSec = 1.0e+03;                ///< 秒到毫秒
constexpr const double kSecToMicroSec = 1.0e+06;                ///< 秒到微秒
constexpr const double kSecToNanoSec = 1.0e+09;                 ///< 秒到纳秒
constexpr const double kSecToPicoSec = 1.0e+12;                 ///< 秒到皮秒

// 角度单位转换常数

constexpr const double kRadToDeg = 57.2957795130823208767;		                ///< 弧度到角度 180.0/kPI

constexpr const double kDegToRad = 0.017453292519943295769236907684886;	        ///< 角度到弧度 kPI/180.0


AST_NAMESPACE_END

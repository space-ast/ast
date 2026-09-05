///
/// @file      Unit.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
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
#include "Dimension.hpp"
#include "UnitRep.hpp"
#include "AstUtil/Object.hpp"
#include "AstUtil/SharedPtr.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/Constants.hpp"
#include <cmath>
#include <string>
#include <vector>
#include <memory>

AST_NAMESPACE_BEGIN


/*!
    @addtogroup Quantity
    @{
*/



class Unit;     ///< 单位
class Quantity; ///< 数量值
class UnitRep;  ///< 单位表示



namespace units
{
    extern AST_UTIL_API Unit none;      ///< 无单位

    extern AST_UTIL_API Unit mm;        ///< 毫米
    extern AST_UTIL_API Unit cm;        ///< 厘米
    extern AST_UTIL_API Unit dm;        ///< 分米
    extern AST_UTIL_API Unit m;         ///< 米
    extern AST_UTIL_API Unit km;        ///< 千米

    extern AST_UTIL_API Unit in;        ///< 英寸
    extern AST_UTIL_API Unit ft;        ///< 英尺
    extern AST_UTIL_API Unit yd;        ///< 码
    extern AST_UTIL_API Unit mi;        ///< 英里



    extern AST_UTIL_API Unit sec;       ///< 秒
    extern AST_UTIL_API Unit s;         ///< 秒
    extern AST_UTIL_API Unit ms;        ///< 毫秒
    extern AST_UTIL_API Unit minute;    ///< 分钟，不要命名为min，容易与std::min冲突
    extern AST_UTIL_API Unit hour;      ///< 小时
    extern AST_UTIL_API Unit h;         ///< 小时
    extern AST_UTIL_API Unit day;       ///< 天

    extern AST_UTIL_API Unit kg;        ///< 千克
    extern AST_UTIL_API Unit g;         ///< 克
    extern AST_UTIL_API Unit mg;        ///< 毫克
    extern AST_UTIL_API Unit lb;        ///< 磅

    extern AST_UTIL_API Unit N;         ///< 牛顿

    extern AST_UTIL_API Unit deg;       ///< 度
    extern AST_UTIL_API Unit rad;       ///< 弧度
    extern AST_UTIL_API Unit arcsec;    ///< 弧秒

    extern AST_UTIL_API Unit A;         ///< 安培

    extern AST_UTIL_API Unit m2;        ///< 平方米

    extern AST_UTIL_API Unit m3;        ///< 立方米
    extern AST_UTIL_API Unit L;         ///< 升
    
    extern AST_UTIL_API Unit Pa;        ///< 帕斯卡

    extern AST_UTIL_API Unit T;         ///< 特斯拉
    extern AST_UTIL_API Unit nT;        ///< 纳特斯拉
    extern AST_UTIL_API Unit Gs;        ///< 高斯

    extern AST_UTIL_API Unit K;         ///< 开氏温度

    extern AST_UTIL_API Unit degC;      ///< 摄氏度
    extern AST_UTIL_API Unit degF;      ///< 华氏度
    extern AST_UTIL_API Unit degR;      ///< 兰氏度

    extern AST_UTIL_API Unit dB;        ///< 分贝(无量纲)
    extern AST_UTIL_API Unit dBm;       ///< 分贝毫瓦
    extern AST_UTIL_API Unit dBW;       ///< 分贝瓦

    extern AST_UTIL_API Unit B;         ///< 贝尔(无量纲)
};



/// @brief 单位乘法
AST_UTIL_API Unit aUnitMultiply(const Unit& unit1, const Unit& unit2);

/// @brief 单位乘法
AST_UTIL_API Unit aUnitMultiply(const Unit& unit1, const Unit& unit2, StringView newname);

/// @brief 单位除法
AST_UTIL_API Unit aUnitDivide(const Unit& unit1, const Unit& unit2);

/// @brief 单位除法
AST_UTIL_API Unit aUnitDivide(const Unit& unit1, const Unit& unit2, StringView newname);

/// @brief 单位幂
AST_UTIL_API Unit aUnitPower(const Unit& unit, int exponent);

/// @brief 单位幂
AST_UTIL_API Unit aUnitPower(const Unit& unit, int exponent, StringView newname);

/// @brief 单位倒数
AST_UTIL_API Unit aUnitInvert(const Unit& unit);

/// @brief 单位倒数
AST_UTIL_API Unit aUnitInvert(const Unit& unit, StringView newname);

/// @brief 单位分解
/// @param[in] unit 分解前的单位
/// @param[out] newUnit 分解后的单位
/// @param[out] scale 单位缩放因子
AST_UTIL_API void aUnitFactorize(const Unit& unit, Unit& newUnit, double& scale);

/// @brief 单位分解
/// @param[in,out] unit 约简前后的单位
/// @param[out] scale 单位缩放因子
AST_UTIL_API void aUnitFactorize(Unit& unit, double& scale);




/// @brief 单位
class Unit
{
public:
    #ifdef AST_BUILD_LIB_UTIL
    friend Unit unit_multiply(const Unit& unit1, const Unit& unit2);
    friend Unit unit_divide(const Unit& unit1, const Unit& unit2);
    friend Unit unit_power(const Unit& unit, int exponent);
    friend void unit_setname(Unit& unit, StringView name);
    #endif
public: // 便捷工厂(只能用于创建基本单位，不建议用于直接创建复合单位，否则所创建复合单位会缺少子单位信息)

    /// @brief 创建乘法型单位
    /// @param name 名称
    /// @param dim 量纲
    /// @param scale 缩放因子
    AST_UTIL_API
    static Unit MakeScale(StringView name, Dimension dim, double scale);

    /// @brief 创建仿射型单位
    /// @param name 名称
    /// @param dim 量纲
    /// @param scale 缩放因子
    /// @param offset 偏移量
    AST_UTIL_API
    static Unit MakeAffine(StringView name, Dimension dim, double scale, double offset);

    /// @brief 创建对数型单位
    /// @param name 名称
    /// @param dim 量纲
    /// @param reference 对数参考值
    /// @param factor 对数因子
    AST_UTIL_API
    static Unit MakeLogarithmic(StringView name, Dimension dim, double reference, double factor);

public:
    /// @brief 无效单位
    static Unit NaN()
    {
        return Unit("", 0.0, EDimension::eUnit);
    }

    /// @brief 无单位
    static Unit None()
    {
        return Unit("", 1.0, EDimension::eUnit);
    }    

    /// @brief 无量纲单位
    static Unit One()
    {
        // return Unit(aText("×1"), 1.0, EDimension::eUnit);
        return Unit(aText("\u00D71"), 1.0, EDimension::eUnit);
    }

    /// @brief 新建无量纲单位
    /// @param scale 缩放因子
    /// @return 单位表示
    AST_UTIL_API 
    static Unit Scale(double scale);

    /// @brief 百分比单位
    static Unit Percent()
    {
        return Unit("%", kPercentToUnit, EDimension::eUnit);
    }


    /// @brief 秒单位
    static Unit Second()
    {
        return Unit("s", 1.0, EDimension::eTime);
    }
    /// @brief 分钟单位
    static Unit Minute()
    {
        return Unit("min", kSecondsPerMinute, EDimension::eTime);
    }
    /// @brief 小时单位
    static Unit Hour()
    {
        return Unit("h", kSecondsPerHour, EDimension::eTime);
    }
    /// @brief 天单位
    static Unit Day()
    {
        return Unit("day", kSecondsPerDay, EDimension::eTime);
    }
    /// @brief 毫秒单位
    static Unit MilliSecond()
    {
        return Unit("ms", kMilliSecToSec, EDimension::eTime);
    }



    /// @brief 米单位
    static Unit Meter()
    {
        return Unit("m", 1.0, EDimension::eLength);
    }
    /// @brief 米单位
    static Unit Kilometer()
    {
        return Unit("km", kKilometerToMeter, EDimension::eLength);
    }
    /// @brief 厘米单位
    static Unit Centimeter()
    {
        return Unit("cm", kCentimeterToMeter, EDimension::eLength);
    }
    /// @brief 分米单位
    static Unit Decimeter()
    {
        return Unit("dm", kDecimeterToMeter, EDimension::eLength);
    }

    /// @brief 毫米单位
    static Unit Millimeter()
    {
        return Unit("mm", kMillimeterToMeter, EDimension::eLength);
    }

    
    /// @brief 英寸单位
    static Unit Inch()
    {
        return Unit("in", kInchToMeter, EDimension::eLength);
    }

    /// @brief 英尺单位
    static Unit Foot()
    {
        return Unit("ft", kFootToMeter, EDimension::eLength);
    }

    /// @brief  码单位
    static Unit Yard()
    {
        return Unit("yd", kYardToMeter, EDimension::eLength);
    }

    /// @brief 英里单位
    static Unit Mile()
    {
        return Unit("mi", kMileToMeter, EDimension::eLength);
    }

    /// @brief 千克单位
    static Unit Kilogram()
    {
        return Unit("kg", 1.0, EDimension::eMass);
    }
    /// @brief 克单位
    static Unit Gram()
    {
        return Unit("g", kGramToKilogram, EDimension::eMass);
    }

    /// @brief 毫克单位
    static Unit Milligram()
    {
        return Unit("mg", kMilligramToKilogram, EDimension::eMass);
    }

    /// @brief 磅单位
    static Unit Pound()
    {
        return Unit("lb", kPoundToKilogram, EDimension::eMass);
    }

    /// @brief 弧度单位
    static Unit Radian()
    {
        return Unit("rad", 1.0, EDimension::eAngle);
    }

    /// @brief 度单位
    static Unit Degree()
    {
        // return Unit("°", 0.017453292519943295, EDimension::eAngle);
        return Unit(aText("\u00B0"), kDegToRad, EDimension::eAngle);
    }

    /// @brief 弧秒单位
    static Unit ArcSecond()
    {
        return Unit(aText("\u2033"), kArcSecToRad, EDimension::eAngle);
    }

    /// @brief 牛顿单位
    static Unit Newton()
    {
        return Unit("N", 1.0, EDimension::eForce);
    }

    /// @brief 安培单位
    static Unit Ampere()
    {
        return Unit("A", 1.0, EDimension::eCurrent);
    }
    /// @brief 开尔文单位
    static Unit Kelvin()
    {
        return Unit("K", 1.0, EDimension::eTemperature);
    }

    /// @brief 摄氏度单位(仿射: SI = v*1 + 273.15)
    static Unit Celsius()
    {
        return MakeAffine(aText("°C"), EDimension::eTemperature, 1.0, kCelsiusToKelvinOffset);
    }
    /// @brief 华氏度单位(仿射: SI = (v+459.67)*5/9)
    static Unit Fahrenheit()
    {
        return MakeAffine(aText("°F"), EDimension::eTemperature, kFahrenheitToKelvinScale, kFahrenheitToKelvinOffset);
    }
    /// @brief 兰氏度单位(仿射: SI = v*5/9)
    static Unit Rankine()
    {
        return MakeScale(aText("°R"), EDimension::eTemperature, kRankineToKelvin);
    }

    /// @brief 分贝单位(对数: SI = 10^(v/10)，无量纲)
    static Unit Decibel()
    {
        return MakeLogarithmic(aText("dB"), EDimension::eUnit, 1.0, kDecibelPowerFactor);
    }
    /// @brief 分贝毫瓦单位(对数: 参考1mW，功率量纲)
    static Unit DecibelMilliwatt()
    {
        // @todo 修复构造单位后缺少子单位信息
        // dBm 为导出量纲(功率)，MakeLogarithmic 的断言不满足，直接构造对数单位表示
        return Unit(std::make_shared<LogarithmicUnitRep>(aText("dBm"), EDimension::ePower, kMilliwattToWatt, kDecibelPowerFactor));
    }
    /// @brief 分贝瓦单位(对数: 参考1W，功率量纲)
    static Unit DecibelWatt()
    {
        // @todo 修复构造单位后缺少子单位信息
        // dBW 为导出量纲(功率)，直接构造对数单位表示
        return Unit(std::make_shared<LogarithmicUnitRep>(aText("dBW"), EDimension::ePower, 1.0, kDecibelPowerFactor));
    }

    /// @brief 贝尔单位(对数: SI = 10^v，无量纲；1B = 10dB)
    static Unit Bel()
    {
        return MakeLogarithmic(aText("B"), EDimension::eUnit, 1.0, kBelFactor);
    }

    /// @brief  mole单位
    // static Unit Mole()
    // {
    //     return Unit("mol", 1.0, EDimension::eAmount);
    // }

    // 下面是组合单位

    /// @brief 厘米每秒单位
    static Unit CentimeterPerSecond(){
        return Centimeter() / Second();
    }
    /// @brief 千米每秒单位
    static Unit KilometerPerHour()
    {
        return Kilometer() / Hour();
    }
    /// @brief 米每分钟单位
    static Unit MeterPerMinute()
    {
        return Meter() / Minute();
    }
    /// @brief 平方米单位
    static Unit SquareMeter()
    {
        return Meter() * Meter();
    }
    /// @brief 平方公里单位
    static Unit SquareKilometer()
    {
        return Kilometer() * Kilometer();
    }
    /// @brief 平方英尺单位
    static Unit SquareFoot()
    {
        return Foot() * Foot();
    }

    /// @brief 平方码单位
    static Unit SquareYard()
    {
        return Yard() * Yard();
    }

    /// @brief 立方米单位
    static Unit CubicMeter()
    {
        return Meter() * Meter() * Meter();
    }
    /// @brief 升单位
    static Unit Liter()
    {
        return Unit("L", kLiterToCubicMeter, EDimension::eVolume);
    }

    /// @brief 帕斯卡单位（压力量纲）
    static Unit Pascal()
    {
        return Unit("Pa", 1.0, EDimension::ePressure);
    }

    /// @brief 特斯拉单位（磁感应强度/磁通密度量纲）
    static Unit Tesla()
    {
        return Unit("T", 1.0, EDimension::eMagneticFluxDensity);
    }
    /// @brief 纳特斯拉单位
    static Unit NanoTesla()
    {
        return Unit("nT", kNanoteslaToTesla, EDimension::eMagneticFluxDensity);
    }
    /// @brief 高斯单位（1 Gs = 1e-4 T）
    static Unit Gauss()
    {
        return Unit("Gs", kGaussToTesla, EDimension::eMagneticFluxDensity);
    }

public:
    /// @brief 默认无单位
    Unit(): Unit{Unit::None()}{}

    /// @brief 根据单位名称创建单位
    AST_UTIL_API
    Unit(StringView name);

    /// @brief 新建单位
    Unit(StringView name, double scale, Dimension dimension)
        : rep_{std::make_shared<ScaleUnitRep>(name, dimension, scale)} 
    {}

protected:
    Unit(double scale, Dimension dimension, const SubUnitListConst& subunits)
        : rep_{std::make_shared<ScaleUnitRep>("", dimension, (subunits), scale)} 
    {}

    /// @param rep 单位表示
    Unit(UnitRepHandle rep) : rep_(rep) {}

public:
    /// @brief 从国际单位制转换为此单位对应的值
    /// @param value 国际单位制下的值
    /// @return 外部值
    double fromSI(double value) const { return rep_->fromSI(value); }

    /// @brief 从此单位对应的值转换为国际单位制下的值
    /// @param value 此单位对应的值
    /// @return 国际单位制下的值
    double toSI(double value) const { return rep_->toSI(value); }

    /// @brief 转换为指定单位下的值
    /// @param value 此单位对应的值
    /// @param unit 指定单位
    /// @return 指定单位下的值
    double convertTo(double value, const Unit& unit) const { return unit.fromSI(toSI(value)); }

    /// @brief 从指定单位下的值转换为此单位对应的值
    /// @param value 指定单位下的值
    /// @param unit 指定单位
    /// @return 此单位对应的值
    double convertFrom(double value, const Unit& unit) const { return fromSI(unit.toSI(value)); }

    /// @brief 获取单位缩放因子
    /// @return 单位缩放因子
    double getScale() const { return rep_->scale(); }

    /// @brief 获取单位量纲
    /// @return 单位量纲
    Dimension dimension() const { return rep_->dimension_; }

    /// @brief 获取单位名称
    /// @return 单位名称
    const std::string& name() const { return rep_->name_; }

    /// @brief 获取单位类型
    /// @return 单位类型
    EUnitKind kind() const { return rep_->kind_; }

    /// @brief 是否有效单位
    /// @return 是否有效单位
    bool isValid() const { return rep_->valid(); }

public: // operators

    /// @brief 单位克隆
    /// @return 新单位(独立副本，与源单位共享底层子单位但顶层表示可独立修改)
    Unit clone() const { return Unit(rep_->clone()); }

    /// @brief 单位倒数
    Unit invert() const { return aUnitInvert(*this); }

    /// @brief 单位乘法
    /// @param newname 新单位名称
    /// @param other 其他单位
    /// @return 新单位
    Unit multiply(StringView newname, const Unit& other) const
    {
        return aUnitMultiply(*this, other, newname);
    }

    /// @brief 单位除法
    /// @param newname 新单位名称
    /// @param other 其他单位
    /// @return 新单位
    Unit divide(StringView newname, const Unit& other) const
    {
        return aUnitDivide(*this, other, newname);
    }

    /// @brief 单位幂运算
    /// @param newname 新单位名称
    /// @param exponent 指数
    /// @return 新单位
    Unit pow(StringView newname, int exponent) const
    {
        return aUnitPower(*this, exponent, newname);
    }

    /// @brief 单位幂运算
    /// @param exponent 指数
    /// @return 新单位
    Unit pow(int exponent) const
    {
        return aUnitPower(*this, exponent);
    }

    /// @brief 单位相等运算符
    /// @param other 其他单位
    /// @return 是否相等
    bool operator==(const Unit& other) const
    {
        if(rep_ == other.rep_)
            return true;
        return rep_->operator==(*other.rep_);    
    }

    /// @brief 单位不相等运算符
    /// @param other 其他单位
    /// @return 是否不相等
    bool operator!=(const Unit& other) const
    {
        return !(*this == other);
    }

    /// @brief 单位乘法运算符
    /// @param other 其他单位
    /// @return 新单位
    // @todo 应禁止 affine（如 °C）与 logarithmic（如 dBm）单位的复合算术（乘/除/幂），这些单位不能按线性比例运算。
    Unit operator*(const Unit& other) const
    {
        return aUnitMultiply(*this, other);
    }

    /// @brief 单位除法运算符
    /// @param other 其他单位
    /// @return 新单位
    Unit operator/(const Unit& other) const
    {
        return aUnitDivide(*this, other);
    }
    /// @brief 单位乘法赋值运算符
    /// @param other 其他单位
    /// @return 新单位
    Unit& operator*=(const Unit& other)
    {
        *this = *this * other;
        return *this;
    }
    /// @brief 单位除法赋值运算符
    /// @param other 其他单位
    /// @return 新单位
    Unit& operator/=(const Unit& other)
    {
        *this = *this / other;
        return *this;
    }


public:
    UnitRepHandle rep_{};
};


/*! @} */


AST_NAMESPACE_END

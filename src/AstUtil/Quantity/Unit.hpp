///
/// @file      Unit.hpp
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
#include "Dimension.hpp"
#include "AstUtil/Object.hpp"
#include "AstUtil/SharedPtr.hpp"
#include "AstUtil/StringView.hpp"
#include <cmath>
#include <string>
#include <vector>
#include <memory>

AST_NAMESPACE_BEGIN

class Unit;
class Quantity;


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
    friend Unit unit_multiply(const Unit& unit1, const Unit& unit2);
    friend Unit unit_divide(const Unit& unit1, const Unit& unit2);
    friend Unit unit_power(const Unit& unit, int exponent);
    friend void unit_setname(Unit& unit, StringView name);

    class UnitRep;                                                      ///< 单位表示
    using UnitRepHandle = std::shared_ptr<UnitRep>;                     ///< 单位表示句柄
    using UnitRepPair = std::pair<UnitRepHandle, int>;                  ///< 单位表示子项
    using SubUnitList = std::vector<UnitRepPair>;                       ///< 单位表示子项列表

    using UnitRepHandleConst = std::shared_ptr<const UnitRep>;          ///< 单位表示句柄（常量）
    using UnitRepPairConst = std::pair<UnitRepHandleConst, int>;        ///< 单位表示子项（常量）
    using SubUnitListConst = std::vector<UnitRepPairConst>;             ///< 单位表示子项列表（常量）

    /// @brief 单位表示
    /// @warning 单位表示结构体为内部使用，不建议外部直接使用。
    class UnitRep{
    public:
        UnitRep(StringView name, double scale, Dimension dimension, const SubUnitListConst& subunits)
            : name_{std::string(name)}
            , scale_{scale}
            , dimension_{dimension}
            , subUnits_{subunits}
        {}
        std::string          name_;          ///< 名称
        double               scale_{1.0};    ///< 单位缩放因子
        Dimension            dimension_;     ///< 量纲
        SubUnitListConst     subUnits_;      ///< 子单位列表
    };
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
        return Unit(aText("×1"), 1.0, EDimension::eUnit);
    }

    /// @brief 新建无量纲单位
    /// @param scale 缩放因子
    /// @return 单位表示
    AST_UTIL_API 
    static Unit Scale(double scale);

    /// @brief 百分比单位
    static Unit Percent()
    {
        return Unit("%", 0.01, EDimension::eUnit);
    }


    /// @brief 秒单位
    static Unit Second()
    {
        return Unit("s", 1.0, EDimension::eTime);
    }
    /// @brief 分钟单位
    static Unit Minute()
    {
        return Unit("min", 60.0, EDimension::eTime);
    }
    /// @brief 小时单位
    static Unit Hour()
    {
        return Unit("h", 3600.0, EDimension::eTime);
    }
    /// @brief 天单位
    static Unit Day()
    {
        return Unit("day", 86400.0, EDimension::eTime);
    }


    /// @brief 米单位
    static Unit Meter()
    {
        return Unit("m", 1.0, EDimension::eLength);
    }
    /// @brief 米单位
    static Unit Kilometer()
    {
        return Unit("km", 1000.0, EDimension::eLength);
    }
    /// @brief 厘米单位
    static Unit Centimeter()
    {
        return Unit("cm", 0.01, EDimension::eLength);
    }
    /// @brief 分米单位
    static Unit Decimeter()
    {
        return Unit("dm", 0.1, EDimension::eLength);
    }

    /// @brief 毫米单位
    static Unit Millimeter()
    {
        return Unit("mm", 0.001, EDimension::eLength);
    }

    
    /// @brief 英寸单位
    static Unit Inch()
    {
        return Unit("in", 0.0254, EDimension::eLength);
    }

    /// @brief 英尺单位
    static Unit Foot()
    {
        return Unit("ft", 0.3048, EDimension::eLength);
    }

    /// @brief  码单位
    static Unit Yard()
    {
        return Unit("yd", 0.9144, EDimension::eLength);
    }

    /// @brief 英里单位
    static Unit Mile()
    {
        return Unit("mi", 1609.344, EDimension::eLength);
    }

    /// @brief 千克单位
    static Unit Kilogram()
    {
        return Unit("kg", 1.0, EDimension::eMass);
    }
    /// @brief 克单位
    static Unit Gram()
    {
        return Unit("g", 0.001, EDimension::eMass);
    }

    /// @brief 毫克单位
    static Unit Milligram()
    {
        return Unit("mg", 0.000001, EDimension::eMass);
    }

    /// @brief 磅单位
    static Unit Pound()
    {
        return Unit("lb", 0.45359237, EDimension::eMass);
    }

    /// @brief 弧度单位
    static Unit Radian()
    {
        return Unit("rad", 1.0, EDimension::eAngle);
    }

    /// @brief 度单位
    static Unit Degree()
    {
        return Unit("°", 0.017453292519943295, EDimension::eAngle);
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
    /// @brief  mole单位
    static Unit Mole()
    {
        return Unit("mol", 1.0, EDimension::eAmount);
    }

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
        return Unit("L", 0.001, EDimension::eVolume);
    }

public:
    /// @brief 默认无单位
    Unit(): Unit{None()}{}

    /// @brief 新建单位
    Unit(StringView name, double scale, Dimension dimension)
        : rep_{std::make_shared<UnitRep>(name, scale, dimension, SubUnitListConst{})} 
    {}

protected:
    Unit(double scale, Dimension dimension, const SubUnitListConst& subunits)
        : rep_{std::make_shared<UnitRep>("", scale, dimension, (subunits))} 
    {}

    /// @param rep 单位表示
    Unit(UnitRepHandle rep) : rep_(rep) {}

public:
    /// @brief 从国际单位制转换为该单位对应的值
    /// @param value 国际单位制下的值
    /// @return 外部值
    double fromSI(double value) const { return value / getScale(); }


    /// @brief 从该单位对应的值转换为国际单位制下的值
    /// @param value 该单位对应的值
    /// @return 国际单位制下的值
    double toSI(double value) const { return value * getScale(); }

    /// @brief 从内部运行时单位对应的值转换为该单位对应的值
    /// @param value 内部运行时单位对应的值
    /// @return 该单位对应的值
    double fromInternal(double value) const { return fromSI(value); }

    /// @brief 从该单位对应的值转换为内部运行时单位对应值
    /// @param value 该单位对应的值
    /// @return 内部运行时单位对应的值
    double toInternal(double value) const { return toSI(value); }

    /// @brief 获取单位缩放因子
    /// @return 单位缩放因子
    double getScale() const { return rep_->scale_; }

    /// @brief 获取单位量纲
    /// @return 单位量纲
    Dimension dimension() const { return rep_->dimension_; }

    /// @brief 获取单位名称
    /// @return 单位名称
    const std::string& name() const { return rep_->name_; }


    /// @brief 是否有效单位
    /// @return 是否有效单位
    bool isValid() const { return rep_->scale_ != 0.0; }

public: // operators

    /// @brief 单位克隆
    /// @return 新单位
    Unit clone() const { return Unit(std::make_shared<UnitRep>(*rep_)); }

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

    /// @brief 单位缩放
    /// @param newname 新单位名称
    /// @param factor 缩放因子
    /// @return 新单位
    Unit scale(StringView newname, double factor) const
    {
        return Unit(newname, factor * rep_->scale_, rep_->dimension_);
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
        // 单位缩放因子和量纲相同即可认为是相同单位
        return rep_->scale_ == other.rep_->scale_ && rep_->dimension_ == other.rep_->dimension_;    
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
    UnitRepHandle rep_;
};


namespace units
{
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
    extern AST_UTIL_API Unit min;       ///< 分钟
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

    extern AST_UTIL_API Unit m2;        ///< 平方米

    extern AST_UTIL_API Unit m3;        ///< 立方米
    extern AST_UTIL_API Unit L;         ///< 升
    
};





AST_NAMESPACE_END

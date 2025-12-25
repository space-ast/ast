///
/// @file      Dimension.hpp
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
#include <string>

AST_NAMESPACE_BEGIN


enum class EDimension: int;

/// @brief 获取量纲的指数
template<typename T>
constexpr int dim_get_exponent(T dimension, int idx) noexcept
{
    static_assert(sizeof(T) == sizeof(EDimension), "size not correct");
    int value = (int)dimension;
    int shift = static_cast<int>(idx) * 4;
    const size_t size = sizeof(T) * 8;
    int exponent = (int((unsigned)value << (size - 4 - shift)) >> (size - 4));  // 先逻辑左移，再算数右移
    return exponent;
}

/// @brief 设置量纲的指数
template<typename T>
constexpr EDimension dim_set_exponent(T dimension, int idx, int exponent) noexcept
{
    static_assert(sizeof(T) == sizeof(EDimension), "size not correct");
    // @todo 检查指数范围（4位有符号数范围：-8到7）
    // if (exponent < -8 || exponent > 7) {
    //     //throw std::out_of_range("Exponent must be between -8 and 7");
    // }
    int value = (int) dimension;
    int shift = static_cast<int>(idx) * 4;
    int mask = 0xF << shift;
    int newValue = value & ~mask; // 清除原值
    // 将指数转换为4位有符号表示
    int expBits = exponent & 0xF;
    newValue |= (expBits << shift); // 设置新值

    return (EDimension)newValue;
}


/// @brief 获取量纲的指数绝对值的和
template<typename T>
constexpr int dim_get_sum_abs_exponent(T dimension) noexcept
{
    int sum = 0;
    for (int i = 0; i < 8; ++i) {
        sum += abs(dim_get_exponent(dimension, i));
    }
    return sum;
}

/// @brief 量纲乘法
template<typename T>
constexpr int dim_product(T dim1, T dim2) noexcept
{
    static_assert(sizeof(T) == sizeof(EDimension), "size not correct");
    const size_t size = sizeof(T) * 8;
    int result = 0;
    int value1 = (int)dim1;
    int value2 = (int)dim2;

    // 对每个量纲分别处理
    for (int i = 0; i < 8; ++i) {
        int shift = i * 4;
        // 提取两个操作数的指数
        int exp1 = (int((unsigned)value1 << (size - 4 - shift)) >> (size - 4)); // 先逻辑左移，再算数右移
        int exp2 = (int((unsigned)value2 << (size - 4 - shift)) >> (size - 4)); // 先逻辑左移，再算数右移

        // 指数相加
        int sum = exp1 + exp2;

        // @todo 检查是否溢出（4位有符号数范围：-8到7）
        // if (sum < -8 || sum > 7) {
        //     //throw std::overflow_error("Exponent overflow in multiplication");
        // }

        // 转换回4位表示
        int sumBits = sum & 0xF;
        result |= (sumBits << shift);
    }

    return result;
}

// 量纲除法
template<typename T>
constexpr int dim_divide(T dim1, T dim2) noexcept
{
    static_assert(sizeof(T) == sizeof(EDimension), "size not correct");
    const size_t size = sizeof(T) * 8;
    int result = 0;
    int value1 = (int)dim1;
    int value2 = (int)dim2;


    // 对每个量纲分别处理
    for (int i = 0; i < 8; ++i) {
        int shift = i * 4;
        // 提取两个操作数的指数
        int exp1 = (int((unsigned)value1 << (size - 4 - shift)) >> (size - 4)); // 先逻辑左移，再算数右移
        int exp2 = (int((unsigned)value2 << (size - 4 - shift)) >> (size - 4)); // 先逻辑左移，再算数右移

        // 指数相减
        int diff = exp1 - exp2;

        // @todo 检查是否溢出
        // if (diff < -8 || diff > 7) {
        //     // throw std::overflow_error("Exponent overflow in division");
        // }

        // 转换回4位表示
        int diffBits = diff & 0xF;
        result |= (diffBits << shift);
    }

    return result;
}

/// @brief 量纲指数幂
template<typename T>
constexpr int dim_pow(T dimension, int n) noexcept
{
    static_assert(sizeof(T) == sizeof(EDimension), "size not correct");
    const size_t size = sizeof(T) * 8;
    int result = 0;
    int value = (int)dimension;

    for (int i = 0; i < 8; ++i) {
        int shift = i * 4;
        int exp = (int((unsigned)value << (size - 4 - shift)) >> (size - 4)); // 先逻辑左移，再算数右移

        int newExp = exp * n;

        // @todo 检查是否溢出
        // if (newExp < -8 || newExp > 7) {
        //     throw std::overflow_error("Exponent overflow in power operation");
        // }

        int newExpBits = newExp & 0xF;
        result |= (newExpBits << shift);
    }

    return result;
}


template<typename T>
constexpr int dim_invert(T dimension) noexcept
{
    return dim_pow(dimension, -1);
}

/// @brief 量纲乘法运算符
constexpr EDimension operator *(EDimension dim1, EDimension dim2) noexcept
{
    return (EDimension)dim_product(dim1, dim2);
}

/// @brief 量纲除法运算符
constexpr EDimension operator/(EDimension dim1, EDimension dim2) noexcept
{
    return (EDimension)dim_divide(dim1, dim2);
}

/// @brief 量纲指数幂运算符
constexpr EDimension pow(EDimension dimension, int n) noexcept
{
    return (EDimension)dim_pow(dimension, n);
}

// @brief 量纲索引枚举类
enum
{
    kIdxMass,
    kIdxLength,
    kIdxAngle,
    kIdxTime,
    kIdxTemperature,
    kIdxCurrent,
    kIdxAmount,
    kIdxLuminous,
};

/// @brief 量纲枚举类
/// @details 采用enum class，避免隐式转换为算数类型参与运算，应使用量纲的自定义运算符
/// @details 每个量纲维度由4个bit表示，指数范围为-8到7
typedef enum class EDimension
{
    eUnit               = 0,

    // 基本量纲 7个
    eLength             = 1 << kIdxLength * 4,                ///< 长度L      (相应的基本单位: m)
    eMass               = 1 << kIdxMass * 4,                  ///< 质量M      (相应的基本单位: kg)
    eTime               = 1 << kIdxTime * 4,                  ///< 时间T      (相应的基本单位: s)
    eCurrent            = 1 << kIdxCurrent * 4,               ///< 电流I      (相应的基本单位: A)
    eTemperature        = 1 << kIdxTemperature * 4,           ///< 温度Θ      (相应的基本单位: K)
    eAmount             = 1 << kIdxAmount * 4,                ///< 物质量N    (相应的基本单位: mol)
    eLuminous           = 1 << kIdxLuminous * 4,              ///< 发光强度J  (相应的基本单位: cd)
    // 辅助量纲 
    eAngle              = 1 << kIdxAngle * 4,                 ///< 角度

    //  
    eAngVel             = dim_divide(eAngle, eTime),          ///< 角速度（别名）
    eAngularVelocity    = eAngVel,                            ///< 角速度

    // 导出量纲（由基本量纲组合而成） 
    eArea               = dim_product(eLength, eLength),      ///< 面积   L^2
    eVolume             = dim_product(eArea, eLength),        ///< 体积   L^3
    eSpeed              = dim_divide(eLength, eTime),         ///< 速度   L·T^-1
    eAcceleration       = dim_divide(eSpeed, eTime),          ///< 加速度 L·T^-2
    eForce              = dim_product(eMass, eAcceleration),  ///< 力     M·L·T^-2
    ePressure           = dim_divide(eForce, eArea),          ///< 压力   M·L^-1·T^-2
    eEnergy             = dim_product(eForce, eLength),       ///< 能量   M·L^2·T^-2
    ePower              = dim_divide(eEnergy, eTime),         ///< 功率   M·L^2·T^-3
    eFrequency          = dim_divide(eUnit, eTime),           ///< 频率   T^-1

} AEDimension;


/// @brief 获取量纲的名称
AST_UTIL_API std::string aDimName(EDimension dimension);

/// @brief 获取量纲的符号
AST_UTIL_API std::string aDimSymbol(EDimension dimension);

/// @brief 判断量纲是否为基本量纲
constexpr bool aDimIsBase(EDimension dimension) noexcept
{
    return dim_get_sum_abs_exponent(dimension) == 1;
}

/// @brief 判断量纲是否为导出量纲
constexpr bool aDimIsDerived(EDimension dimension) noexcept
{
    return dim_get_sum_abs_exponent(dimension) > 1;
}

/// @brief 判断量纲是否为单位量纲
constexpr bool aDimIsUnit(EDimension dimension) noexcept
{
    return dimension == EDimension::eUnit;
}



/// @brief 量纲
class Dimension
{
public:
    /// @brief 默认构造函数，默认单位量纲
    constexpr Dimension() noexcept: dimension_(EDimension::eUnit) {}

    /// @brief 隐式构造函数
    constexpr Dimension(EDimension dimension) noexcept: dimension_(dimension) {}
    
    /// @brief 获取单位量纲
    static constexpr Dimension Unit() { return Dimension(EDimension::eUnit); }
    /// @brief 获取长度量纲
    static constexpr Dimension Length() { return Dimension(EDimension::eLength); }
    /// @brief 获取质量量纲
    static constexpr Dimension Mass() { return Dimension(EDimension::eMass); }
    /// @brief 获取时间量纲
    static constexpr Dimension Time() { return Dimension(EDimension::eTime); }
    /// @brief 获取电流量纲
    static constexpr Dimension Current() { return Dimension(EDimension::eCurrent); }
    /// @brief 获取温度量纲
    static constexpr Dimension Temperature() { return Dimension(EDimension::eTemperature); }
    /// @brief 获取物质量量纲
    static constexpr Dimension Amount() { return Dimension(EDimension::eAmount); }
    /// @brief 获取发光强度量纲
    static constexpr Dimension Luminous() { return Dimension(EDimension::eLuminous); }
    /// @brief 获取角度量纲
    static constexpr Dimension Angle() { return Dimension(EDimension::eAngle); }
    /// @brief 获取角速度量纲
    static constexpr Dimension AngVel() noexcept{ return Dimension(EDimension::eAngVel); }
    /// @brief 获取角速度量纲
    static constexpr Dimension AngularVelocity() noexcept{ return Dimension(EDimension::eAngularVelocity); }
    /// @brief 获取面积量纲
    static constexpr Dimension Area() noexcept{ return Dimension(EDimension::eArea); }
    /// @brief 获取体积量纲
    static constexpr Dimension Volume() noexcept{ return Dimension(EDimension::eVolume); }
    /// @brief 获取速度量纲
    static constexpr Dimension Speed() noexcept{ return Dimension(EDimension::eSpeed); }
    /// @brief 获取加速度量纲
    static constexpr Dimension Acceleration() noexcept{ return Dimension(EDimension::eAcceleration); }
    /// @brief 获取压力量纲
    static constexpr Dimension Pressure() noexcept{ return Dimension(EDimension::ePressure); }
    /// @brief 获取能量量纲
    static constexpr Dimension Energy() noexcept{ return Dimension(EDimension::eEnergy); }
    /// @brief 获取功率量纲
    static constexpr Dimension Power() noexcept{ return Dimension(EDimension::ePower); }
    /// @brief 获取频率量纲
    static constexpr Dimension Frequency() noexcept { return Dimension(EDimension::eFrequency); }
    /// @brief 获取力量纲
    static constexpr Dimension Force() noexcept{ return Dimension(EDimension::eForce); }
public:
    /// @brief 获取量纲的名称
    std::string name() const { return aDimName(value()); }
    /// @brief 获取量纲的符号
    std::string symbol() const { return aDimSymbol(value()); }
    /// @brief 判断量纲是否为基本量纲
    bool isBase() const noexcept { return aDimIsBase(value()); }
    /// @brief 判断量纲是否为导出量纲
    bool isDerived() const noexcept { return aDimIsDerived(value()); }
    /// @brief 判断量纲是否为单位量纲
    bool isUnit() const noexcept { return aDimIsUnit(value()); }
public:
    /// @brief 量纲指数幂
    constexpr Dimension pow(int n) const noexcept
    {
        return EDimension(dim_pow(value(), n));
    }
    /// @brief 量纲倒数
    constexpr Dimension invert() const noexcept
    {
        return EDimension(dim_invert(value()));
    }
    /// @brief 量纲乘法运算符
    constexpr Dimension operator*(Dimension other) const noexcept
    {
        return value() * other.value();
    }
    /// @brief 量纲乘法赋值运算符
    constexpr Dimension& operator*=(Dimension other) noexcept
    {
        this->dimension_ = value() * other.value();
        return *this;
    }
    /// @brief 量纲除法运算符
    constexpr Dimension operator/(Dimension other) const noexcept
    {
        return value() / other.value();
    }
    /// @brief 量纲除法赋值运算符
    constexpr Dimension& operator/=(Dimension other) noexcept
    {
        this->dimension_ = value() / other.value();
        return *this;
    }
    /// @brief 量纲相等运算符
    constexpr bool operator==(Dimension other) const noexcept
    {
        return value() == other.value();
    }
    /// @brief 量纲不相等运算符
    constexpr bool operator!=(Dimension other) const noexcept
    {
        return value() != other.value();
    }
    constexpr bool operator==(EDimension other) const noexcept
    {
        return value() == other;
    }
    constexpr bool operator!=(EDimension other) const noexcept
    {
        return value() != other;
    }
public:
    /// @brief 转换为EDimension枚举
    constexpr operator EDimension() const noexcept { return dimension_; }
    /// @brief 获取量纲的值
    constexpr EDimension value() const noexcept { return dimension_; }
    /// @brief 获取量纲中的质量维度
    constexpr int getMass() const noexcept { return dim_get_exponent(dimension_, kIdxMass); }
    /// @brief 获取量纲中的长度维度
    constexpr int getLength() const noexcept { return dim_get_exponent(dimension_, kIdxLength);}
    /// @brief 获取量纲中的时间维度
    constexpr int getTime() const noexcept { return dim_get_exponent(dimension_, kIdxTime);}
    /// @brief 获取量纲中的电流维度
    constexpr int getCurrent() const noexcept { return  dim_get_exponent(dimension_, kIdxCurrent);}
    /// @brief 获取量纲中的角度维度
    constexpr int getAngle() const noexcept { return  dim_get_exponent(dimension_, kIdxAngle);}
    /// @brief 获取量纲中的温度维度
    constexpr int getTemperature() const noexcept { return  dim_get_exponent(dimension_, kIdxTemperature);}
    /// @brief 获取量纲中的物质量维度
    constexpr int getAmount() const noexcept { return  dim_get_exponent(dimension_, kIdxAmount);}
    /// @brief 获取量纲中的发光强度维度
    constexpr int getLuminous() const noexcept { return  dim_get_exponent(dimension_, kIdxLuminous);}

    /// @brief 设置量纲中的质量维度
    constexpr Dimension& setMass(int n) noexcept
    {
        dimension_ = (EDimension)dim_set_exponent(dimension_, kIdxMass, n);
        return *this;
    }
    /// @brief 设置量纲中的长度维度
    constexpr Dimension& setLength(int n) noexcept
    {
        dimension_ = (EDimension)dim_set_exponent(dimension_, kIdxLength, n);
        return *this;
    }
    /// @brief 设置量纲中的时间维度
    constexpr Dimension& setTime(int n) noexcept
    {
        dimension_ = (EDimension)dim_set_exponent(dimension_, kIdxTime, n);
        return *this;
    }
    /// @brief 设置量纲中的电流维度
    constexpr Dimension& setCurrent(int n) noexcept
    {
        dimension_ = (EDimension)dim_set_exponent(dimension_, kIdxCurrent, n);
        return *this;
    }
    /// @brief 设置量纲中的角度维度
    constexpr Dimension& setAngle(int n) noexcept
    {
        dimension_ = (EDimension)dim_set_exponent(dimension_, kIdxAngle, n);
        return *this;
    }
    /// @brief 设置量纲中的温度维度
    constexpr Dimension& setTemperature(int n) noexcept
    {
        dimension_ = (EDimension)dim_set_exponent(dimension_, kIdxTemperature, n);
        return *this;
    }
    /// @brief 设置量纲中的物质量维度
    constexpr Dimension& setAmount(int n) noexcept
    {
        dimension_ = (EDimension)dim_set_exponent(dimension_, kIdxAmount, n);
        return *this;
    }
    /// @brief 设置量纲中的发光强度维度
    constexpr Dimension& setLuminous(int n) noexcept
    {
        dimension_ = (EDimension)dim_set_exponent(dimension_, kIdxLuminous, n);
        return *this;
    }
    
protected:
    EDimension dimension_;
};

/// @brief 量纲指数幂运算符
constexpr Dimension pow(Dimension dim, int n)
{
    return pow(dim.value(), n);
}

AST_NAMESPACE_END

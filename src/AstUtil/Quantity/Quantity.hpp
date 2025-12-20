///
/// @file      Quantity.hpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-15
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
#include "AstUtil/Unit.hpp"
#include <string>

AST_NAMESPACE_BEGIN


class Quantity;

/// @brief 数量值加法
/// @param q1 数量值1
/// @param q2 数量值2
/// @return 数量值1与数量值2的和
AST_UTIL_API Quantity aQuantityAdd(const Quantity& q1, const Quantity& q2);

/// @brief 数量值减法
/// @param q1 数量值1
/// @param q2 数量值2
/// @return 数量值1与数量值2的差
AST_UTIL_API Quantity aQuantitySub(const Quantity& q1, const Quantity& q2);


/// @brief 数量值乘法
/// @param q1 数量值1
/// @param q2 数量值2
/// @return 数量值1与数量值2的积
AST_UTIL_API Quantity aQuantityMul(const Quantity& q1, const Quantity& q2);


/// @brief 数量值单位乘法
/// @param q 数量值
/// @param unit 单位
/// @return 数量值与单位的积
AST_UTIL_API Quantity aQuantityMul(const Quantity& q, const Unit& unit);


/// @brief 数量值除法
/// @param q1 数量值1
/// @param q2 数量值2
/// @return 数量值1与数量值2的商
AST_UTIL_API Quantity aQuantityDiv(const Quantity& q1, const Quantity& q2);


/// @brief 数量值单位除法
/// @param q 数量值
/// @param unit 单位
/// @return 数量值与单位的商
AST_UTIL_API Quantity aQuantityDiv(const Quantity& q, const Unit& unit);


/// @brief 数量值简化
/// @param q 数量值
AST_UTIL_API void aQuantityReduce(Quantity& q);


/// @brief 数量值转换为字符串
/// @param q 数量值
/// @return 数量值的字符串表示
AST_UTIL_API std::string aQuantityToString(const Quantity& q);

/// @brief 单位乘法运算符
/// @param value 数值
/// @param unit 单位
/// @return 新单位
AST_UTIL_API Quantity operator*(double value, const Unit& unit);



/// @brief 带单位的数量值
class Quantity
{
public:
    /// @brief 无效数量值
    static Quantity NaN()
    {
        return Quantity(0.0, Unit::NaN());
    }

    /// @brief 零数量值
    static Quantity Zero()
    {
        return Scalar(0.0);
    }

    /// @brief 单位数量值
    static Quantity One()
    {
        return Scalar(1.0);
    }

    /// @brief 标量数量值
    static Quantity Scalar(double value)
    {
        return Quantity(value, Unit::None());
    }

public:
    Quantity()
        : value_(0.0), unit_{}
    {}
    Quantity(double value, const Unit& unit)
        : value_(value), unit_(unit)
    {}
public:
    /// @brief 获取数量值
    /// @return 数量值
    double value() const { return value_; }
    
    /// @brief 获取可修改的数量值
    /// @return 可修改的数量值
    double& value() { return value_; }

    /// @brief 获取单位
    /// @return 单位
    const Unit& unit() const { return unit_; }

    /// @brief 获取可修改的单位
    /// @return 可修改的单位
    Unit& unit() { return unit_; }
    
    /// @brief 获取量纲
    /// @return 量纲
    EDimension dimension() const { return unit_.dimension(); }

    /// @brief 设置数量值
    /// @param value 数量值
    void setValue(double value) { value_ = value; }

    /// @brief 设置单位
    /// @param unit 单位
    void setUnit(const Unit& unit) { unit_ = unit; }

    
    /// @brief 获取内部运行时单位表示的值
    /// @return 内部运行时单位表示的值
    double getInternalValue() const { return unit_.toInternal(value_); }


    /// @brief 检查数量值是否有效
    /// @return 如果数量值有效则返回true，否则返回false
    bool isValid() const { return unit_.isValid(); }

    /// @brief 检查数量值是否相等
    /// @param q 数量值
    /// @return 如果数量值相等则返回true，否则返回false
    bool operator ==(const Quantity& q) const{
        return this->dimension() == q.dimension() && this->getInternalValue() == q.getInternalValue();
    }
    /// @brief 检查数量值是否不相等
    /// @param q 数量值
    /// @return 如果数量值不相等则返回true，否则返回false
    bool operator !=(const Quantity& q) const{return !(*this == q);}
    
    /// @brief 检查数量值是否等于标量
    /// @param value 标量值
    /// @return 如果数量值等于标量值则返回true，否则返回false
    bool operator == (double value) const{
        return this->dimension() == EDimension::eUnit && this->getInternalValue() == value;
    }

    /// @brief 检查数量值是否不等于标量
    /// @param value 标量值
    /// @return 如果数量值不等于标量值则返回true，否则返回false
    bool operator != (double value) const{
        return !(*this == value);
    }

    /// @brief 数量值取正
    Quantity operator+() const { return *this; }
    /// @brief 数量值取负
    Quantity operator-() const { return Quantity(-value_, unit_); }

    /// @brief 数量值缩放
    Quantity operator*(double scale) const { return Quantity(value_ * scale, unit_); }
    /// @brief 数量值除法
    Quantity operator/(double scale) const { return Quantity(value_ / scale, unit_); }
    /// @brief 数量值缩放赋值
    Quantity& operator*=(double scale) { value_ *= scale; return *this; }
    /// @brief 数量值除法赋值
    Quantity& operator/=(double scale) { value_ /= scale; return *this; }
    /// @brief 数量值单位乘法
    Quantity operator*(const Unit& unit) const { return aQuantityMul(*this, unit); }
    /// @brief 数量值单位除法
    Quantity operator/(const Unit& unit) const { return aQuantityDiv(*this, unit); }
    /// @brief 数量值单位乘法赋值
    Quantity& operator*=(const Unit& unit) { *this = aQuantityMul(*this, unit); return *this; }
    /// @brief 数量值单位除法赋值
    Quantity& operator/=(const Unit& unit) { *this = aQuantityDiv(*this, unit); return *this; }


    /// @brief 数量值加法
    Quantity operator+(const Quantity& q) const { return aQuantityAdd(*this, q); }
    /// @brief 数量值减法
    Quantity operator-(const Quantity& q) const { return aQuantitySub(*this, q); }
    /// @brief 数量值乘法
    Quantity operator*(const Quantity& q) const { return aQuantityMul(*this, q); }
    /// @brief 数量值除法
    Quantity operator/(const Quantity& q) const { return aQuantityDiv(*this, q); }
    /// @brief 数量值加法赋值
    Quantity& operator+=(const Quantity& q) { *this = aQuantityAdd(*this, q); return *this; }
    /// @brief 数量值减法赋值
    Quantity& operator-=(const Quantity& q) { *this = aQuantitySub(*this, q); return *this; }
    /// @brief 数量值乘法赋值
    Quantity& operator*=(const Quantity& q) { *this = aQuantityMul(*this, q); return *this; }
    /// @brief 数量值除法赋值
    Quantity& operator/=(const Quantity& q) { *this = aQuantityDiv(*this, q); return *this; }
    /// @brief 数量值转换为字符串
    /// @return 数量值的字符串表示
    std::string toString() const{return aQuantityToString(*this);}
private:
    double value_;
    Unit unit_;
};

inline Quantity operator*(double value, const Quantity& q)
{
    return q * value;
}

AST_NAMESPACE_END

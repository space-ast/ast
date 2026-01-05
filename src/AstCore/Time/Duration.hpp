///
/// @file      Duration.hpp
/// @brief     
/// @details   ~
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
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。
 
#pragma once
 
#include "AstGlobal.h"
#include <stdint.h>     // for int64_t
 
AST_NAMESPACE_BEGIN
 

/// @brief 短时长
class ShortDuration
{
public:
    double second() const{return second_;}
    double& second(){return second_;}
public:
    double second_;         // 秒数
};


/// @brief 长时长
class LongDuration
{
public:
    /// @brief 获取整数秒部分
    int64_t integer() const{return integer_;}
    int64_t& integer(){return integer_;}
    /// @brief 获取小数秒部分
    double fractional() const{return fractional_;}
    double& fractional(){return fractional_;}
    
    /// @brief 时长减法运算符
    /// @param other 另一个时长
    /// @return 两个时长的差值（秒数）
    double operator-(const LongDuration& other) const{
        return (integer_ - other.integer_) + (fractional_ - other.fractional_);
    }
public:
    int64_t integer_;       // 整数秒部分
    double  fractional_;    // 小数秒部分
};


/// @brief 整数天 + 小数秒时长（也能解决长时长问题）
class DaySecDuration
{
public:
    /// @brief 获取整数天部分
    int day() const{return day_;}
    int& day(){return day_;}

    /// @brief 获取小数部分的秒数
    double second() const{return second_;}
    double& second(){return second_;}
    /// @warning 
    /// 注意：totalSecond() 方法返回的是一个不精确的double值，
    /// 计算会引入舍入误差。且不考虑闰秒等因素。
    /// @return 总秒数（整数天秒数 + 小数秒数）
    double totalSecond() const{return day_ * 86400.0 + second_;}
public:
    int    day_;            // 整数天部分
    double second_;         // 小数秒部分
};

 
AST_NAMESPACE_END
 

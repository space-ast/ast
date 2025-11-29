///
/// @file      Time.hpp
/// @brief     
/// @details   ~
/// @author    jinke18
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
 
AST_NAMESPACE_BEGIN

class Time;


/// @brief 规范化时间对象
/// @details 将时间中的秒、分进行进位或借位调整，使其处于合理范围内（0-59）
/// @param time 要规范化的时间对象
AST_CORE_CAPI void aTimeNormalize(Time& time);




/// @brief 时间
class Time
{
public:
    int hour() const{return hour_;}
    int& hour(){return hour_;}
    int minute() const{return minute_;}
    int& minute(){return minute_;}
    double second() const{return second_;}
    double& second(){return second_;}
    void setHour(int hour){hour_ = hour;}
    void setMinute(int minute){minute_ = minute;}
    void setSecond(double second){second_ = second;}
public:
    /// @brief 规范化时间对象
    void normalize(){
        aTimeNormalize(*this);
    }
    
    /// @brief 获取规范化后的时间对象副本
    Time normalized() const{
        Time t = *this;
        aTimeNormalize(t);
        return t;
    }
public:
    int    hour_;       ///< 时
    int    minute_;     ///< 分
    double second_;     ///< 秒
};
 
 
AST_NAMESPACE_END
 

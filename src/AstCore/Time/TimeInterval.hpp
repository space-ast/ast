///
/// @file      TimeInterval.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-02-04
/// @copyright 版权所有 (C) 2026-present, ast项目.
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
#include "TimePoint.hpp"
#include <string>

AST_NAMESPACE_BEGIN

class TimeInterval;

/// @brief 将时间区间格式化为字符串
/// @param interval 时间区间
/// @param str 输出字符串
/// @return err_t 错误码
AST_CORE_CAPI err_t aTimeIntervalFormat(const TimeInterval& interval, std::string& strStart, std::string& strEnd);


/// @brief 从字符串解析时间区间
/// @param str 输入字符串
/// @param interval 输出时间区间
/// @return err_t 错误码
AST_CORE_CAPI err_t aTimeIntervalParse(StringView strStart, StringView strEnd, TimeInterval& interval);


/// @brief 时间区间
/// @details 时间区间表示一个连续时间区间，由开始时间点和结束时间点定义。
class TimeInterval
{
public:
    TimeInterval() = default;
    ~TimeInterval() = default;

    TimeInterval(const TimePoint& start, const TimePoint& end){
        this->setStartEnd(start, end);
    }

    /// @brief 时间区间的开始时间点
    const TimePoint& start() const{return reinterpret_cast<const TimePoint&>(*this);}

    /// @brief 时间区间的开始时间点
    const TimePoint& getStart() const{return reinterpret_cast<const TimePoint&>(*this);}

    /// @brief 时间区间的结束时间点
    TimePoint end() const{return {epoch_, end_};}

    /// @brief 时间区间的结束时间点
    TimePoint getEnd() const{return {epoch_, end_};}


    /// @brief 设置时间区间的开始时间点和结束时间点
    void setStartEnd(const TimePoint& start, const TimePoint& end){
        epoch_ = start.integerPart();
        start_ = start.fractionalPart();
        end_ = end.fractionalPart() + (end.integerPart() - start.integerPart());
    }

    /// @brief 时间区间的持续时间（秒）
    double duration() const{return end_ - start_;}


    /// @brief 将时间区间转换为字符串
    std::string toString() const{
        std::string strStart, strEnd;
        aTimeIntervalFormat(*this, strStart, strEnd);
        return strStart + " - " + strEnd;
    }

protected:
    int64_t epoch_;     ///< 时间区间的基准时间点（秒，从J2000.0 TAI 开始）
    double  start_;     ///< 相对开始时间(s)
    double  end_;       ///< 相对结束时间(s)
};

AST_NAMESPACE_END

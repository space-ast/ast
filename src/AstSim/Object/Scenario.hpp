///
/// @file      Scenario.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-07
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
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
#include "AstUtil/Object.hpp"
#include "AstUtil/StringView.hpp"
// #include "AstCore/EventTime.hpp"
// #include "AstCore/EventInterval.hpp"
// #include "AstCore/CelestialBody.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class EventTime;
class EventInterval;
class CelestialBody;

/// @brief 场景类
/// @details 场景类用于定义模拟中的场景，包括场景的初始化、运行和结束。
class AST_SIM_API Scenario: public Object
{
public:
    AST_OBJECT(Scenario)
    Scenario();
    ~Scenario();
public:
    /// @brief 获取场景历元时间
    /// @return SharedPtr<EventTime>& 场景历元时间句柄
    SharedPtr<EventTime>& getEpochHandle() { return epoch_; }

    /// @brief 获取场景历元时间
    /// @param epoch 场景历元时间
    /// @return errc_t 错误码
    errc_t getEpoch(TimePoint& epoch) const;

    /// @brief 设置场景历元时间
    /// @param epoch 场景历元时间
    void setEpoch(EventTime* epoch);

    /// @brief 设置场景历元时间
    /// @param epoch 场景历元时间
    void setEpoch(const TimePoint& epoch);

    /// @brief 获取场景时间间隔
    /// @return SharedPtr<EventInterval>& 场景时间间隔句柄
    SharedPtr<EventInterval>& getIntervalHandle() { return interval_; }

    /// @brief 获取场景时间间隔
    /// @param interval 场景时间间隔
    /// @return errc_t 错误码
    errc_t getInterval(TimeInterval& interval) const;

    /// @brief 设置场景时间间隔
    /// @param interval 场景时间间隔
    void setInterval(EventInterval* interval);

    /// @brief 设置场景时间间隔
    /// @param interval 场景时间间隔
    void setInterval(const TimeInterval& interval);
public:
    void setName(StringView name) override {name_ = std::string(name);}

    const std::string& getName() const override{return name_;}

    void setEOPFileName(StringView filename){eopFileName_ = std::string(filename);}
    
    void setInheritEOPSource(bool inherit){inheritEOPSource_ = inherit;}

    void setPrimaryBody(CelestialBody* body);
protected:
    std::string name_;                      ///< 场景名称
    SharedPtr<EventTime> epoch_;            ///< 场景历元时间
    SharedPtr<EventInterval> interval_;     ///< 场景时间间隔
    std::string eopFileName_;               ///< EOP文件名
    bool inheritEOPSource_{};                 ///< 是否继承EOP来源设置
    SharedPtr<CelestialBody> primaryBody_;                  ///< 场景天体
};

/*! @} */

AST_NAMESPACE_END

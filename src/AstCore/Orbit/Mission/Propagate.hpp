///
/// @file      Propagate.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-15
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
#include "MissionCommand.hpp"
#include "Segment.hpp"
#include "AstCore/HPOP.hpp"
#include "AstCore/EventDetector.hpp"


AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 预报段，用于建模任务序列(MissionCommand)中的轨道预报
class AST_CORE_API Propagate: public Segment
{
public:
    AST_OBJECT(Propagate)
    Propagate() = default;
    ~Propagate() = default;
public:
    errc_t execute() override;
public:
    HPOP* propagator() const { return propagator_.get(); }
    void setPropagator(HPOP* propagator) { propagator_ = propagator; }
    EventDetector* getEventDetector(StringView name) const;
    void setEventDetectors(const std::vector<SharedPtr<EventDetector>>& eventDetectors) { eventDetectors_ = eventDetectors; }
PROPERTIES:
    double minPropTime() const { return minPropTime_; }
    void setMinPropTime(double minPropTime) { minPropTime_ = minPropTime; }

    double maxPropTime() const { return maxPropTime_; }
    void setMaxPropTime(double maxPropTime) { maxPropTime_ = maxPropTime; }

    bool useMaxPropTime() const { return useMaxPropTime_; }
    void setUseMaxPropTime(bool useMaxPropTime) { useMaxPropTime_ = useMaxPropTime; }

    bool useMaxPropTimeWarn() const { return useMaxPropTimeWarn_; }
    void setUseMaxPropTimeWarn(bool useMaxPropTimeWarn) { useMaxPropTimeWarn_ = useMaxPropTimeWarn; }

    bool overrideMaxPropTime() const { return overrideMaxPropTime_; }
    void setOverrideMaxPropTime(bool overrideMaxPropTime) { overrideMaxPropTime_ = overrideMaxPropTime; }
private:
    WeakPtr<HPOP> propagator_;                                    ///< 轨道预报器
    std::vector<SharedPtr<EventDetector>> eventDetectors_;        ///< 事件检测器
    double minPropTime_{0};                                       ///< 最小预报时间??
    double maxPropTime_{8640000};                                 ///< 最大预报时间
    bool useMaxPropTime_{true};                                   ///< 是否使用最大预报时间
    bool useMaxPropTimeWarn_{true};                               ///< 是否警告最大预报时间超过最大预报时间
    bool overrideMaxPropTime_{true};                              ///< ???
};




/*! @} */

AST_NAMESPACE_END

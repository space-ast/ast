///
/// @file      MotionSax.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-19
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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
#include "AstUtil/BKVSax.hpp"
#include "AstCore/EventTime.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstSim/MotionWithIntervalStep.hpp"
#include "AstSim/MotionOrbitDynamics.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/
class MotionOrbitDynamics;
struct VehiclePathData;
class BKVParser;

class AST_LOADER_API MotionOrbitDynamicsSax: public BKVSax
{
public:
    MotionOrbitDynamicsSax(BKVParser& parser, const VehiclePathData& vehiclePathData)
        : parser_(parser)
        , vehiclePathData_(vehiclePathData)
    {}
    ~MotionOrbitDynamicsSax() override = default;
public:
    errc_t begin(StringView name) override;
    errc_t end(StringView name) override;
    errc_t keyValue(StringView key, const ValueView& value) override;
public:
    virtual errc_t getMotion(ScopedPtr<MotionProfile>& motion) = 0;
public:
    HEventTime makeStateEpoch() const;
    HEventInterval makeInterval() const;
protected:
    Body* getBody() const;
protected:
    BKVParser& parser_;
    const VehiclePathData& vehiclePathData_;
    SharedPtr<EventTime> ephemSmartEpoch_{};
    SharedPtr<EventInterval> ephemSmartInterval_{};
    TimePoint ephemEpoch_{};
    TimePoint startTime_{};
    TimePoint stopTime_{};
    bool useScenTime_ = false;
};

/*! @} */

AST_NAMESPACE_END

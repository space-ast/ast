///
/// @file      DataGroupEclipse.hpp
/// @brief     日食时刻数据组
/// @details   提供 "Eclipse Times" 报表所需的数据
///            事件型数据（非等步长时间网格），故继承 DataGroupInterval，
///            其 calculate 直接接收时间区间并产出日食事件行。
/// @author    axel
/// @date      2026-08-15
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
#include "AstReport/DataGroupInterval.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/TimeInterval.hpp"
#include "AstCore/Point.hpp"
#include "AstCore/Frame.hpp"

#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup AstReport
    @{
*/

/// @brief 日食时刻数据组
/// @details 遍历航天器在给定区间内的日食事件，为每个本影/半影段产出一行。
class AST_REPORT_API DataGroupEclipse : public DataGroupInterval
{
public:
    /// @brief 单行数据
    struct Data
    {
        TimePoint   startTime_{};
        TimePoint   stopTime_{};
        double      duration_{0.0};
        std::string obstruction_{};
        std::string currentCondition_{};
        std::string worstCondition_{};
        double      totalDuration_{0.0};

        const TimePoint&   getStartTime()       const { return startTime_; }
        const TimePoint&   getStopTime()        const { return stopTime_; }
        double             getDuration()        const { return duration_; }
        const std::string& getObstruction()     const { return obstruction_; }
        const std::string& getCurrentCondition() const { return currentCondition_; }
        const std::string& getWorstCondition()  const { return worstCondition_; }
        double             getTotalDuration()   const { return totalDuration_; }
    };

    static DataElements Elements();

    DataGroupEclipse() = default;
    ~DataGroupEclipse() = default;

    errc_t calculate(const TimeInterval& interval, VariantVector& result) const override;
    errc_t calculate(const TimeInterval& interval, std::vector<Data>& result) const;
    const DataElements& getElements() const override;

    /// @name 配置
    /// @{
    Point* getPoint() const { return point_.get(); }
    void   setPoint(Point* p) { point_ = p; }
    void   setLightSource(CelestialBody* b) { lightSource_ = b; }
    CelestialBody* getLightSource() const { return lightSource_.get(); }
    void   setOccultingBodies(const std::vector<HCelestialBody>& bodies) { occultingBodies_ = bodies; }
    void   setStepSize(double s) { stepSize_ = s; }
    /// @}

private:
    WeakPtr<Point>                 point_{nullptr};
    WeakPtr<CelestialBody>         lightSource_{nullptr};
    std::vector<HCelestialBody>    occultingBodies_{};
    double                         stepSize_{5.0};
};

/*! @} */

AST_NAMESPACE_END

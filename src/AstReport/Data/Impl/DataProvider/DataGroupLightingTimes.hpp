///
/// @file      DataGroupLightingTimes.hpp
/// @brief     光照时刻数据组
/// @details   提供 "Lighting Times" 报表所需的数据
///            事件型数据（非等步长时间网格），故继承 DataGroupInterval，
///            其 calculate 直接接收时间区间并产出指定光照类型（全光照/半影/本影）的区间行。
/// @author    axel
/// @date      2026-08-18
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
#include "AstUtil/StringView.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/Eclipse.hpp"
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

/// @brief 光照时刻数据组
/// @details 遍历航天器在给定区间内的光照状态，按 lightingType_ 筛选出对应光照类型
///           （全光照 Sunlight / 半影 Penumbra / 本影 Umbra）的区间，为每个区间产出一行。
class AST_REPORT_API DataGroupLightingTimes : public DataGroupInterval
{
public:
    /// @brief 单行数据
    struct Data
    {
        TimeInterval interval_{};
        std::string  obstruction_{};

        const TimePoint&   getStartTime()   const { return interval_.start(); }
        const TimePoint&   getStopTime()    const { return interval_.stop(); }
        double             getDuration()    const { return interval_.duration(); }
        const std::string& getObstruction() const { return obstruction_; }
    };

    static DataElements Elements();

    /// @brief 将 .rst 的 type 字符串映射为光照类型（未知输入默认全光照）
    static ELightingType LightingTypeFromString(StringView type);

    DataGroupLightingTimes() = default;
    ~DataGroupLightingTimes() = default;

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
    void   setLightingType(ELightingType t) { lightingType_ = t; }
    ELightingType getLightingType() const { return lightingType_; }
    /// @}

private:
    WeakPtr<Point>                 point_{nullptr};
    WeakPtr<CelestialBody>         lightSource_{nullptr};
    std::vector<HCelestialBody>    occultingBodies_{};
    double                         stepSize_{60.0};
    ELightingType                  lightingType_{ELightingType::eSunlight};
};

/*! @} */

AST_NAMESPACE_END

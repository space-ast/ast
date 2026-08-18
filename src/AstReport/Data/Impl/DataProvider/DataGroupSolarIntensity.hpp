///
/// @file      DataGroupSolarIntensity.hpp
/// @brief     太阳光照强度数据组
/// @details   提供 "Solar Intensity" 报表所需的数据（时间变量型）。
///             在每个时间网格点，对每个遮挡天体计算双锥模型光照比例，取最小值为
///             光照强度，并派生遮蔽比例、当前光照条件与遮挡体名称。
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
#include "AstReport/DataGroupTimeVar.hpp"
#include "AstCore/Point.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstUtil/Span.hpp"

#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup AstReport
    @{
*/

/// @brief 太阳光照强度数据组
/// @details 时间变量型数据组，每个时间网格点一行。光照强度取所有遮挡天体
///          双锥模型光照比例的最小值（0=本影，1=全光照，(0,1)=半影）。
class AST_REPORT_API DataGroupSolarIntensity : public DataGroupTimeVar
{
public:
    /// @brief 单行数据
    struct Data
    {
        TimePoint   time_{};
        double      intensity_{1.0};     ///< 光照强度 [0,1]
        std::string currentCondition_{}; ///< 当前光照条件：Sunlight/Penumbra/Umbra
        std::string obstruction_{};      ///< 遮挡体名称（全光照时为空）

        const TimePoint&   getTime()             const { return time_; }
        double             getIntensity()        const { return intensity_; }
        double             getShadowRatio()      const { return 1.0 - intensity_; }
        const std::string& getCurrentCondition() const { return currentCondition_; }
        const std::string& getObstruction()      const { return obstruction_; }
    };

    static DataElements Elements();

    DataGroupSolarIntensity() = default;
    ~DataGroupSolarIntensity() = default;

    errc_t calculate(const TimeList& timeList, VariantVector& result) const override;
    errc_t calculate(const TimeList& timeList, std::vector<Data>& result) const;
    errc_t calculate(const TimeList& timeList, Span<Data> result) const;
    const DataElements& getElements() const override;

    /// @name 配置
    /// @{
    Point* getPoint() const { return point_.get(); }
    void   setPoint(Point* p) { point_ = p; }
    void   setLightSource(CelestialBody* b) { lightSource_ = b; }
    CelestialBody* getLightSource() const { return lightSource_.get(); }
    void   setOccultingBodies(const std::vector<HCelestialBody>& bodies) { occultingBodies_ = bodies; }
    /// @}

private:
    WeakPtr<Point>              point_{nullptr};
    WeakPtr<CelestialBody>      lightSource_{nullptr};
    std::vector<HCelestialBody> occultingBodies_{};
};

/*! @} */

AST_NAMESPACE_END

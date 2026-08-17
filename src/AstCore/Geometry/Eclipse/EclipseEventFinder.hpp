///
/// @file      EclipseEventFinder.hpp
/// @brief     日食事件求解器
/// @details   在给定时间区间内求解航天器进出本影（Umbra）/半影（Penumbra）的时刻
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
#include "AstCore/CelestialBody.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/TimeInterval.hpp"
#include "AstCore/Point.hpp"
#include "AstCore/Frame.hpp"

#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Eclipse
    @{
*/

/// @brief 单个日食事件
/// @details 一次穿过某遮挡体阴影的完整事件，包含半影进入/本影进入/本影退出/半影退出
///          四个边界时刻。掠影（仅半影、未进入本影）时 hasUmbra() 为 false，
///          umbraStart_/umbraStop_ 为最深遮蔽时刻的近似值。
class AST_CORE_API EclipseEvent
{
public:
    /// @name 原始字段 getter
    /// @{
    CelestialBody* getObstruction() const { return obstruction_.get(); }
    const TimePoint& getPenumbraStart() const { return penumbraStart_; }
    const TimePoint& getUmbraStart()    const { return umbraStart_; }
    const TimePoint& getUmbraStop()     const { return umbraStop_; }
    const TimePoint& getPenumbraStop()  const { return penumbraStop_; }
    double          getMaxShadowRatio() const { return maxShadowRatio_; }
    const TimePoint& getTimeAtMaxShadow() const { return timeAtMaxShadow_; }
    bool            hasUmbra()          const { return hasUmbra_; }
    /// @}

    /// @name 派生元素 getter（对应报表列）
    /// @{
    /// @brief 本影时长 [s]（掠影时为 0）
    double getUmbraDuration() const { return umbraStop_ - umbraStart_; }
    /// @brief 半影时长 [s]（进入段 + 退出段之和；掠影时退化为单段半影）
    double getPenumbraDuration() const { return (umbraStart_ - penumbraStart_) + (penumbraStop_ - umbraStop_); }
    /// @brief 总遮蔽时长 [s]
    double getTotalDuration() const { return penumbraStop_ - penumbraStart_; }
    /// @brief 遮挡体名称（无遮挡体时为空串）
    std::string getObstructionName() const
    {
        CelestialBody* b = obstruction_.get();
        return b ? b->getName() : std::string{};
    }
    /// @brief 最小光照强度 [0,1]（= 1 - 最大遮蔽比例）
    double getMinIntensity() const { return 1.0 - maxShadowRatio_; }
    /// @brief 最小光照强度时刻（= 最大遮蔽时刻）
    const TimePoint& getTimeAtMinIntensity() const { return timeAtMaxShadow_; }
    /// @}

    /// @name setter
    /// @{
    void setObstruction(CelestialBody* b) { obstruction_ = b; }
    void setPenumbraStart(const TimePoint& t) { penumbraStart_ = t; }
    void setUmbraStart(const TimePoint& t)    { umbraStart_ = t; }
    void setUmbraStop(const TimePoint& t)     { umbraStop_ = t; }
    void setPenumbraStop(const TimePoint& t)  { penumbraStop_ = t; }
    void setMaxShadowRatio(double r)          { maxShadowRatio_ = r; }
    void setTimeAtMaxShadow(const TimePoint& t) { timeAtMaxShadow_ = t; }
    void setHasUmbra(bool b)                  { hasUmbra_ = b; }
    /// @}

private:
    WBody     obstruction_{};        ///< 遮挡天体
    TimePoint penumbraStart_{};      ///< 半影进入时刻
    TimePoint umbraStart_{};         ///< 本影进入时刻（无本影时为最深遮蔽时刻）
    TimePoint umbraStop_{};          ///< 本影退出时刻（无本影时为最深遮蔽时刻）
    TimePoint penumbraStop_{};       ///< 半影退出时刻
    double    maxShadowRatio_{0.0};  ///< 事件内最大遮蔽比例 [0, 1]（0=无遮蔽，1=全遮蔽）
    TimePoint timeAtMaxShadow_{};    ///< 最大遮蔽时刻
    bool      hasUmbra_{false};      ///< 是否进入本影（false = 掠影，仅半影）
};

/// @brief 日食事件求解器
/// @details 配置航天器点、光源与遮挡天体，调用 find() 在区间内求解日食事件。
///          光源默认太阳（aGetSun），遮挡体默认地球 + 月球。
class AST_CORE_API EclipseEventFinder
{
public:
    EclipseEventFinder() = default;
    ~EclipseEventFinder() = default;

    EclipseEventFinder(const EclipseEventFinder&) = default;
    EclipseEventFinder& operator=(const EclipseEventFinder&) = default;

    /// @name 配置
    /// @{
    void setPoint(Point* p) { point_ = p; }
    void setLightSource(CelestialBody* b) { lightSource_ = b; }
    void setOccultingBodies(const std::vector<HCelestialBody>& bodies) { occultingBodies_ = bodies; }
    void setStepSize(double s) { stepSize_ = s; }
    /// @}

    /// @brief 求解区间内的日食事件
    /// @param interval 搜索时间区间
    /// @param events   输出事件列表（按 penumbraStart_ 升序）
    /// @return 错误码
    /// @note 采用定步长采样，步长（stepSize）必须显著小于最短要检测的事件时长，
    ///       否则进出半影都落在同一步长内的短时事件（掠影、短时月影）会被遗漏。
    errc_t find(const TimeInterval& interval, std::vector<EclipseEvent>& events) const;

private:
    WeakPtr<Point> point_{nullptr};
    WeakPtr<CelestialBody> lightSource_{nullptr};
    std::vector<HCelestialBody> occultingBodies_{};
    double     stepSize_{60.0};
};

/*! @} */

AST_NAMESPACE_END

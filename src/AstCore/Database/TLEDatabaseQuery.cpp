///
/// @file      TLEDatabaseQuery.cpp
/// @brief     TLE 数据库查询条件实现
/// @details   实现基于子串匹配和范围比较的查询条件匹配逻辑。
/// @author    axel
/// @date      2026-07-24
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


#include "TLEDatabaseQuery.hpp"
#include "AstCore/TLE.hpp"

AST_NAMESPACE_BEGIN


TLEDatabaseQuery::TLEDatabaseQuery() = default;

TLEDatabaseQuery::~TLEDatabaseQuery() = default;

// ============================================================================
// 名称过滤
// ============================================================================

void TLEDatabaseQuery::setName(StringView pattern)
{
    name_ = std::string(pattern);
}

// ============================================================================
// NORAD 编号过滤
// ============================================================================

void TLEDatabaseQuery::setNoradId(int id)
{
    noradId_ = id;
    hasNoradId_ = true;
}

void TLEDatabaseQuery::clearNoradId()
{
    hasNoradId_ = false;
}

// ============================================================================
// 历元时间范围过滤
// ============================================================================

void TLEDatabaseQuery::setMinEpoch(const TimePoint& v)
{
    minEpoch_ = v;
    useMinEpoch_ = true;
}

void TLEDatabaseQuery::setMaxEpoch(const TimePoint& v)
{
    maxEpoch_ = v;
    useMaxEpoch_ = true;
}

void TLEDatabaseQuery::clearMinEpoch()
{
    useMinEpoch_ = false;
}

void TLEDatabaseQuery::clearMaxEpoch()
{
    useMaxEpoch_ = false;
}

// ============================================================================
// 密级过滤
// ============================================================================

void TLEDatabaseQuery::setClassification(char c)
{
    classification_ = c;
    hasClassification_ = true;
}

void TLEDatabaseQuery::clearClassification()
{
    hasClassification_ = false;
}

// ============================================================================
// 轨道参数范围过滤
// ============================================================================

void TLEDatabaseQuery::setMinInclination(double v)
{
    minInclination_ = v;
    useMinInclination_ = true;
}

void TLEDatabaseQuery::setMaxInclination(double v)
{
    maxInclination_ = v;
    useMaxInclination_ = true;
}

void TLEDatabaseQuery::clearMinInclination()
{
    useMinInclination_ = false;
}

void TLEDatabaseQuery::clearMaxInclination()
{
    useMaxInclination_ = false;
}

void TLEDatabaseQuery::setMinEccentricity(double v)
{
    minEccentricity_ = v;
    useMinEccentricity_ = true;
}

void TLEDatabaseQuery::setMaxEccentricity(double v)
{
    maxEccentricity_ = v;
    useMaxEccentricity_ = true;
}

void TLEDatabaseQuery::clearMinEccentricity()
{
    useMinEccentricity_ = false;
}

void TLEDatabaseQuery::clearMaxEccentricity()
{
    useMaxEccentricity_ = false;
}

void TLEDatabaseQuery::setMinMeanMotion(double v)
{
    minMeanMotion_ = v;
    useMinMeanMotion_ = true;
}

void TLEDatabaseQuery::setMaxMeanMotion(double v)
{
    maxMeanMotion_ = v;
    useMaxMeanMotion_ = true;
}

void TLEDatabaseQuery::clearMinMeanMotion()
{
    useMinMeanMotion_ = false;
}

void TLEDatabaseQuery::clearMaxMeanMotion()
{
    useMaxMeanMotion_ = false;
}

void TLEDatabaseQuery::setMinBstar(double v)
{
    minBstar_ = v;
    useMinBstar_ = true;
}

void TLEDatabaseQuery::setMaxBstar(double v)
{
    maxBstar_ = v;
    useMaxBstar_ = true;
}

void TLEDatabaseQuery::clearMinBstar()
{
    useMinBstar_ = false;
}

void TLEDatabaseQuery::clearMaxBstar()
{
    useMaxBstar_ = false;
}

// ============================================================================
// 匹配
// ============================================================================

bool TLEDatabaseQuery::matches(const TLE& tle) const
{
    // 名称过滤（子串匹配）
    if (!name_.empty())
    {
        if (tle.lines().name().find(name_) == std::string::npos)
            return false;
    }

    // NORAD 编号过滤（精确匹配）
    if (hasNoradId_)
    {
        if (tle.noradId() != noradId_)
            return false;
    }

    // 历元时间范围过滤
    {
        const TimePoint& epoch = tle.epochTime();
        if (useMinEpoch_ && epoch < minEpoch_)
            return false;
        if (useMaxEpoch_ && epoch > maxEpoch_)
            return false;
    }

    // 密级过滤
    if (hasClassification_)
    {
        if (tle.classification() != classification_)
            return false;
    }

    // 轨道倾角范围过滤
    if (useMinInclination_ && tle.inclination() < minInclination_)
        return false;
    if (useMaxInclination_ && tle.inclination() > maxInclination_)
        return false;

    // 偏心率范围过滤
    if (useMinEccentricity_ && tle.eccentricity() < minEccentricity_)
        return false;
    if (useMaxEccentricity_ && tle.eccentricity() > maxEccentricity_)
        return false;

    // 平均运动范围过滤
    if (useMinMeanMotion_ && tle.meanMotion() < minMeanMotion_)
        return false;
    if (useMaxMeanMotion_ && tle.meanMotion() > maxMeanMotion_)
        return false;

    // B* 范围过滤
    if (useMinBstar_ && tle.bstar() < minBstar_)
        return false;
    if (useMaxBstar_ && tle.bstar() > maxBstar_)
        return false;

    return true;
}

bool TLEDatabaseQuery::isEmpty() const
{
    return name_.empty()
        && !hasNoradId_
        && !useMinEpoch_ && !useMaxEpoch_
        && !hasClassification_
        && !useMinInclination_ && !useMaxInclination_
        && !useMinEccentricity_ && !useMaxEccentricity_
        && !useMinMeanMotion_ && !useMaxMeanMotion_
        && !useMinBstar_ && !useMaxBstar_;
}

AST_NAMESPACE_END

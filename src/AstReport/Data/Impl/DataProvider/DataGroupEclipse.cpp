///
/// @file      DataGroupEclipse.cpp
/// @brief     日食时刻数据组实现
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

#include "DataGroupEclipse.hpp"
#include "AstCore/EclipseEventFinder.hpp"
#include "AstUtil/VariantVector.hpp"
#include "AstUtil/Dimension.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

namespace
{

/// @brief 将单个日食事件展开为 1~3 行（半影进入 / 本影 / 半影退出），零时长段自动跳过
void buildEclipseRows(const EclipseEvent& ev, std::vector<DataGroupEclipse::Data>& rows)
{
    CelestialBody* body = ev.obstruction_.get();
    std::string obstruction = body ? body->getName() : std::string{};
    std::string worst       = ev.hasUmbra_ ? "Umbra" : "Penumbra";
    double      totalDur    = ev.penumbraStop_ - ev.penumbraStart_;

    auto push = [&](const TimePoint& s, const TimePoint& e, const char* cond)
    {
        double d = e - s;
        if (d <= 0.0) return; // 跳过零时长段（区间边界裁剪产生的退化段）
        DataGroupEclipse::Data row;
        row.startTime_       = s;
        row.stopTime_        = e;
        row.duration_        = d;
        row.obstruction_     = obstruction;
        row.currentCondition_ = cond;
        row.worstCondition_  = worst;
        row.totalDuration_   = totalDur;
        rows.push_back(row);
    };

    if (ev.hasUmbra_)
    {
        push(ev.penumbraStart_, ev.umbraStart_, "Penumbra");   // 半影进入
        push(ev.umbraStart_,    ev.umbraStop_,  "Umbra");      // 本影
        push(ev.umbraStop_,     ev.penumbraStop_, "Penumbra"); // 半影退出
    }
    else
    {
        push(ev.penumbraStart_, ev.penumbraStop_, "Penumbra"); // 掠影：单段半影
    }
}

} // anonymous namespace


DataElements DataGroupEclipse::Elements()
{
    DataElements elements;
    elements.addElement<Data, const TimePoint&, &Data::getStartTime>
        ("Start Time", Dimension::DateTime());
    elements.addElement<Data, const TimePoint&, &Data::getStopTime>
        ("Stop Time", Dimension::DateTime());
    elements.addElement<Data, double, &Data::getDuration>
        ("Duration", Dimension::Time());
    elements.addElement<Data, const std::string&, &Data::getObstruction>
        ("Obstruction");
    elements.addElement<Data, const std::string&, &Data::getCurrentCondition>
        ("Current Condition");
    elements.addElement<Data, const std::string&, &Data::getWorstCondition>
        ("Worst Condition");
    elements.addElement<Data, double, &Data::getTotalDuration>
        ("Total Duration", Dimension::Time());
    return elements;
}


errc_t DataGroupEclipse::calculate(const TimeInterval& interval, VariantVector& result) const
{
    std::vector<Data> rows;
    errc_t rc = calculate(interval, rows);
    if (rc != eNoError)
        return rc;

    result.reset<Data>(rows.size());
    if (!rows.empty())
    {
        auto span = result.asSpan<Data>();
        for (size_t i = 0; i < rows.size(); ++i)
            span[i] = rows[i];
    }
    return eNoError;
}

errc_t DataGroupEclipse::calculate(const TimeInterval& interval, std::vector<Data>& result) const
{
    auto point = this->getPoint();
    auto lightSource = this->getLightSource();
    if (!point)
    {
        aError("point is null");
        return eErrorNullPtr;
    }

    EclipseEventFinder finder;
    finder.setPoint(point);
    if (lightSource)                  finder.setLightSource(lightSource);
    if (!occultingBodies_.empty())    finder.setOccultingBodies(occultingBodies_);
    finder.setStepSize(stepSize_);

    std::vector<EclipseEvent> events;
    errc_t rc = finder.find(interval, events);
    if (rc != eNoError)
        return rc;

    result.clear();
    result.reserve(events.size() * 3);
    for (size_t p = 0; p < events.size(); ++p)
        buildEclipseRows(events[p], result);
    return eNoError;
}

const DataElements& DataGroupEclipse::getElements() const
{
    static DataElements elements = Elements();
    return elements;
}

AST_NAMESPACE_END

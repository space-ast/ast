///
/// @file      DataGroupLightingTimes.cpp
/// @brief     光照时刻数据组实现
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

#include "DataGroupLightingTimes.hpp"
#include "AstCore/EclipseEventFinder.hpp"
#include "AstUtil/VariantVector.hpp"
#include "AstUtil/Dimension.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

namespace
{

/// @brief 追加一行（零/负时长段自动跳过）
void pushRow(std::vector<DataGroupLightingTimes::Data>& rows,
             const TimePoint& s, const TimePoint& e, const std::string& obstruction)
{
    double d = e - s;
    if (d <= 0.0) return; // 跳过零时长段（区间边界裁剪产生的退化段）
    DataGroupLightingTimes::Data row;
    row.interval_.setStartStop(s, e);
    row.obstruction_ = obstruction;
    rows.push_back(row);
}

} // anonymous namespace


DataElements DataGroupLightingTimes::Elements()
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
    return elements;
}

ELightingType DataGroupLightingTimes::LightingTypeFromString(StringView type)
{
    if (type == "Penumbra") return ELightingType::ePenumbra;
    if (type == "Umbra")    return ELightingType::eUmbra;
    return ELightingType::eSunlight;
}


errc_t DataGroupLightingTimes::calculate(const TimeInterval& interval, VariantVector& result) const
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

errc_t DataGroupLightingTimes::calculate(const TimeInterval& interval, std::vector<Data>& result) const
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
    if (lightSource)               finder.setLightSource(lightSource);
    if (!occultingBodies_.empty()) finder.setOccultingBodies(occultingBodies_);
    finder.setStepSize(stepSize_);

    std::vector<EclipseEvent> events;
    errc_t rc = finder.find(interval, events);
    if (rc != eNoError)
        return rc;

    result.clear();
    switch (lightingType_)
    {
    case ELightingType::eSunlight:
    {
        // 全光照 = 各日食事件之间的空隙，裁剪到区间
        TimePoint cursor = interval.start();
        for (const auto& ev : events)
        {
            pushRow(result, cursor, ev.getPenumbraStart(), std::string{});
            cursor = ev.getPenumbraStop();
        }
        pushRow(result, cursor, interval.stop(), std::string{});
        break;
    }
    case ELightingType::ePenumbra:
    {
        for (const auto& ev : events)
        {
            std::string obstruction = ev.getObstructionName();
            if (ev.hasUmbra())
            {
                pushRow(result, ev.getPenumbraStart(), ev.getUmbraStart(), obstruction);   // 半影进入
                pushRow(result, ev.getUmbraStop(),    ev.getPenumbraStop(), obstruction);  // 半影退出
            }
            else
            {
                pushRow(result, ev.getPenumbraStart(), ev.getPenumbraStop(), obstruction); // 掠影：单段半影
            }
        }
        break;
    }
    case ELightingType::eUmbra:
    {
        for (const auto& ev : events)
        {
            if (ev.hasUmbra())
                pushRow(result, ev.getUmbraStart(), ev.getUmbraStop(), ev.getObstructionName());
        }
        break;
    }
    }
    return eNoError;
}

const DataElements& DataGroupLightingTimes::getElements() const
{
    static DataElements elements = Elements();
    return elements;
}

AST_NAMESPACE_END

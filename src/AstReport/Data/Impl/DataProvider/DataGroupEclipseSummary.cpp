///
/// @file      DataGroupEclipseSummary.cpp
/// @brief     日食摘要数据组实现
/// @author    axel
/// @date      2026-08-17
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

#include "DataGroupEclipseSummary.hpp"
#include "AstUtil/VariantVector.hpp"
#include "AstUtil/Dimension.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

DataElements DataGroupEclipseSummary::Elements()
{
    DataElements elements;
    elements.addElement<EclipseEvent, const TimePoint&, &EclipseEvent::getPenumbraStart>
        ("Penumbra Start Time", Dimension::DateTime());
    elements.addElement<EclipseEvent, const TimePoint&, &EclipseEvent::getUmbraStart>
        ("Umbra Start Time", Dimension::DateTime());
    elements.addElement<EclipseEvent, const TimePoint&, &EclipseEvent::getUmbraStop>
        ("Umbra Stop Time", Dimension::DateTime());
    elements.addElement<EclipseEvent, const TimePoint&, &EclipseEvent::getPenumbraStop>
        ("Penumbra Stop Time", Dimension::DateTime());
    elements.addElement<EclipseEvent, double, &EclipseEvent::getUmbraDuration>
        ("Umbra Duration", Dimension::Time());
    elements.addElement<EclipseEvent, double, &EclipseEvent::getPenumbraDuration>
        ("Penumbra Duration", Dimension::Time());
    elements.addElement<EclipseEvent, double, &EclipseEvent::getTotalDuration>
        ("Total Duration", Dimension::Time());
    elements.addElement<EclipseEvent, std::string, &EclipseEvent::getObstructionName>
        ("Obstruction");
    elements.addElement<EclipseEvent, double, &EclipseEvent::getMinIntensity>
        ("Min Intensity");
    elements.addElement<EclipseEvent, double, &EclipseEvent::getMaxShadowRatio>
        ("Max Percent Shadow");
    elements.addElement<EclipseEvent, const TimePoint&, &EclipseEvent::getTimeAtMinIntensity>
        ("Time at Min Intensity", Dimension::DateTime());
    return elements;
}


errc_t DataGroupEclipseSummary::calculate(const TimeInterval& interval, VariantVector& result) const
{
    std::vector<Data> events;
    errc_t rc = calculate(interval, events);
    if (rc != eNoError)
        return rc;

    result.reset<Data>(events.size());
    if (!events.empty())
    {
        auto span = result.asSpan<Data>();
        for (size_t i = 0; i < events.size(); ++i)
            span[i] = events[i];
    }
    return eNoError;
}

errc_t DataGroupEclipseSummary::calculate(const TimeInterval& interval, std::vector<Data>& result) const
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

    // Data == EclipseEvent，find() 内部会先 clear 再填充
    return finder.find(interval, result);
}

const DataElements& DataGroupEclipseSummary::getElements() const
{
    static DataElements elements = Elements();
    return elements;
}

AST_NAMESPACE_END

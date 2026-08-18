///
/// @file      DataGroupSolarIntensity.cpp
/// @brief     太阳光照强度数据组实现
/// @details   对每个时间网格点，遍历遮挡天体计算双锥模型光照比例并取最小值，
///             派生遮蔽比例（1 - intensity）、光照条件字符串与遮挡体名称。
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

#include "DataGroupSolarIntensity.hpp"
#include "AstCore/Eclipse.hpp"
#include "AstCore/RunTimeSolarSystem.hpp"
#include "AstUtil/VariantVector.hpp"
#include "AstUtil/Dimension.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

namespace
{

/// @brief 光照强度 → 光照条件字符串（简短形式，与 DataGroupEclipse 的 currentCondition 一致）
const char* conditionString(double intensity)
{
    if (intensity >= 1.0) return "Sunlight";
    if (intensity <= 0.0) return "Umbra";
    return "Penumbra";
}

} // anonymous namespace

DataElements DataGroupSolarIntensity::Elements()
{
    DataElements elements;
    elements.addElement<Data, const TimePoint&, &Data::getTime>
        ("Time", Dimension::DateTime());
    elements.addElement<Data, double, &Data::getIntensity>
        ("Intensity", Dimension::Unit());
    elements.addElement<Data, double, &Data::getShadowRatio>
        ("Percent Shadow", Dimension::Unit());
    elements.addElement<Data, const std::string&, &Data::getCurrentCondition>
        ("Current Condition");
    elements.addElement<Data, const std::string&, &Data::getObstruction>
        ("Obstruction");
    return elements;
}

errc_t DataGroupSolarIntensity::calculate(const TimeList& timeList, VariantVector& result) const
{
    size_t size = timeList.size();
    result.reset<Data>(size);
    return calculate(timeList, result.asSpan<Data>());
}

const DataElements& DataGroupSolarIntensity::getElements() const
{
    static DataElements elements = Elements();
    return elements;
}

errc_t DataGroupSolarIntensity::calculate(const TimeList& timeList, std::vector<Data>& result) const
{
    size_t size = timeList.size();
    result.resize(size);
    return calculate(timeList, Span<Data>(result));
}

errc_t DataGroupSolarIntensity::calculate(const TimeList& timeList, Span<Data> result) const
{
    Point* point = this->getPoint();
    if (!point)
    {
        aError("point is null");
        return eErrorNullPtr;
    }

    CelestialBody* lightSource = lightSource_.get();
    if (!lightSource)
    {
        lightSource = aGetSun();
        if (!lightSource)
        {
            aError("no light source");
            return eErrorNullPtr;
        }
    }

    std::vector<HCelestialBody> bodies = occultingBodies_;
    if (bodies.empty())
    {
        aWarning("no occulting bodies, using Earth and Moon as default");
        if (auto earth = aGetEarth()) bodies.push_back(earth);
        if (auto moon  = aGetMoon())  bodies.push_back(moon);
        if (bodies.empty())
        {
            aError("no occulting bodies");
            return eErrorNullPtr;
        }
    }

    const size_t size = result.size();
    if (size != timeList.size())
    {
        aError("result size must be equal to timeList size");
        return eErrorInvalidParam;
    }

    errc_t rc = eNoError;
    for (size_t i = 0; i < size; i++)
    {
        Data& data = result[i];
        data.time_ = timeList[i];

        double intensity = 1.0;
        CelestialBody* obstruction = nullptr;
        for (auto& b : bodies)
        {
            if (!b || b.get() == lightSource) continue;
            double l = aLightingRatio_DualConeModel(data.time_, point, lightSource, b.get());
            if (l < intensity)
            {
                intensity = l;
                obstruction = b.get();
            }
        }

        data.intensity_        = intensity;
        data.currentCondition_ = conditionString(intensity);
        data.obstruction_      = obstruction ? obstruction->getName() : std::string{};
    }
    return rc;
}

AST_NAMESPACE_END

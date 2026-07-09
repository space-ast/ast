///
/// @file      DataGroupEquinElem.cpp
/// @brief     春分点根数数据组实现
/// @details   非奇异轨道根数：半长轴、h、k、p、q、平经度及平均角速度
/// @author    axel
/// @date      2026-07-08
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

#include "DataGroupEquinElem.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstUtil/VariantVector.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Dimension.hpp"

AST_NAMESPACE_BEGIN

DataElements DataGroupEquinElem::Elements()
{
    DataElements elements;
    elements.addElement<Data, const TimePoint&, &Data::getTime>
    ("Time", Dimension::DateTime());
    elements.addElement<Data, double, &Data::getSemiMajorAxis>
    ("Semi-Major Axis", Dimension::Length());
    elements.addElement<Data, double, &Data::getH>
    ("e * sin(omegaBar)", Dimension::Unit());
    elements.addElement<Data, double, &Data::getK>
    ("e * cos(omegaBar)", Dimension::Unit());
    elements.addElement<Data, double, &Data::getP>
    ("tan(i/2) * sin(raan)", Dimension::Unit());
    elements.addElement<Data, double, &Data::getQ>
    ("tan(i/2) * cos(raan)", Dimension::Unit());
    elements.addElement<Data, double, &Data::getMeanLon>
    ("Mean Lon", Dimension::Angle());
    elements.addElement<Data, double, &Data::getMeanMotion>
    ("Mean Motion", Dimension::AngularVelocity());

    // 以下元素需要额外基础设施支持，暂未注册：
    // "Direction"                — DataType 2 (string), 需要字符串类型支持
    // "Semi-Major Axis Rate"     — DataType 3, 需要传播器状态
    // "e * sin(omegaBar) Rate"   — DataType 3
    // "e * cos(omegaBar) Rate"   — DataType 3
    // "tan(i/2) * sin(raan) Rate"— DataType 3
    // "tan(i/2) * cos(raan) Rate"— DataType 3
    // "Mean Lon Rate"            — DataType 3
    // "Mean Lon Perturb Rate"    — DataType 3
    return elements;
}

errc_t DataGroupEquinElem::calculate(const TimeList& timeList, VariantVector& result) const
{
    size_t size = timeList.size();
    result.reset<Data>(size);
    return calculate(timeList, result.asSpan<Data>());
}

const DataElements& DataGroupEquinElem::getElements() const
{
    static DataElements elements = Elements();
    return elements;
}

errc_t DataGroupEquinElem::calculate(const TimeList& timeList, std::vector<Data>& result) const
{
    size_t size = timeList.size();
    result.resize(size);
    return calculate(timeList, Span<Data>(result));
}

errc_t DataGroupEquinElem::calculate(const TimeList& timeList, Span<Data> result) const
{
    Point* point = this->getPoint();
    Frame* frame = this->getFrame();
    if(!point || !frame)
    {
        aError("Point or Frame is null");
        return eErrorNullPtr;
    }

    Body* body = frame->getBody();
    if(!body)
    {
        aError("Body is null");
        return eErrorNullPtr;
    }

    double gm = body->getGM();
    size_t size = result.size();
    if(size != timeList.size())
    {
        aError("result size must be equal to timeList size");
        return eErrorInvalidParam;
    }

    errc_t rc = eNoError;
    for(size_t i = 0; i < size; i++)
    {
        Data& data = result[i];
        data.gm_ = gm;
        data.time_ = timeList[i];

        Vector3d pos, vel;
        errc_t err = point->getPosVelIn(frame, data.time_, pos, vel);
        if(err != eNoError)
        {
            rc = err;
            continue;
        }
        aCartToEquinElem(pos, vel, gm, data.equinElem_);
    }
    return rc;
}


AST_NAMESPACE_END

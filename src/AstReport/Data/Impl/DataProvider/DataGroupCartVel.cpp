///
/// @file      DataGroupCartVel.cpp
/// @brief     笛卡尔速度数据组实现
/// @details   对应 CartVel 服务，输出 ICRF 坐标系下的笛卡尔速度分量、速率、径向速度和迹向速度
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

#include "DataGroupCartVel.hpp"
#include "AstUtil/VariantVector.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Dimension.hpp"

AST_NAMESPACE_BEGIN

DataElements DataGroupCartVel::Elements()
{
    DataElements elements;
    elements.addElement<Data, const TimePoint&, &Data::getTime>
    // 1. Time (独立变量)
    ("Time", Dimension::DateTime());
    // 2. vx
    elements.addElement<Data, double, &Data::getVX>
    ("vx", Dimension::Speed());
    // 3. vy
    elements.addElement<Data, double, &Data::getVY>
    ("vy", Dimension::Speed());
    // 4. vz
    elements.addElement<Data, double, &Data::getVZ>
    ("vz", Dimension::Speed());
    // 5. speed — 速率（速度大小）
    elements.addElement<Data, double, &Data::getSpeed>
    ("speed", Dimension::Speed());
    // 6. vr — 径向速度
    elements.addElement<Data, double, &Data::getRadialVel>
    ("radial", Dimension::Speed());
    // 7. vi — 迹向速度
    elements.addElement<Data, double, &Data::getInTrackVel>
    ("in-track", Dimension::Speed());

    // 添加别名
    elements.addAlias("x", "vx");
    elements.addAlias("y", "vy");
    elements.addAlias("z", "vz");
    return elements;
}

errc_t DataGroupCartVel::calculate(const TimeList& timeList, VariantVector& result) const
{
    size_t size = timeList.size();
    result.reset<Data>(size);
    return calculate(timeList, result.asSpan<Data>());
}

const DataElements& DataGroupCartVel::getElements() const
{
    static DataElements elements = Elements();
    return elements;
}

errc_t DataGroupCartVel::calculate(const TimeList& timeList, std::vector<Data>& result) const
{
    size_t size = timeList.size();
    result.resize(size);
    return calculate(timeList, Span<Data>(result));
}


errc_t DataGroupCartVel::calculate(const TimeList& timeList, Span<Data> result) const
{
    Frame* frame = this->getFrame();
    Point* point = this->getPoint();
    if(!point || !frame)
    {
        aError("Point or Frame is null");
        return eErrorNullPtr;
    }

    size_t size = result.size();
    if(size != timeList.size())
    {
        aError("result size must be equal to timeList size");
        return eErrorInvalidParam;
    }

    errc_t rc = 0;
    for(size_t i = 0; i < size; i++)
    {
        Data& data = result[i];
        data.time_ = timeList[i];
        errc_t err = point->getPosVelIn(frame, data.time_, data.pos_, data.vel_);
        if(err != eNoError)
            rc = err;
    }
    return rc;
}


AST_NAMESPACE_END

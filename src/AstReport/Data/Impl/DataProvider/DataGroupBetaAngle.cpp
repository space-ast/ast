///
/// @file      DataGroupBetaAngle.cpp
/// @brief     贝塔角数据组实现
/// @details   太阳贝塔角及月球贝塔角 — 计算天体方向矢量与轨道面的夹角
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

#include "DataGroupBetaAngle.hpp"
#include "AstUtil/VariantVector.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Dimension.hpp"

AST_NAMESPACE_BEGIN

/// @brief 计算贝塔角 — 第三体方向矢量与轨道面的夹角
/// @todo 待测试验证
/// @param pos 航天器位置矢量
/// @param vel 航天器速度矢量
/// @param thirdBodyPos 第三体位置矢量
/// @return 贝塔角 (弧度)
double aCalcBetaAngle(const Vector3d& pos, const Vector3d& vel, const Vector3d& thirdBodyPos)
{
    Vector3d h = pos.cross(vel);
    double hNorm = h.norm();
    if(hNorm < 1e-12)
        return 0.0;
    Vector3d hHat = h / hNorm;

    double thirdNorm = thirdBodyPos.norm();
    if(thirdNorm < 1e-12)
        return 0.0;
    double dot = (thirdBodyPos / thirdNorm).dot(hHat);
    if(dot > 1.0) dot = 1.0;
    else if(dot < -1.0) dot = -1.0;
    return std::asin(dot);
}

DataElements DataGroupBetaAngle::Elements()
{
    DataElements elements;
    elements.addElement<Data, const TimePoint&, &Data::getTime>
    ("Time", Dimension::DateTime());
    elements.addElement<Data, double, &Data::getBetaAngle>
    ("Beta Angle", Dimension::Angle());
    elements.addElement<Data, double, &Data::getLunarBetaAngle>
    ("Lunar Beta Angle", Dimension::Angle());
    return elements;
}

errc_t DataGroupBetaAngle::calculate(const TimeList& timeList, VariantVector& result) const
{
    size_t size = timeList.size();
    result.reset<Data>(size);
    return calculate(timeList, result.asSpan<Data>());
}

const DataElements& DataGroupBetaAngle::getElements() const
{
    static DataElements elements = Elements();
    return elements;
}

errc_t DataGroupBetaAngle::calculate(const TimeList& timeList, std::vector<Data>& result) const
{
    size_t size = timeList.size();
    result.resize(size);
    return calculate(timeList, Span<Data>(result));
}

errc_t DataGroupBetaAngle::calculate(const TimeList& timeList, Span<Data> result) const
{
    Point* scPoint = this->getPoint();
    Frame* frame = this->getFrame();
    Point* sunPoint = this->getSunPoint();
    Point* moonPoint = this->getMoonPoint();
    if(!scPoint || !frame || !sunPoint || !moonPoint)
    {
        aError("scPoint, frame, sunPoint or moonPoint is null");
        return eErrorNullPtr;
    }

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
        data.time_ = timeList[i];

        Vector3d pos, vel;
        errc_t err = scPoint->getPosVelIn(frame, data.time_, pos, vel);
        if(err != eNoError)
            { rc = err; continue; }

        Vector3d sunPos;
        err = sunPoint->getPosIn(frame, data.time_, sunPos);
        if(err != eNoError)
            { rc = err; continue; }

        Vector3d moonPos;
        err = moonPoint->getPosIn(frame, data.time_, moonPos);
        if(err != eNoError)
            { rc = err; continue; }

        data.betaAngle_      = aCalcBetaAngle(pos, vel, sunPos);
        data.lunarBetaAngle_ = aCalcBetaAngle(pos, vel, moonPos);
    }
    return rc;
}


AST_NAMESPACE_END

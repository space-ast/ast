///
/// @file      DataGroupQuats.cpp
/// @brief     四元数姿态数据组实现
/// @details   对应 Quats 服务，输出四元数分量 (q1-q4) 及角速度分量 (wx, wy, wz, wmag, RA, Dec)
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

#include "DataGroupQuats.hpp"
#include "AstMath/KinematicRotation.hpp"
#include "AstUtil/VariantVector.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Dimension.hpp"

AST_NAMESPACE_BEGIN

DataElements DataGroupQuats::Elements()
{
    DataElements elements;
    elements.addElement<Data, const TimePoint&, &Data::getTime>
    ("Time", Dimension::DateTime());
    elements.addElement<Data, double, &Data::getQ1>
    ("q1", Dimension::Unit());
    elements.addElement<Data, double, &Data::getQ2>
    ("q2", Dimension::Unit());
    elements.addElement<Data, double, &Data::getQ3>
    ("q3", Dimension::Unit());
    elements.addElement<Data, double, &Data::getQ4>
    ("q4", Dimension::Unit());
    elements.addElement<Data, double, &Data::getWX>
    ("wx", Dimension::AngularVelocity());
    elements.addElement<Data, double, &Data::getWY>
    ("wy", Dimension::AngularVelocity());
    elements.addElement<Data, double, &Data::getWZ>
    ("wz", Dimension::AngularVelocity());
    elements.addElement<Data, double, &Data::getWMagnitude>
    ("w mag", Dimension::AngularVelocity());
    elements.addElement<Data, double, &Data::getRightAscensionOfW>
    ("RightAscension of w", Dimension::Angle());
    elements.addElement<Data, double, &Data::getDeclinationOfW>
    ("Declination of w", Dimension::Angle());
    return elements;
}

errc_t DataGroupQuats::calculate(const TimeList& timeList, VariantVector& result) const
{
    size_t size = timeList.size();
    result.reset<Data>(size);
    return calculate(timeList, result.asSpan<Data>());
}

const DataElements& DataGroupQuats::getElements() const
{
    static DataElements elements = Elements();
    return elements;
}

errc_t DataGroupQuats::calculate(const TimeList& timeList, std::vector<Data>& result) const
{
    size_t size = timeList.size();
    result.resize(size);
    return calculate(timeList, Span<Data>(result));
}

errc_t DataGroupQuats::calculate(const TimeList& timeList, Span<Data> result) const
{
    Axes* axes = this->getAxes();
    Axes* referenceAxes = this->getReferenceAxes();
    if(!axes || !referenceAxes)
    {
        aError("Axes or ReferenceAxes is null");
        return eErrorNullPtr;
    }

    size_t size = result.size();
    if(size != timeList.size())
    {
        aError("result size must be equal to timeList size");
        return eErrorInvalidParam;
    }

    errc_t rc = 0;
    KinematicRotation rotation;
    for(size_t i = 0; i < size; i++)
    {
        Data& data = result[i];
        data.time_ = timeList[i];
        errc_t err = axes->getTransformFrom(referenceAxes, data.time_, rotation);
        if(err == eNoError)
        {
            data.quat_ = rotation.getQuaternion();
            data.angvel_ = rotation.getRotationRate();
        }
        else
            rc = err;
    }
    return rc;
}


AST_NAMESPACE_END

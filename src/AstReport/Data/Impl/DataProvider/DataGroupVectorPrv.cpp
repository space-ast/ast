///
/// @file      DataGroupVectorPrv.cpp
/// @brief     向量数据组 (派生量) 实现
/// @details   通用向量分量及派生量：模、方向余弦、球面角、方向角、导数
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

#include "DataGroupVectorPrv.hpp"
#include "AstUtil/VariantVector.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Dimension.hpp"

AST_NAMESPACE_BEGIN

DataElements DataGroupVectorPrv::Elements()
{
    DataElements elements;
    elements.addElement<Data, const TimePoint&, &Data::getTime>
    ("Time", Dimension::DateTime());
    elements.addElement<Data, double, &Data::getX>
    ("x", Dimension::Length());
    elements.addElement<Data, double, &Data::getY>
    ("y", Dimension::Length());
    elements.addElement<Data, double, &Data::getZ>
    ("z", Dimension::Length());
    elements.addElement<Data, double, &Data::getMagnitude>
    ("Magnitude", Dimension::Length());
    elements.addElement<Data, double, &Data::getXOverMag>
    ("x/Magnitude", Dimension::Unit());
    elements.addElement<Data, double, &Data::getYOverMag>
    ("y/Magnitude", Dimension::Unit());
    elements.addElement<Data, double, &Data::getZOverMag>
    ("z/Magnitude", Dimension::Unit());
    elements.addElement<Data, double, &Data::getRightAscension>
    ("RightAscension", Dimension::Angle());
    elements.addElement<Data, double, &Data::getDeclination>
    ("Declination", Dimension::Angle());
    elements.addElement<Data, double, &Data::getCoDeclination>
    ("Co-Declination", Dimension::Angle());
    elements.addElement<Data, double, &Data::getNegativeDeclination>
    ("NegativeDeclination", Dimension::Angle());
    elements.addElement<Data, double, &Data::getDirectionAngleX>
    ("DirectionAngle x", Dimension::Angle());
    elements.addElement<Data, double, &Data::getDirectionAngleY>
    ("DirectionAngle y", Dimension::Angle());
    elements.addElement<Data, double, &Data::getDirectionAngleZ>
    ("DirectionAngle z", Dimension::Angle());
    elements.addElement<Data, double, &Data::getDerivativeX>
    ("Derivative x", Dimension::Speed());
    elements.addElement<Data, double, &Data::getDerivativeY>
    ("Derivative y", Dimension::Speed());
    elements.addElement<Data, double, &Data::getDerivativeZ>
    ("Derivative z", Dimension::Speed());
    elements.addElement<Data, double, &Data::getDerivativeMagnitude>
    ("Derivative Magnitude", Dimension::Speed());
    return elements;
}

errc_t DataGroupVectorPrv::calculate(const TimeList& timeList, VariantVector& result) const
{
    size_t size = timeList.size();
    result.reset<Data>(size);
    return calculate(timeList, result.asSpan<Data>());
}

const DataElements& DataGroupVectorPrv::getElements() const
{
    static DataElements elements = Elements();
    return elements;
}

errc_t DataGroupVectorPrv::calculate(const TimeList& timeList, std::vector<Data>& result) const
{
    size_t size = timeList.size();
    result.resize(size);
    return calculate(timeList, Span<Data>(result));
}

errc_t DataGroupVectorPrv::calculate(const TimeList& timeList, Span<Data> result) const
{
    Vector* vector = this->getVector();
    Axes* axes = this->getAxes();
    if(!vector || !axes)
    {
        aError("Vector or Axes is null");
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
        errc_t err = vector->getVectorIn(axes, data.time_, data.vector_, data.velocity_);
        if(err != eNoError)
            rc = err;
    }
    return rc;
}


AST_NAMESPACE_END

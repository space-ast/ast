///
/// @file      DataGroupEuler.cpp
/// @brief     欧拉角姿态数据组实现
/// @details   对应 Euler 服务，输出欧拉角 (A, B, C) 及角速率 (A dot, B dot, C dot)，转序 121
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

#include "DataGroupEuler.hpp"
#include "AstMath/KinematicRotation.hpp"
#include "AstUtil/VariantVector.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Dimension.hpp"
#include <cmath>

AST_NAMESPACE_BEGIN

/// @brief 将四元数和体轴角速度转换为欧拉角及欧拉角速率
/// @param quat 四元数
/// @param angvel 体轴角速度
/// @param seq 欧拉角转序
/// @param angles 输出欧拉角
/// @param rates 输出欧拉角速率（万向节死锁时返回零）
void aQuatAngvelToEuler(const Quaternion& quat, const Vector3d& angvel, int rotationOrder,
                         Euler& angles, Euler& rates)
{
    angles.fromQuat(quat, rotationOrder);

    double A = angles.angle1();
    double B = angles.angle2();
    double sinA = std::sin(A), cosA = std::cos(A);
    double sinB = std::sin(B);

    if(std::fabs(sinB) < 1e-12)
    {
        rates.angle1() = 0.0;
        rates.angle2() = 0.0;
        rates.angle3() = 0.0;
        return;
    }

    double cotB = std::cos(B) / sinB;
    double cscB = 1.0 / sinB;
    rates.angle1() = angvel.x() - sinA * cotB * angvel.y() - cosA * cotB * angvel.z();
    rates.angle2() = cosA * angvel.y() - sinA * angvel.z();
    rates.angle3() = sinA * cscB * angvel.y() + cosA * cscB * angvel.z();
}


DataElements DataGroupEuler::Elements()
{
    DataElements elements;
    elements.addElement<Data, const TimePoint&, &Data::getTime>
    ("Time", Dimension::DateTime());
    elements.addElement<Data, double, &Data::getA>
    ("A", Dimension::Angle());
    elements.addElement<Data, double, &Data::getB>
    ("B", Dimension::Angle());
    elements.addElement<Data, double, &Data::getC>
    ("C", Dimension::Angle());
    elements.addElement<Data, double, &Data::getADot>
    ("A dot", Dimension::AngularVelocity());
    elements.addElement<Data, double, &Data::getBDot>
    ("B dot", Dimension::AngularVelocity());
    elements.addElement<Data, double, &Data::getCDot>
    ("C dot", Dimension::AngularVelocity());
    return elements;
}

errc_t DataGroupEuler::calculate(const TimeList& timeList, VariantVector& result) const
{
    size_t size = timeList.size();
    result.reset<Data>(size);
    return calculate(timeList, result.asSpan<Data>());
}

const DataElements& DataGroupEuler::getElements() const
{
    static DataElements elements = Elements();
    return elements;
}

errc_t DataGroupEuler::calculate(const TimeList& timeList, std::vector<Data>& result) const
{
    size_t size = timeList.size();
    result.resize(size);
    return calculate(timeList, Span<Data>(result));
}

errc_t DataGroupEuler::calculate(const TimeList& timeList, Span<Data> result) const
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

    errc_t rc = eNoError;
    KinematicRotation rotation;
    for(size_t i = 0; i < size; i++)
    {
        Data& data = result[i];
        data.time_ = timeList[i];
        errc_t err = axes->getTransformFrom(referenceAxes, data.time_, rotation);
        if(err != eNoError)
        {
            rc = err;
            continue;
        }

        aQuatAngvelToEuler(rotation.getQuaternion(), rotation.getRotationRate(),
                            rotationOrder_, data.angles_, data.rates_);
    }
    return rc;
}


AST_NAMESPACE_END

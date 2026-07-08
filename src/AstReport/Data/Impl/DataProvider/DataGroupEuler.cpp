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

namespace
{
    /// @brief 绕指定轴旋转向量的分量
    /// @param v   输入向量
    /// @param axis 旋转轴 (0=X, 1=Y, 2=Z)
    /// @param c   cos(角度)
    /// @param s   sin(角度)
    Vector3d rotateVector(const Vector3d& v, int axis, double c, double s)
    {
        switch (axis)
        {
            case 0: return {v.x(), c * v.y() - s * v.z(), s * v.y() + c * v.z()};
            case 1: return {c * v.x() + s * v.z(), v.y(), -s * v.x() + c * v.z()};
            case 2: return {c * v.x() - s * v.y(), s * v.x() + c * v.y(), v.z()};
            default: return v;
        }
    }

    /// @brief 欧拉角转序分解为三个轴序号 (0=X, 1=Y, 2=Z)
    inline void decomposeOrder(int order, int& i, int& j, int& k)
    {
        i = (order / 100) % 10 - 1;
        j = (order / 10)  % 10 - 1;
        k =  order        % 10 - 1;
    }
} // namespace


/// @brief 将四元数和体轴角速度转换为欧拉角及欧拉角速率
/// @details 通过构建运动学矩阵 M = [c1|c2|c3] 并求逆，支持全部 12 种转序。
///          symmetry (i==k) 时 det = sin(θ₂)，TaitBryan (i≠k) 时 det = cos(θ₂)。
/// @todo 待测试验证
/// @param quat 四元数
/// @param angvel 体轴角速度
/// @param rotationOrder 欧拉角转序 (如 Euler::eZYX, Euler::eXYX)
/// @param angles 输出欧拉角
/// @param rates 输出欧拉角速率（死锁时返回零）
void aQuatAngvelToEuler(const Quaternion& quat, const Vector3d& angvel, int rotationOrder,
                         Euler& angles, Euler& rates)
{
    angles.fromQuat(quat, rotationOrder);

    int i, j, k;
    decomposeOrder(rotationOrder, i, j, k);
    double a = angles.angle1();
    double b = angles.angle2();
    double ca = std::cos(a), sa = std::sin(a);
    double cb = std::cos(b), sb = std::sin(b);

    // 构建运动学矩阵 M = [c1 | c2 | c3]
    // c₁ = eᵢ
    Vector3d e{}; 
    e.data()[i] = 1.0;
    Vector3d c1 = e;  
    e.data()[i] = 0.0;
    // c₂ = Rᵢ(α) · eⱼ
    e.data()[j] = 1.0;
    Vector3d c2 = rotateVector(e, i, ca, sa);
    e.data()[j] = 0.0;
    // c₃ = Rᵢ(α) · Rⱼ(β) · eₖ
    e.data()[k] = 1.0;
    Vector3d tmp = rotateVector(e, j, cb, sb);
    Vector3d c3  = rotateVector(tmp, i, ca, sa);

    // det(M)
    double det = c1.x() * (c2.y() * c3.z() - c2.z() * c3.y())
               + c1.y() * (c2.z() * c3.x() - c2.x() * c3.z())
               + c1.z() * (c2.x() * c3.y() - c2.y() * c3.x());

    if (std::fabs(det) < 1e-12)
    {
        rates.angle1() = 0.0;
        rates.angle2() = 0.0;
        rates.angle3() = 0.0;
        return;
    }

    // M⁻¹ 的第 r 行 = (c_{r+1} × c_{r+2})^T / det  (索引回绕)
    double invDet = 1.0 / det;
    rates.angle1() = c2.cross(c3).dot(angvel) * invDet;
    rates.angle2() = c3.cross(c1).dot(angvel) * invDet;
    rates.angle3() = c1.cross(c2).dot(angvel) * invDet;
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

///
/// @file      Axes.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-04
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "Axes.hpp"
#include "Frame.hpp"
#include "AstUtil/Logger.hpp"
#include "AstMath/Rotation.hpp"
#include "AstMath/AttitudeUtil.hpp"
#include "AstMath/KinematicRotation.hpp"
#include "AstMath/AccelerationRotation.hpp"
#include "AstMath/KinematicTransform.hpp"
#include "AstMath/AccelerationTransform.hpp"
#include <cmath>
#include <limits>
#include <cassert>

AST_NAMESPACE_BEGIN

template<typename GeometryType>
int aGeometryDepth(const GeometryType* geometry)
{
    int depth = 0;
    while (geometry != nullptr)
    {
        depth++;
        geometry = geometry->getParent();
    }
    return depth;
}

template<typename GeometryType>
GeometryType* aGeometryAncestor(const GeometryType* geometry, int depth)
{
    for (int i = 0; i < depth; i++)
    {
        if(geometry == nullptr)
            return nullptr;
        geometry = geometry->getParent();
    }
    return const_cast<GeometryType*>(geometry);
}

int Axes::getDepth() const
{
    return aGeometryDepth(this);
}

Axes* Axes::getAncestor(int depth) const{
    
    return aGeometryAncestor(this, depth);
}

errc_t Axes::getAttitudeIn(Axes &axes, const TimePoint &tp, Quaternion &quat) const
{
    Rotation rotation;
    errc_t rc = this->getTransformFrom(axes, tp, rotation);
    quat = rotation.getQuaternion();
    return rc;
}

errc_t Axes::getAttitudeIn(Axes &axes, const TimePoint &tp, Quaternion &quat, Vector3d &angvel) const
{
    KinematicRotation kr;
    errc_t rc = this->getTransformFrom(axes, tp, kr);
    quat = kr.quaternion();
    angvel = kr.getRotationRate();
    return rc;
}

/// @brief 用差分求角加速度时的步长 [s]
/// @details 与 aAxesRotationRateByDifference 的默认步长保持一致；
///          角加速度是角速度的差分，步长过小会放大舍入误差，过大则增大截断误差。
static constexpr double kAxesAccelerationDiffStep = 0.05;

errc_t Axes::getTransform(const TimePoint &tp, AccelerationRotation &rotation) const
{
    /*!
        默认实现：先由具体轴系给出本轴系相对父轴系的运动学旋转变换(旋转矩阵 + 角速度)，
        再对两端时刻的角速度做中心差分(端点处退化为单侧差分)得到角加速度。

        按 AccelerationRotation 的约定，角加速度是角速度的分量在源轴系(父轴系)下的
        坐标时间导数，因此这里直接对 getTransform 返回的角速度分量做差商即可，
        不需要额外补偿父轴系自身的转动——父轴系转动引入的输运项由组合变换
        AccelerationRotation::composed 统一处理。

        这是给没有解析角加速度的轴系兜底用的默认实现，有解析解的轴系应当重写本函数。
    */
    KinematicRotation kr0;
    errc_t rc = this->getTransform(tp, kr0);
    if (A_UNLIKELY(rc != eNoError))
        return rc;

    const double h = kAxesAccelerationDiffStep;
    const TimePoint tpPlus  = tp + h;
    const TimePoint tpMinus = tp - h;

    KinematicRotation krPlus, krMinus;
    const errc_t rcPlus  = this->getTransform(tpPlus, krPlus);
    const errc_t rcMinus = this->getTransform(tpMinus, krMinus);

    Vector3d angvelDot;
    if (rcPlus == eNoError && rcMinus == eNoError)
    {
        // 中心差分
        angvelDot = (krPlus.rotationRate() - krMinus.rotationRate()) / (2.0 * h);
    }
    else if (rcPlus == eNoError)
    {
        // 前向差分(时间点靠近可用数据的起点)
        angvelDot = (krPlus.rotationRate() - kr0.rotationRate()) / h;
    }
    else if (rcMinus == eNoError)
    {
        // 后向差分(时间点靠近可用数据的终点)
        angvelDot = (kr0.rotationRate() - krMinus.rotationRate()) / h;
    }
    else
    {
        // 两侧都取不到，返回原始错误码，而不是悄悄给一个零角加速度
        return rcPlus;
    }

    rotation = AccelerationRotation(kr0, angvelDot);
    return eNoError;
}

template<typename GeometryType, typename RotationType>
A_ALWAYS_INLINE errc_t aGeometryTransform(GeometryType& source, GeometryType& target, const TimePoint& tp, RotationType &rotation)
{
    // if (A_UNLIKELY(source == nullptr || target == nullptr))
    // {
    //     return eErrorNullInput;
    // }
    
    /*!
        计算出源坐标系和目标坐标系的深度，
        同时分别填充源坐标系路径和目标坐标系路径。
        最后从源坐标系路径和目标坐标系路径的末尾开始比较，
        找到第一个相同的坐标系，
        则该坐标系为最近公共祖先

        假设坐标系的深度不会超过256层，
        因此使用了一个256层的栈数组来存储源坐标系路径和目标坐标系路径。
        为了避免缓冲区溢出，我们使用uint8_t类型来存储深度
        
        @todo
        在寻找最近公共祖先时，如果出现了超过256层的意外情况（非常非常罕见），
        需要进入动态分配内存的计算模式
    */

    uint8_t sourceDepth = 0;                 // 源坐标系深度
    uint8_t targetDepth = 0;                 // 目标坐标系深度
    GeometryType* sourcePath[256];           // 源坐标系路径
    GeometryType* targetPath[256];           // 目标坐标系路径
    // 1. 填充源坐标系路径和目标坐标系路径
    {
        // 填充源坐标系路径
        GeometryType* current = &source;
        do
        {
            sourcePath[sourceDepth++] = current;
            current = current->getParent();
        }while (current != nullptr);
        // 填充目标坐标系路径
        current = &target;
        do
        {
            targetPath[targetDepth++] = current;
            current = current->getParent();
        }while (current != nullptr);
    }
    // 2. 寻找最近的公共祖先
    {
        if(sourcePath[sourceDepth - 1] != targetPath[targetDepth - 1])
        {
            aWarning(_("查找公共祖先失败"));
            return eErrorInvalidParam;
        }
        if(sourceDepth > targetDepth){
            sourceDepth = sourceDepth - targetDepth;
            targetDepth = 0;
        }else{
            targetDepth = targetDepth - sourceDepth;
            sourceDepth = 0;
        }
        while(sourcePath[sourceDepth] != targetPath[targetDepth]){
            sourceDepth++;
            targetDepth++;
        }
    }
    // 3. 计算旋转变换
    {
        RotationType commonToSource = RotationType::Identity();
        for (int i = 0; i < sourceDepth; i++)
        {
            RotationType tempRot;
            errc_t rc = sourcePath[i]->getTransform(tp, tempRot);
            if(A_UNLIKELY(rc != eNoError))
                return rc;
            commonToSource = tempRot * commonToSource;
        }
        RotationType commonToTarget = RotationType::Identity();
        for (int i = 0; i < targetDepth; i++)
        {
            RotationType tempRot;
            errc_t rc = targetPath[i]->getTransform(tp, tempRot);
            if(A_UNLIKELY(rc != eNoError))
                return rc;
            commonToTarget = tempRot * commonToTarget;
        }
        /// @todo 这里应该可以专门写个函数将 `.inverse() *` 的计算进行合并 
        rotation = commonToSource.inverse() * commonToTarget;
    }
    return eNoError;
}

errc_t aFrameTransform(Frame &source, Frame &target, const TimePoint &tp, Transform &transform)
{
    auto sourcePoint = source.getOrigin();
    auto targetPoint = target.getOrigin();
    if(sourcePoint == targetPoint){
        transform.setTranslation(Vector3d::Zero());
        return aAxesTransform(source.getAxes(), target.getAxes(), tp, transform.getRotation());
    }
    return aGeometryTransform<Frame, Transform>(source, target, tp, transform);
}

errc_t aFrameTransform(Frame *source, Frame *target, const TimePoint &tp, Transform &transform)
{
    if (A_UNLIKELY(source == nullptr || target == nullptr))
        return eErrorNullInput;
    return aFrameTransform(*source, *target, tp, transform);
}

errc_t aFrameTransform(Frame &source, Frame &target, const TimePoint &tp, KinematicTransform &transform)
{
    auto sourcePoint = source.getOrigin();
    auto targetPoint = target.getOrigin();
    if(sourcePoint == targetPoint){
        transform.setTranslation(Vector3d::Zero());
        transform.setVelocity(Vector3d::Zero());
        return aAxesTransform(source.getAxes(), target.getAxes(), tp, transform.getKinematicRotation());
    }
    return aGeometryTransform<Frame, KinematicTransform>(source, target, tp, transform);
}

errc_t aFrameTransform(Frame *source, Frame *target, const TimePoint &tp, KinematicTransform &transform)
{
    if (A_UNLIKELY(source == nullptr || target == nullptr))
        return eErrorNullInput;
    return aFrameTransform(*source, *target, tp, transform);
}

errc_t aFrameTransform(Frame &source, Frame &target, const TimePoint &tp, AccelerationTransform &transform)
{
    auto sourcePoint = source.getOrigin();
    auto targetPoint = target.getOrigin();
    if(sourcePoint == targetPoint){
        transform.setTranslation(Vector3d::Zero());
        transform.setVelocity(Vector3d::Zero());
        transform.setAcceleration(Vector3d::Zero());
        return aAxesTransform(source.getAxes(), target.getAxes(), tp, transform.getAccelerationRotation());
    }
    return aGeometryTransform<Frame, AccelerationTransform>(source, target, tp, transform);
}

errc_t aFrameTransform(Frame *source, Frame *target, const TimePoint &tp, AccelerationTransform &transform)
{
    if (A_UNLIKELY(source == nullptr || target == nullptr))
        return eErrorNullInput;
    return aFrameTransform(*source, *target, tp, transform);
}

errc_t aAxesTransform(Axes &source, Axes &target, const TimePoint &tp, Rotation &rotation)
{
    return aGeometryTransform<Axes, Rotation>(source, target, tp, rotation);
}

errc_t aAxesTransform(Axes *source, Axes *target, const TimePoint &tp, Rotation &rotation)
{
    if (A_UNLIKELY(source == nullptr || target == nullptr))
        return eErrorNullInput;
    return aAxesTransform(*source, *target, tp, rotation);
}

errc_t aAxesTransform(Axes &source, Axes &target, const TimePoint &tp, KinematicRotation &rotation)
{
    return aGeometryTransform<Axes, KinematicRotation>(source, target, tp, rotation);
}

errc_t aAxesTransform(Axes *source, Axes *target, const TimePoint &tp, KinematicRotation &rotation)
{
    if (A_UNLIKELY(source == nullptr || target == nullptr))
        return eErrorNullInput;
    return aAxesTransform(*source, *target, tp, rotation);
}

errc_t aAxesTransform(Axes &source, Axes &target, const TimePoint &tp, AccelerationRotation &rotation)
{
    return aGeometryTransform<Axes, AccelerationRotation>(source, target, tp, rotation);
}

errc_t aAxesTransform(Axes *source, Axes *target, const TimePoint &tp, AccelerationRotation &rotation)
{
    if (A_UNLIKELY(source == nullptr || target == nullptr))
        return eErrorNullInput;
    return aAxesTransform(*source, *target, tp, rotation);
}

errc_t aAxesTransform(Axes &source, Axes &target, const TimePoint &tp, Matrix3d &matrix)
{
    return aAxesTransform(source, target, tp, Rotation::CastFrom(matrix));
}

errc_t aAxesTransform(Axes *source, Axes *target, const TimePoint &tp, Matrix3d &matrix)
{
    return aAxesTransform(source, target, tp, Rotation::CastFrom(matrix));
}

errc_t aAxesRotationRateByDifference(Axes &axes, Axes &referenceAxes, const TimePoint &tp, Vector3d &angvel, double h)
{
    if (A_UNLIKELY(h == 0.0))
        return eErrorInvalidParam;

    const TimePoint tpPlus  = tp + h;
    const TimePoint tpMinus = tp - h;

    Rotation rotPlus, rotMinus;
    const errc_t rcPlus  = aAxesTransform(referenceAxes, axes, tpPlus, rotPlus);
    const errc_t rcMinus = aAxesTransform(referenceAxes, axes, tpMinus, rotMinus);

    if (rcPlus == eNoError && rcMinus == eNoError)
    {
        // 中心差分
        aQuatAverageAngularVelocity(rotMinus.getQuaternion(), rotPlus.getQuaternion(), 2 * h, angvel);
    }
    else
    {
        Rotation rot0;
        const errc_t rc0 = aAxesTransform(referenceAxes, axes, tp, rot0);
        if(rc0 != eNoError)
        {
            return rc0;
        }
        else if (rcPlus == eNoError)
        {
            // 前向差分
            aQuatAverageAngularVelocity(rot0.getQuaternion(), rotPlus.getQuaternion(), h, angvel);
        }
        else if (rcMinus == eNoError)
        {
            // 后向差分
            aQuatAverageAngularVelocity(rotMinus.getQuaternion(), rot0.getQuaternion(), h, angvel);
        }
        else
        {
            return rcPlus;
        }
    } 
    return eNoError;
}

AST_NAMESPACE_END


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
#include "AstMath/Rotation.hpp"
#include "AstMath/KinematicRotation.hpp"
#include "AstMath/KinematicTransform.hpp"
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

errc_t aAxesTransform(Axes &source, Axes &target, const TimePoint &tp, Matrix3d &matrix)
{
    return aAxesTransform(source, target, tp, Rotation::CastFrom(matrix));
}

errc_t aAxesTransform(Axes *source, Axes *target, const TimePoint &tp, Matrix3d &matrix)
{
    return aAxesTransform(source, target, tp, Rotation::CastFrom(matrix));
}

AST_NAMESPACE_END


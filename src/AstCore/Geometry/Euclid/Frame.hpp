///
/// @file      Frame.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-22
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

#pragma once

#include "AstGlobal.h"
#include "AstUtil/Object.hpp"
#include "AstUtil/ObjectNamed.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Geometry
    @{
*/

class Transform;
class KinematicTransform;
class Frame;
class Axes;
class Point;

/// @brief 计算源坐标系到目标坐标系的变换，不考虑速度项
/// @param source 源坐标系
/// @param target 目标坐标系
/// @param tp 时间点
/// @param transform 输出参数，变换
/// @return 错误码
AST_CORE_API errc_t aFrameTransform(Frame* source, Frame* target, const TimePoint& tp, Transform& transform);

/// @brief 计算源坐标系到目标坐标系的运动学变换，用于需要速度变换的场景（如将速度向量从一个坐标系转换到另一个坐标系）
/// @param source 源坐标系
/// @param target 目标坐标系
/// @param tp 时间点
/// @param transform 输出参数，运动学变换（包含速度项）
/// @return 错误码
AST_CORE_API errc_t aFrameTransform(Frame* source, Frame* target, const TimePoint& tp, KinematicTransform& transform);


/// @brief 坐标系抽象基类
/// @details 坐标系是一种几何元素，由原点（Point）和轴系（Axes）组成，
///          定义了三维空间中的一个完整参考系（同时包含平移和旋转信息）。
///          所有具体坐标系类型（ICRF、地固系、当地水平系等）均派生自此类。
class AST_CORE_API Frame: public ObjectNamed
{
public:
    AST_OBJECT(Frame)

    /// @brief 解析坐标系
    /// @details 根据名称或路径字符串查找已注册的坐标系实例。
    /// @param value 坐标系的名称或路径
    /// @return 坐标系指针，未找到时返回 nullptr
    static Frame* Resolve(StringView value);

    Frame() = default;
    ~Frame() override= default;

    /// @brief 获取当前坐标系的路径表示
    /// @details 沿父坐标系链向上组合，返回完整的路径字符串，如 "Earth/ITRF"。
    /// @return 路径字符串
    std::string getRepresentation() const override;

    /// @brief 获取当前坐标系中心对应的天体
    /// @details 如果当前坐标系的原点是天体，则返回该天体；否则返回 nullptr。
    /// @return 天体
    CelestialBody* getBody();

    /// @brief 获取当前坐标系中心对应天体的引力参数
    /// @details 通过 getBody() 查找天体后返回其 GM 值。
    /// @return 引力参数 GM（km³/s²）
    double getGM();

    /// @brief 获取当前坐标系的父坐标系
    /// @return 父坐标系指针，根坐标系返回 nullptr
    virtual Frame* getParent() const;
    
    /// @brief 获取当前坐标系相对于父坐标系的变换
    /// @param tp 时间点
    /// @param transform 输出参数，变换
    /// @return 错误码
    virtual errc_t getTransform(const TimePoint& tp, Transform& transform) const;
    
    /// @brief 获取当前坐标系相对于父坐标系的运动学变换
    /// @param tp 时间点
    /// @param transform 输出参数，运动学变换（包含速度项）
    /// @return 错误码
    virtual errc_t getTransform(const TimePoint& tp, KinematicTransform& transform) const;
    
    /// @brief 获取当前坐标系的轴系
    /// @details 轴系定义了坐标轴的旋转方向。纯虚方法，子类必须实现。
    /// @return 轴系指针
    virtual Axes* getAxes() const = 0;
    
    /// @brief 获取当前坐标系的原点
    /// @details 原点定义了坐标系的平移位置。纯虚方法，子类必须实现。
    /// @return 原点指针
    virtual Point* getOrigin() const = 0;
public:
    /// @brief 获取当前坐标系到目标坐标系的变换。
    /// @param target 目标坐标系
    /// @param tp 时间点
    /// @param transform 输出的变换
    /// @return 错误码
    errc_t getTransformTo(Frame* target, const TimePoint& tp, Transform& transform) const
    {
        return aFrameTransform(const_cast<Frame*>(this), target, tp, transform);
    }
    
    /// @brief 获取当前坐标系到目标坐标系的运动学变换。
    /// @param target 目标坐标系
    /// @param tp 时间点
    /// @param transform 输出的运动学变换
    /// @return 错误码
    errc_t getTransformTo(Frame* target, const TimePoint& tp, KinematicTransform& transform) const
    {
        return aFrameTransform(const_cast<Frame*>(this), target, tp, transform);
    }
    
    /// @brief 获取源坐标系到当前坐标系的变换。
    /// @param source 源坐标系
    /// @param tp 时间点
    /// @param transform 输出的变换
    /// @return 错误码
    errc_t getTransformFrom(Frame* source, const TimePoint& tp, Transform& transform) const
    {
        return aFrameTransform(source, const_cast<Frame*>(this), tp, transform);
    }
    
    /// @brief 获取源坐标系到当前坐标系的运动学变换。
    /// @param source 源坐标系
    /// @param tp 时间点
    /// @param transform 输出的运动学变换
    /// @return 错误码
    errc_t getTransformFrom(Frame* source, const TimePoint& tp, KinematicTransform& transform) const
    {
        return aFrameTransform(source, const_cast<Frame*>(this), tp, transform);
    }
};

using PFrame = Frame*;                ///< 坐标系指针(pointer to frame)
using HFrame = SharedPtr<Frame>;      ///< 坐标系句柄(handle to frame)

/*! @} */

AST_NAMESPACE_END

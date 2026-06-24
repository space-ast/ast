///
/// @file      AxesMacro.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-05
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
#include "AstUtil/TypeTraits.hpp"
#include "AstMath/KinematicRotation.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Coordinate
    @{
*/

class KinematicRotation;
class Rotation;


/// @brief 声明轴系类
/// @param NAME 轴系名称
#define _AST_DECL_AXES(NAME) \
    class AST_CORE_API Axes##NAME : public Axes \
    { \
    public: \
        Axes##NAME() = default; \
        ~Axes##NAME() override = default; \
        static Axes##NAME* Instance(); \
        Axes* getParent() const override; \
        errc_t getTransform(const TimePoint& tp, Rotation& rotation) const override; \
        errc_t getTransform(const TimePoint& tp, KinematicRotation& rotation) const override; \
    };\
    A_ALWAYS_INLINE Axes* aAxes##NAME()\
    {\
        return Axes##NAME::Instance();\
    }\

/// @brief 实现轴系类
/// @param NAME 轴系名称
/// @param PARENT 父轴系名称
#define _AST_IMPL_AXES_BASE(NAME, PARENT)\
    Axes##NAME* Axes##NAME::Instance() \
    { \
        static SharedPtr<Axes##NAME> instance(new Axes##NAME()); \
        return instance.get(); \
    }\
    Axes *Axes##NAME::getParent() const\
    {\
        return Axes##PARENT::Instance();\
    }\

/// @brief 定义轴系之间的动态变换关系
/// 动态变换关系是指：轴系之间的旋转关系随时间变化，存在相对旋转速度
/// @param NAME 轴系名称
/// @param PARENT 父轴系名称
#define _AST_IMPL_AXES_DYNAMIC(NAME, PARENT) \
    _AST_IMPL_AXES_BASE(NAME, PARENT)\
    errc_t Axes##NAME::getTransform(const TimePoint &tp, Rotation &rotation) const\
    {\
        a##PARENT##To##NAME##Transform(tp, rotation);\
        return eNoError;\
    }\
    errc_t Axes##NAME::getTransform(const TimePoint &tp, KinematicRotation &rotation) const\
    {\
        a##PARENT##To##NAME##Transform(tp, rotation);\
        return eNoError;\
    }\

/// @brief 定义轴系之间的静态变换关系
/// 静态变换关系是指：轴系之间的旋转关系与时间无关，其相对旋转速度为0
/// @param NAME 轴系名称
/// @param PARENT 父轴系名称
#define _AST_IMPL_AXES_STATIC(NAME, PARENT) \
    _AST_IMPL_AXES_BASE(NAME, PARENT)\
    errc_t Axes##NAME::getTransform(const TimePoint &tp, Rotation &rotation) const\
    {\
        a##PARENT##To##NAME##Transform(rotation);\
        return eNoError;\
    }\
    errc_t Axes##NAME::getTransform(const TimePoint &tp, KinematicRotation &rotation) const\
    {\
        a##PARENT##To##NAME##Transform(rotation.getRotation());\
        rotation.setRotationRate(Vector3d::Zero());\
        return eNoError;\
    }\

/// @brief 定义伪静态变换关系
/// 伪静态变换关系是指：轴系之间的旋转关系随时间变化，但是变化非常缓慢，故认为其相对旋转速度为0
/// @param NAME 轴系名称
/// @param PARENT 父轴系名称
#define _AST_IMPL_AXES_PSEUDO_STATIC(NAME, PARENT) \
    _AST_IMPL_AXES_BASE(NAME, PARENT)\
    errc_t Axes##NAME::getTransform(const TimePoint &tp, Rotation &rotation) const\
    {\
        a##PARENT##To##NAME##Transform(tp, rotation);\
        return eNoError;\
    }\
    errc_t Axes##NAME::getTransform(const TimePoint &tp, KinematicRotation &rotation) const\
    {\
        a##PARENT##To##NAME##Transform(tp, rotation.getRotation());\
        rotation.setRotationRate(Vector3d::Zero());\
        return eNoError;\
    }\


/*! @} */

AST_NAMESPACE_END

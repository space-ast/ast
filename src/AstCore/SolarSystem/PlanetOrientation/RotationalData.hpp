///
/// @file      RotationalData.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-20
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
#include "RotationElement.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstMath/Transform.hpp"
#include "AstMath/Rotation.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup SolarSystem
    @{
*/


/// @brief 行星旋转数据
/// @details 描述行星旋转状态的系数
class AST_CORE_API RotationalData
{
public:
    RotationalData() = default;
    ~RotationalData() = default;
    err_t load(StringView filepath);

    /// @brief 获取ICRF到天体固连系的旋转矩阵
    /// @param tp 时间点
    /// @param matrix 矩阵
    void getICRFToFixedMatrix(const TimePoint& tp, Matrix3d &matrix) const;

    /// @brief 获取ICRF到天体固连系的旋转
    /// @param tp 时间点
    /// @param rot 旋转
    void getICRFToFixedTransform(const TimePoint& tp, Rotation &rot) const;

    /// @brief 获取ICRF到天体惯性系的旋转矩阵
    /// @param tp 时间点
    /// @param matrix 矩阵
    void getICRFToInertialMatrix(const TimePoint& tp, Matrix3d &matrix) const;

    /// @brief 获取ICRF到天体惯性系的旋转
    /// @param tp 时间点
    /// @param rot 旋转
    void getICRFToInertialTransform(const TimePoint& tp, Rotation &rot) const;

    /// @brief 获取ICRF到TOD系的旋转矩阵
    /// @param tp 时间点
    /// @param matrix 矩阵
    void getICRFToTODMatrix(const TimePoint& tp, Matrix3d &matrix) const;

    /// @brief 获取ICRF到TOD系的旋转
    /// @param tp 时间点
    /// @param rot 旋转
    void getICRFToTODTransform(const TimePoint& tp, Rotation &rot) const;


    /// @brief 获取ICRF到MOD系的旋转矩阵
    /// @param tp 时间点
    /// @param matrix 矩阵
    void getICRFToMODMatrix(const TimePoint& tp, Matrix3d &matrix) const;

    /// @brief 获取ICRF到MOD系的旋转    
    /// @param tp 时间点
    /// @param rot 旋转
    void getICRFToMODTransform(const TimePoint& tp, Rotation &rot) const;

protected:
    TimePoint rotationEpoch_;           ///< 旋转历元
    RotationElement rightAscension_;
    RotationElement declination_;
    RotationElement rotation_;
};

/*! @} */

AST_NAMESPACE_END

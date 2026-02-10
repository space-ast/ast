///
/// @file      AngleAxis.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-08
/// @copyright 版权所有 (C) 2025-present, ast项目.
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
#include "AstMath/Vector.hpp"
#include "AstMath/Matrix.hpp"
#include "AstMath/AttitudeConvert.hpp"

AST_NAMESPACE_BEGIN


/// @brief 轴角类
/// @details 轴角类表示三维空间的旋转，由旋转轴和旋转角度组成。
/// @ingroup Attitude
class AngleAxis
{
public:
    /// @brief 轴角类默认构造函数，这里不进行任何初始化，也不初始化为0
    AngleAxis(){};

    /// @brief 轴角类构造函数
    /// @param axis 旋转轴
    /// @param angle 旋转角度（弧度）
    AngleAxis(double angle, const Vector3d& axis)
    : axis_(axis), angle_(angle){}

    /// @brief 获取旋转角度（弧度）
    /// @return 旋转角度（弧度）
    double angle() const { return angle_; }
    double& angle() { return angle_; }

    /// @brief 获取旋转轴
    /// @return 旋转轴
    const Vector3d& axis() const { return axis_; }
    Vector3d& axis() { return axis_; }


public:
    /// @brief 获取轴角的逆旋转
    /// @return 轴角的逆旋转
    AngleAxis inverse() const { return AngleAxis(-angle_, axis_); }

public:

    /// @brief 从旋转矩阵构造轴角
    /// @param mtx 旋转矩阵
    /// @return 轴角
    AngleAxis& fromRotationMatrix(const Matrix3d& mtx){
        aMatrixToAngleAxis(mtx, *this);
        return *this;
    }

    /// @brief 轴角转旋转矩阵
    /// @return 旋转矩阵
    Matrix3d toRotationMatrix() const{
        Matrix3d mtx;
        aAngleAxisToMatrix(*this, mtx);
        return mtx;
    }

public:
    Vector3d axis_;     ///< 旋转轴
    double angle_;      ///< 旋转角度（弧度）
};





AST_NAMESPACE_END
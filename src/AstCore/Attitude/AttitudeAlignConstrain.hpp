///
/// @file      AttitudeAlignConstrain.hpp
/// @brief     对齐/约束姿态(Aligned and Constrained)
/// @details   STK 中大多数预定义姿态剖面都是由"一对对齐向量 + 一对约束向量"生成的，
///            本文件提供该机制的求解引擎(aAlignConstrainRotation)以及通用剖面
///            AttitudeAlignConstrain，具体剖面(ECIVVLH、ECFVelRadial 等)都建立在其之上。
/// @author    axel
/// @date      2026-09-28
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
#include "AstCore/AttitudeProfile.hpp"
#include "AstMath/Rotation.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Attitude
    @{
*/

/// @brief 体轴标记
/// @note 取值与 aRotationXMatrix 等函数中的 axis 参数保持一致(1=X, 2=Y, 3=Z)
enum class EAttitudeAxis
{
    eX = 1,     ///< 体 X 轴
    eY = 2,     ///< 体 Y 轴
    eZ = 3,     ///< 体 Z 轴
};

/// @brief 姿态偏移(offset)的旋向
enum class EAttitudeOffsetSense
{
    eRightHanded,   ///< 右手旋向(逆着轴看为逆时针)
    eLeftHanded,    ///< 左手旋向(逆着轴看为顺时针)
};

/// @brief 计算对齐/约束姿态对应的旋转变换
/// @details 构造方式：
///          1. 由对齐向量与约束向量在参考坐标系下构造正交三轴 (f1,f2,f3)，
///             其中 f1 为对齐方向，f2 为约束方向中垂直于 f1 的分量，f3 = f1 x f2；
///          2. 同样由体固连矢量构造三轴 (e1,e2,e3)；
///          3. 基础旋转 M0 满足 M0*f1 = e1、M0*f2 = e2、M0*f3 = e3；
///          4. 最后绕指定的体轴施加 offset 旋转，得到 M = R(offset) * M0。
///
///          第 4 步中 offset 所绕的轴取自**未施加 offset 之前**的体轴，因此当 offset 轴
///          与对齐轴重合时对齐关系严格保持；当 offset 轴取约束轴时，实际效果是把
///          "哪个体轴去对齐"整体转过一个角度，这与 STK 帮助中
///          "to constrain with the Y axis, set the offset to +90 degrees" 的描述一致。
/// @param alignRef 参考坐标系下的对齐向量
/// @param constrRef 参考坐标系下的约束向量
/// @param alignBody 体系下的对齐方向(单位向量)
/// @param constrBody 体系下的约束方向(单位向量)
/// @param offsetAxis offset 旋转所绕的体轴
/// @param offsetSense offset 的旋向
/// @param offset offset 角度 [rad]
/// @param rotation 输出参数，父轴系到本轴系的旋转变换
/// @return 错误码
/// @note 当约束向量与对齐向量平行(即约束方向没有垂直于对齐方向的分量)时无法定姿，
///       返回 eErrorInvalidParam。
AST_CORE_API errc_t aAlignConstrainRotation(const Vector3d& alignRef, const Vector3d& constrRef,
                                            const Vector3d& alignBody, const Vector3d& constrBody,
                                            EAttitudeAxis offsetAxis, EAttitudeOffsetSense offsetSense,
                                            double offset, Rotation& rotation);

/// @brief 获取体轴单位向量
/// @param axis 体轴标记
/// @return 体轴单位向量
AST_CORE_API Vector3d aAttitudeAxisVector(EAttitudeAxis axis);

/// @brief 对齐/约束姿态剖面 (Aligned and Constrained)
/// @details 用一对"对齐"矢量和一对"约束"矢量来定姿：
///          体系下的对齐矢量与参考系下的对齐矢量严格对齐，
///          在此前提下让体系下的约束矢量尽可能接近参考系下的约束矢量。
class AST_CORE_API AttitudeAlignConstrain : public AttitudeProfileBase
{
public:
    AST_OBJECT(AttitudeAlignConstrain)

    AttitudeAlignConstrain() = default;
    ~AttitudeAlignConstrain() override = default;

    using AttitudeProfileBase::getTransform;
    errc_t getTransform(const TimePoint& tp, Rotation& rotation) const override;

PROPERTIES:
    /// @brief 获取 offset 角度 [rad]
    /// @note 属性名与 STK .sa 文件里剖面块内的 Azimuth 字段保持一致，便于后续接入加载器
    double getAzimuth() const { return azimuth_; }
    /// @brief 设置 offset 角度 [rad]
    void setAzimuth(double azimuth) { azimuth_ = azimuth; }

    /// @brief 获取参考系下的对齐向量种类
    EAttitudeVector getAlignVector() const { return alignVector_; }
    /// @brief 设置参考系下的对齐向量种类
    void setAlignVector(EAttitudeVector kind) { alignVector_ = kind; }

    /// @brief 获取体系下的对齐轴
    EAttitudeAxis getAlignAxis() const { return alignAxis_; }
    /// @brief 设置体系下的对齐轴
    void setAlignAxis(EAttitudeAxis axis) { alignAxis_ = axis; }

    /// @brief 获取参考系下的约束向量种类
    EAttitudeVector getConstraintVector() const { return constraintVector_; }
    /// @brief 设置参考系下的约束向量种类
    void setConstraintVector(EAttitudeVector kind) { constraintVector_ = kind; }

    /// @brief 获取体系下的约束轴
    EAttitudeAxis getConstraintAxis() const { return constraintAxis_; }
    /// @brief 设置体系下的约束轴
    void setConstraintAxis(EAttitudeAxis axis) { constraintAxis_ = axis; }

    /// @brief 获取 offset 旋转所绕的体轴
    EAttitudeAxis getOffsetAxis() const { return offsetAxis_; }
    /// @brief 设置 offset 旋转所绕的体轴
    void setOffsetAxis(EAttitudeAxis axis) { offsetAxis_ = axis; }

    /// @brief 获取 offset 的旋向
    EAttitudeOffsetSense getOffsetSense() const { return offsetSense_; }
    /// @brief 设置 offset 的旋向
    void setOffsetSense(EAttitudeOffsetSense sense) { offsetSense_ = sense; }

protected:
    EAttitudeVector alignVector_{EAttitudeVector::eNadir};            ///< 参考系下的对齐向量
    EAttitudeVector constraintVector_{EAttitudeVector::eVelocity};    ///< 参考系下的约束向量
    EAttitudeAxis alignAxis_{EAttitudeAxis::eZ};                      ///< 体系下的对齐轴
    EAttitudeAxis constraintAxis_{EAttitudeAxis::eX};                 ///< 体系下的约束轴
    EAttitudeAxis offsetAxis_{EAttitudeAxis::eZ};                     ///< offset 旋转所绕的体轴
    EAttitudeOffsetSense offsetSense_{EAttitudeOffsetSense::eLeftHanded};  ///< offset 旋向
    double azimuth_{0.0};                                             ///< offset 角度 [rad]
};

/*! @} */

AST_NAMESPACE_END

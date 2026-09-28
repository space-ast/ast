///
/// @file      AttitudeProfile.hpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-03-13
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
#include "AstUtil/ObjectNamed.hpp"
#include "AstCore/Axes.hpp"
#include "AstCore/Point.hpp"
#include "AstCore/Frame.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Attitude
    @{
*/

class Rotation;
class KinematicRotation;

/// @brief 姿态参考向量的种类
/// @details 这些向量都由载体的位置、速度在剖面的参考坐标系下算出。
enum class EAttitudeVector
{
    eVelocity,      ///< 速度方向
    eNadir,         ///< 对地方向(由载体指向中心天体中心)
    eRadial,        ///< 径向方向(由中心天体中心指向载体)
    eOrbitNormal,   ///< 轨道法向(位置叉乘速度)
    eSun,           ///< 由载体指向太阳的方向
    eFrameZ,        ///< 参考坐标系的 Z 轴
};

/// @brief 姿态剖面抽象基类
/// @details 姿态剖面描述的是一台载体的体轴系随时间的变化规律，因此它本身就是一个 Axes，
///          其父轴系由参考坐标系(getFrame)决定，而体轴系相对参考系的旋转则由各具体剖面给出。
///
///          与一般轴系不同，姿态剖面需要载体(Point)的位置和速度才能计算对地方向、
///          速度方向、太阳方向等参考向量，因此使用前必须通过 setPoint 指定载体，
///          并通过 setFrame 指定参考坐标系(默认由 defaultFrame 给出)。
///
/// @note 约定一: getTransform 返回的是**父轴系到本轴系**的旋转，即
///       `v_this = rotation.transformVector(v_parent)`。
///       也就是说 rotation 矩阵的第 0/1/2 行分别是体轴 X/Y/Z 在**父轴系**下的分量。
///       这与 aFrameToVVLHMatrix 等函数的输出形状一致。
///
/// @note 约定二: getTransform(tp, KinematicRotation&) 中的角速度是
///       **本轴系相对父轴系的角速度，在父轴系下分解**。
///       具体剖面通常只需要实现 Rotation 版本，基类会用中心差分给出角速度；
///       有闭式解的剖面(如 AttitudeFixed、AttitudeSpinning)应当重载覆盖。
///
/// @note 具体剖面在重载 getTransform(tp, Rotation&) 时，必须在 public 区写一句
///       `using AttitudeProfile::getTransform;`，否则会遮蔽基类的
///       getTransform(tp, KinematicRotation&) 重载，导致对具体类型直接调用运动学版本时编译失败。
class AST_CORE_API AttitudeProfile : public Axes
{
public:
    AST_OBJECT(AttitudeProfile)

    AttitudeProfile() = default;
    ~AttitudeProfile() override = default;

    /// @brief 获取剖面所描述的载体
    /// @return 载体指针，未设置时返回 nullptr
    Point* getPoint() const;

    /// @brief 设置剖面所描述的载体
    /// @details 载体提供位置和速度，用于计算对地、速度、太阳等参考向量。
    /// @param point 载体指针(通常是一个 Mover)
    void setPoint(Point* point);

    /// @brief 获取参考坐标系
    /// @return 参考坐标系指针，未显式设置时返回 defaultFrame()
    Frame* getFrame() const;

    /// @brief 设置参考坐标系
    /// @details 参考坐标系决定了姿态的参考方向，也决定了载体的位置、速度在哪个坐标系下计算。
    ///          例如"ECI 速度约束对地指向"应指定地球惯性系，"ECF 速度约束"应指定地球固连系。
    /// @param frame 参考坐标系指针，传 nullptr 表示恢复为 defaultFrame()
    void setFrame(Frame* frame);

    /// @brief 获取父轴系，即参考坐标系的轴系
    /// @return 父轴系指针
    Axes* getParent() const override;

    /// @brief 获取本轴系相对父轴系的旋转变换
    /// @param tp 时间点
    /// @param rotation 输出参数，旋转变换
    /// @return 错误码
    errc_t getTransform(const TimePoint& tp, Rotation& rotation) const override = 0;

    /// @brief 获取本轴系相对父轴系的运动学旋转变换
    /// @details 基类默认用中心差分(步长 0.1s，与 STK 一致)对角速度做数值微分，
    ///          端点处退化为单侧差分。有闭式解的剖面应当重载本函数。
    /// @param tp 时间点
    /// @param rotation 输出参数，运动学旋转变换(包含角速度)
    /// @return 错误码
    errc_t getTransform(const TimePoint& tp, KinematicRotation& rotation) const override;

    /// @brief 获取指定种类的姿态参考向量(在参考坐标系下的单位向量)
    /// @param kind 参考向量种类
    /// @param tp 时间点
    /// @param vector 输出参数，参考向量
    /// @return 错误码
    errc_t getReferenceVector(EAttitudeVector kind, const TimePoint& tp, Vector3d& vector) const;

protected:
    /// @brief 该剖面的自然参考坐标系
    /// @details 当用户没有显式调用 setFrame 时使用。注意该函数是惰性求值的，
    ///          不可以在构造函数中调用(可能早于 aInitialize)。
    /// @return 参考坐标系指针
    virtual Frame* defaultFrame() const;

    /// @brief 获取载体在参考坐标系下的位置和速度
    /// @details 参考坐标系的**原点**需要位于中心天体，否则"对地"等方向量没有意义，
    ///          因此通常应该传 aFrameECI()、aFrameECF() 这类以天体中心为原点的坐标系。
    /// @param tp 时间点
    /// @param pos 输出参数，位置向量
    /// @param vel 输出参数，速度向量
    /// @return 错误码
    errc_t getState(const TimePoint& tp, Vector3d& pos, Vector3d& vel) const;

    /// @brief 获取参考坐标系下的地对方向(单位向量，由载体指向中心天体中心)
    /// @param tp 时间点
    /// @param dir 输出参数，方向单位向量
    /// @return 错误码
    errc_t getNadirDirection(const TimePoint& tp, Vector3d& dir) const;

    /// @brief 获取参考坐标系下的径向方向(单位向量，由中心天体中心指向载体)
    /// @param tp 时间点
    /// @param dir 输出参数，方向单位向量
    /// @return 错误码
    errc_t getRadialDirection(const TimePoint& tp, Vector3d& dir) const;

    /// @brief 获取参考坐标系下的速度方向(单位向量)
    /// @param tp 时间点
    /// @param dir 输出参数，方向单位向量
    /// @return 错误码
    errc_t getVelocityDirection(const TimePoint& tp, Vector3d& dir) const;

    /// @brief 获取参考坐标系下的轨道法向(单位向量，位置叉乘速度)
    /// @param tp 时间点
    /// @param dir 输出参数，方向单位向量
    /// @return 错误码
    errc_t getOrbitNormalDirection(const TimePoint& tp, Vector3d& dir) const;

    /// @brief 获取参考坐标系下由载体指向太阳的方向(单位向量)
    /// @param tp 时间点
    /// @param dir 输出参数，方向单位向量
    /// @return 错误码
    errc_t getSunDirection(const TimePoint& tp, Vector3d& dir) const;

protected:
    WeakPtr<Point>  point_{};   ///< 载体(弱引用，避免与 Platform::orientation_ 形成循环引用)
    SharedPtr<Frame> frame_{};  ///< 参考坐标系(nullptr 表示使用 defaultFrame())
};

/*! @} */

AST_NAMESPACE_END

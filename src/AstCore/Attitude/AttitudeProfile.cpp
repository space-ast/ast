///
/// @file      AttitudeProfile.cpp
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

#include "AttitudeProfile.hpp"
#include "AstCore/Frame.hpp"
#include "AstCore/Point.hpp"
#include "AstCore/BuiltinFrame.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/RunTimeSolarSystem.hpp"
#include "AstMath/Rotation.hpp"
#include "AstMath/KinematicRotation.hpp"
#include "AstMath/Matrix.hpp"
#include <cmath>

AST_NAMESPACE_BEGIN

/// @brief 中心差分求角速度时的时间步长
/// @details 与 STK 保持一致，见 STK 帮助 "Attitude interpolation and differentiation in STK"：
///          "the central differencing of attitude data is set to use points +/-0.1 seconds apart"。
///          步长太小会被位置/速度的舍入误差放大，太大则引入截断误差。
static constexpr double kAttitudeDiffStep = 0.1;

Point* AttitudeProfile::getPoint() const
{
    return point_.get();
}

void AttitudeProfile::setPoint(Point* point)
{
    point_ = point;
}

Frame* AttitudeProfile::getFrame() const
{
    return frame_ ? frame_.get() : defaultFrame();
}

void AttitudeProfile::setFrame(Frame* frame)
{
    frame_ = frame;
}

Axes* AttitudeProfile::getParent() const
{
    Frame* frame = getFrame();
    return frame ? frame->getAxes() : nullptr;
}

Frame* AttitudeProfile::defaultFrame() const
{
    return aFrameECI();
}

errc_t AttitudeProfile::getState(const TimePoint& tp, Vector3d& pos, Vector3d& vel) const
{
    Point* point = point_.get();
    if (point == nullptr)
        return eErrorNullInput;
    Frame* frame = getFrame();
    if (frame == nullptr)
        return eErrorNullInput;
    return point->getPosVelIn(frame, tp, pos, vel);
}

errc_t AttitudeProfile::getNadirDirection(const TimePoint& tp, Vector3d& dir) const
{
    Vector3d pos, vel;
    errc_t rc = getState(tp, pos, vel);
    if (rc != eNoError)
        return rc;
    const double norm = pos.norm();
    if (norm == 0.0)
        return eErrorInvalidParam;
    dir = pos / -norm;
    return eNoError;
}

errc_t AttitudeProfile::getRadialDirection(const TimePoint& tp, Vector3d& dir) const
{
    Vector3d pos, vel;
    errc_t rc = getState(tp, pos, vel);
    if (rc != eNoError)
        return rc;
    const double norm = pos.norm();
    if (norm == 0.0)
        return eErrorInvalidParam;
    dir = pos / norm;
    return eNoError;
}

errc_t AttitudeProfile::getVelocityDirection(const TimePoint& tp, Vector3d& dir) const
{
    Vector3d pos, vel;
    errc_t rc = getState(tp, pos, vel);
    if (rc != eNoError)
        return rc;
    const double norm = vel.norm();
    if (norm == 0.0)
        return eErrorInvalidParam;
    dir = vel / norm;
    return eNoError;
}

errc_t AttitudeProfile::getOrbitNormalDirection(const TimePoint& tp, Vector3d& dir) const
{
    Vector3d pos, vel;
    errc_t rc = getState(tp, pos, vel);
    if (rc != eNoError)
        return rc;
    Vector3d normal = pos.cross(vel);
    const double norm = normal.norm();
    if (norm == 0.0)
        return eErrorInvalidParam;
    dir = normal / norm;
    return eNoError;
}

errc_t AttitudeProfile::getReferenceVector(EAttitudeVector kind, const TimePoint& tp, Vector3d& vector) const
{
    switch (kind)
    {
    case EAttitudeVector::eVelocity:
        return getVelocityDirection(tp, vector);
    case EAttitudeVector::eNadir:
        return getNadirDirection(tp, vector);
    case EAttitudeVector::eRadial:
        return getRadialDirection(tp, vector);
    case EAttitudeVector::eOrbitNormal:
        return getOrbitNormalDirection(tp, vector);
    case EAttitudeVector::eSun:
        return getSunDirection(tp, vector);
    case EAttitudeVector::eFrameZ:
        vector = Vector3d::UnitZ();
        return eNoError;
    }
    return eErrorInvalidParam;
}

errc_t AttitudeProfile::getSunDirection(const TimePoint& tp, Vector3d& dir) const
{
    Vector3d pos, vel;
    errc_t rc = getState(tp, pos, vel);
    if (rc != eNoError)
        return rc;

    CelestialBody* sun = aGetSun();
    if (sun == nullptr)
        return eErrorNullInput;

    Vector3d sunPos;
    rc = sun->getPosIn(getFrame(), tp, sunPos);
    if (rc != eNoError)
        return rc;

    Vector3d towardSun = sunPos - pos;
    const double norm = towardSun.norm();
    if (norm == 0.0)
        return eErrorInvalidParam;
    dir = towardSun / norm;
    return eNoError;
}

/// @brief 求旋转矩阵在单位旋转处的对数向量(轴角)
/// @details 对旋转矩阵 A，其反对称部分 S = (A - A^T) / 2 = sin(theta) * [n]x，
///          于是 log(A) = theta * n = (theta / sin(theta)) * vee(S)。
///          theta 很小时 theta/sin(theta) 趋近 1，直接取 vee(S) 即可。
static Vector3d aRotationLogVector(const Matrix3d& matrix)
{
    const double sx = 0.5 * (matrix(2, 1) - matrix(1, 2));
    const double sy = 0.5 * (matrix(0, 2) - matrix(2, 0));
    const double sz = 0.5 * (matrix(1, 0) - matrix(0, 1));

    double cosTheta = 0.5 * (matrix(0, 0) + matrix(1, 1) + matrix(2, 2) - 1.0);
    if (cosTheta > 1.0)
        cosTheta = 1.0;
    else if (cosTheta < -1.0)
        cosTheta = -1.0;
    const double theta = std::acos(cosTheta);

    const double sinTheta = std::sqrt(sx * sx + sy * sy + sz * sz);
    const double scale = (sinTheta > 1e-12) ? (theta / sinTheta) : 1.0;
    return Vector3d{sx * scale, sy * scale, sz * scale};
}

errc_t AttitudeProfile::getTransform(const TimePoint& tp, KinematicRotation& rotation) const
{
    Rotation rot0;
    errc_t rc = this->getTransform(tp, rot0);
    if (rc != eNoError)
        return rc;

    const double h = kAttitudeDiffStep;
    const TimePoint tpPlus  = tp + h;
    const TimePoint tpMinus = tp - h;

    Rotation rotPlus, rotMinus;
    errc_t rcPlus  = this->getTransform(tpPlus, rotPlus);
    errc_t rcMinus = this->getTransform(tpMinus, rotMinus);

    const Matrix3d& m0 = rot0.getMatrix();

    /*!
        先求出两端体轴系之间的"增量旋转"，再取它的对数(轴角)，除以实际时间间隔得到角速度。

        直接对旋转矩阵的元素做差商是有偏的：对绕定轴以 w 匀速旋转的体轴系，
        逐元素中心差分得到的是 sin(w*h)/h 而不是 w，相对误差约为 (w*h)^2/6，
        取 h=0.1s、w=0.35rad/s 时高达 2e-4，而且这个误差会随转速平方增长。
        改用增量旋转的矩阵对数，则在定轴匀速情形下是精确的，其余情形误差为 O(h^2)。

        记 M 为父系到体系的转换矩阵，则增量旋转 A = M(t2)^T * M(t1) 是"把父系下的
        向量从 t1 时刻推进到 t2 时刻"的旋转，其对数除以时间间隔即父系下的角速度
        (符合 KinematicRotation 中"本系相对父系、在父系下分解"的约定)。
    */
    Matrix3d increment;   ///< 增量旋转矩阵
    double dt = 0.0;      ///< 实际时间间隔
    if (rcPlus == eNoError && rcMinus == eNoError)
    {
        // 中心差分
        dt = tpPlus - tpMinus;
        const Matrix3d& mp = rotPlus.getMatrix();
        const Matrix3d& mm = rotMinus.getMatrix();
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
            {
                double value = 0.0;
                for (int k = 0; k < 3; k++)
                    value += mp(k, i) * mm(k, j);
                increment(i, j) = value;
            }
    }
    else if (rcPlus == eNoError)
    {
        // 前向差分(时间点靠近可用数据的起点)
        dt = tpPlus - tp;
        const Matrix3d& mp = rotPlus.getMatrix();
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
            {
                double value = 0.0;
                for (int k = 0; k < 3; k++)
                    value += mp(k, i) * m0(k, j);
                increment(i, j) = value;
            }
    }
    else if (rcMinus == eNoError)
    {
        // 后向差分(时间点靠近可用数据的终点)
        dt = tp - tpMinus;
        const Matrix3d& mm = rotMinus.getMatrix();
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
            {
                double value = 0.0;
                for (int k = 0; k < 3; k++)
                    value += m0(k, i) * mm(k, j);
                increment(i, j) = value;
            }
    }
    else
    {
        // 两侧都取不到载体状态，返回原始错误码，而不是悄悄给一个零角速度
        return rcPlus;
    }

    const Vector3d logVector = aRotationLogVector(increment);
    rotation.setRotation(rot0);
    rotation.setRotationRate(logVector / dt);
    return eNoError;
}

AST_NAMESPACE_END

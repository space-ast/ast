///
/// @file      StateMixedSpherical.hpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-09-14
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
/// 您使用本软件所产生的风险，需由您自行承担。

#pragma once

#include "AstGlobal.h"
#include "AstCore/State.hpp"
#include "AstMath/Vector.hpp"
#include "AstCore/OrbitElement.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/
class StateMixedSpherical;
using PStateMixedSpherical = StateMixedSpherical*;
using HStateMixedSpherical = SharedPtr<StateMixedSpherical>;

/// @brief 混合球坐标状态
/// @details
/// 混合球坐标状态用天体固连系下的大地坐标(大地经度、大地纬度、高度)描述位置,
/// 用参考系(惯性系)下惯性速度矢量的大小、航迹角、航迹方位角描述速度。
/// 具体的转换算法与奇异处理参见 @ref aCartToMixedSpherical 和 @ref aMixedSphericalToCart 。
/// @note  该状态需要参考坐标系(提供中心天体与引力常数)、状态历元与中心天体的参考椭球:
///        参考坐标系即惯性速度所在的坐标系, 应为惯性系, 航迹角与航迹方位角亦在该系下度量;
///        中心天体取自参考坐标系, 参考椭球取自中心天体的形状(见 @ref BodyShape ),
///        椭球与固连系只用于位置分量。
class AST_CORE_API StateMixedSpherical final: public State
{
public:
    AST_OBJECT(StateMixedSpherical)
    AST_PROPERT(Lon)
    AST_PROPERT(Lat)
    AST_PROPERT(Alt)
    AST_PROPERT(FltPathAng)
    AST_PROPERT(VertFltPathAng)
    AST_PROPERT(FltPathAzi)
    AST_PROPERT(Vel)
    static PStateMixedSpherical New();
    static HStateMixedSpherical MakeShared();
    static PStateMixedSpherical New(const MixedSphericalElem& mixedSphericalElem);
    static HStateMixedSpherical MakeShared(const MixedSphericalElem& mixedSphericalElem);

    StateMixedSpherical() = default;
    StateMixedSpherical(const MixedSphericalElem& mixedSphericalElem);
    StateMixedSpherical(const State& state);
    ~StateMixedSpherical() override = default;
public:
    EStateType getStateType() const override { return EStateType::eMixedSpherical; }
    errc_t getState(CartState& state) const override;
    errc_t getState(ModOrbElem& orbElem) const override;
    errc_t setState(const CartState& state) override;
    errc_t setState(const ModOrbElem& orbElem) override;
public:
    /// @brief 获取混合球坐标根数
    /// @param mixedSphericalElem 输出混合球坐标根数
    void getState(MixedSphericalElem& mixedSphericalElem) const { mixedSphericalElem = mixedSphericalElem_; }

    /// @brief 设置混合球坐标根数
    /// @param mixedSphericalElem 混合球坐标根数
    /// @return 错误码，成功返回eNoError
    errc_t setState(const MixedSphericalElem& mixedSphericalElem);
protected:
    /// @brief 获取中心天体的参考椭球
    /// @param shape 输出参考椭球
    /// @return 错误码，成功返回eNoError
    BodyShape* getShape() const;

    /// @brief 获取惯性系到天体固连系的旋转
    /// @param inertialToFixed 输出惯性系到天体固连系的旋转
    /// @return 错误码，成功返回eNoError
    errc_t getInertialToFixed(Rotation& inertialToFixed) const;
PROPERTIES:
    /// @brief 获取大地经度
    angle_d getLon() const { return mixedSphericalElem_.lon_; }
    /// @brief 设置大地经度
    void setLon(angle_d lon) { mixedSphericalElem_.lon_ = lon; }

    /// @brief 获取大地纬度
    angle_d getLat() const { return mixedSphericalElem_.lat_; }
    /// @brief 设置大地纬度
    void setLat(angle_d lat) { mixedSphericalElem_.lat_ = lat; }

    /// @brief 获取高度
    length_d getAlt() const { return mixedSphericalElem_.alt_; }
    /// @brief 设置高度
    void setAlt(length_d alt) { mixedSphericalElem_.alt_ = alt; }

    /// @brief 获取航迹角(水平航迹角)
    angle_d getFltPathAng() const { return mixedSphericalElem_.fpa_; }
    /// @brief 设置航迹角
    void setFltPathAng(angle_d fpa) { mixedSphericalElem_.fpa_ = fpa; }

    /// @brief 获取垂直航迹角(水平航迹角的余角)
    angle_d getVertFltPathAng() const { return mixedSphericalElem_.getVertFltPathAng(); }
    /// @brief 设置垂直航迹角
    void setVertFltPathAng(angle_d vfpa) { mixedSphericalElem_.fpa_ = kHalfPI - vfpa; }

    /// @brief 获取航迹方位角
    angle_d getFltPathAzi() const { return mixedSphericalElem_.azi_; }
    /// @brief 设置航迹方位角
    void setFltPathAzi(angle_d azi) { mixedSphericalElem_.azi_ = azi; }

    /// @brief 获取速度大小
    speed_d getVel() const { return mixedSphericalElem_.v_; }
    /// @brief 设置速度大小
    void setVel(speed_d v) { mixedSphericalElem_.v_ = v; }
protected:
    MixedSphericalElem mixedSphericalElem_{};   ///< 混合球坐标根数
};



/*! @} */

AST_NAMESPACE_END

///
/// @file      StateGeodetic.hpp
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
class StateGeodetic;
using PStateGeodetic = StateGeodetic*;
using HStateGeodetic = SharedPtr<StateGeodetic>;

/// @brief 大地坐标状态
/// @details
/// 位置与速度均用天体固连系下的大地坐标及其变化率描述:
/// 位置为大地经度、大地纬度、高度, 速度为三者对时间的导数(即固连系速度的分量表达)。
/// 具体的转换算法与奇异处理参见 @ref aCartToGeodetic 和 @ref aGeodeticToCart 。
/// @note  该状态需要参考坐标系(提供中心天体与引力常数)、状态历元与中心天体的参考椭球:
///        中心天体取自参考坐标系, 参考椭球取自中心天体的形状(见 @ref BodyShape );
///        状态分量全部定义在天体固连系下, 与参考坐标系是否为惯性系无关,
///        参考系与固连系之间的运动学变换由状态历元确定(见 @ref KinematicTransform )。
/// @warning 与 @ref StateMixedSpherical 不同, 本状态的速度是固连系速度, 不含固连系牵连速度;
///          同步轨道等共转工况下三个变化率均为零。
class AST_CORE_API StateGeodetic final: public State
{
public:
    AST_OBJECT(StateGeodetic)
    AST_PROPERT(Lon)
    AST_PROPERT(Lat)
    AST_PROPERT(Alt)
    AST_PROPERT(LonRate)
    AST_PROPERT(LatRate)
    AST_PROPERT(AltRate)
    static PStateGeodetic New();
    static HStateGeodetic MakeShared();
    static PStateGeodetic New(const GeodeticElem& geodeticElem);
    static HStateGeodetic MakeShared(const GeodeticElem& geodeticElem);

    StateGeodetic() = default;
    StateGeodetic(const GeodeticElem& geodeticElem);
    StateGeodetic(const State& state);
    ~StateGeodetic() override = default;
public:
    EStateType getStateType() const override { return EStateType::eGeodetic; }
    errc_t getState(CartState& state) const override;
    errc_t getState(ModOrbElem& orbElem) const override;
    errc_t setState(const CartState& state) override;
    errc_t setState(const ModOrbElem& orbElem) override;
public:
    /// @brief 获取大地坐标根数
    /// @param geodeticElem 输出大地坐标根数
    void getState(GeodeticElem& geodeticElem) const { geodeticElem = geodeticElem_; }

    /// @brief 设置大地坐标根数
    /// @param geodeticElem 大地坐标根数
    /// @return 错误码，成功返回eNoError
    errc_t setState(const GeodeticElem& geodeticElem);
protected:
    /// @brief 获取中心天体的参考椭球
    /// @return 参考椭球, 失败返回nullptr
    BodyShape* getShape() const;

    /// @brief 获取中心天体的固连坐标系
    /// @return 固连坐标系, 失败返回nullptr
    Frame* getFixedFrame() const;

    /// @brief 获取参考坐标系到天体固连系的运动学变换
    /// @param frameToFixed 输出参考坐标系到天体固连系的运动学变换
    /// @return 错误码，成功返回eNoError
    errc_t getFrameToFixed(KinematicTransform& frameToFixed) const;

    /// @brief 获取天体固连系到参考坐标系的运动学变换
    /// @param fixedToFrame 输出天体固连系到参考坐标系的运动学变换
    /// @return 错误码，成功返回eNoError
    errc_t getFixedToFrame(KinematicTransform& fixedToFrame) const;
PROPERTIES:
    /// @brief 获取大地经度
    angle_d getLon() const { return geodeticElem_.lon_; }
    /// @brief 设置大地经度
    void setLon(angle_d lon) { geodeticElem_.lon_ = lon; }

    /// @brief 获取大地纬度
    angle_d getLat() const { return geodeticElem_.lat_; }
    /// @brief 设置大地纬度
    void setLat(angle_d lat) { geodeticElem_.lat_ = lat; }

    /// @brief 获取高度
    length_d getAlt() const { return geodeticElem_.alt_; }
    /// @brief 设置高度
    void setAlt(length_d alt) { geodeticElem_.alt_ = alt; }

    /// @brief 获取大地经度变化率
    angle_d getLonRate() const { return geodeticElem_.lonRate_; }
    /// @brief 设置大地经度变化率
    void setLonRate(angle_d lonRate) { geodeticElem_.lonRate_ = lonRate; }

    /// @brief 获取大地纬度变化率
    angle_d getLatRate() const { return geodeticElem_.latRate_; }
    /// @brief 设置大地纬度变化率
    void setLatRate(angle_d latRate) { geodeticElem_.latRate_ = latRate; }

    /// @brief 获取高度变化率
    speed_d getAltRate() const { return geodeticElem_.altRate_; }
    /// @brief 设置高度变化率
    void setAltRate(speed_d altRate) { geodeticElem_.altRate_ = altRate; }
protected:
    GeodeticElem geodeticElem_{};   ///< 大地坐标根数
};



/*! @} */

AST_NAMESPACE_END

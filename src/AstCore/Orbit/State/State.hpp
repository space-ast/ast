///
/// @file      State.hpp
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
#include "AstUtil/Object.hpp"
#include "AstUtil/ObjectNamed.hpp"
#include "AstCore/Frame.hpp"
#include "AstCore/EventTime.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 状态类型
enum class EStateType
{
    eCartesian,     ///< 笛卡尔状态
    eKeplerian,     ///< 开普勒状态
    eUnknown,       ///< 未知状态类型
};

class State;
using PState = State*;
using HState = SharedPtr<State>;

/// @brief 航天器状态
/// @details 参考了orekit的Orbit类
/// 但是这个可能不只是限制于轨道状态，所以取名为State
class AST_CORE_API State: public ObjectNamed
{
public:
    AST_OBJECT(State)
    AST_PROPERT(Frame)
    AST_PROPERT(StateEpoch)
;

    static PState NewDefault();

    State() = default;
    ~State() override = default;

    /// @brief 创建状态
    /// @param type 状态类型
    /// @return HState 状态句柄
    static HState MakeShared(EStateType type);

    /// @brief 创建状态
    /// @param state 状态
    /// @param type 状态类型
    /// @return HState 状态句柄
    static HState MakeShared(State& state, EStateType type);


    /// @brief 创建状态
    /// @param type 状态类型
    /// @return PState 状态指针
    static PState New(EStateType type);


    /// @brief 创建状态
    /// @param state 状态
    /// @param type 状态类型
    /// @return PState 状态指针
    static PState New(State& state, EStateType type);
protected:
    State(const State& state) = default; // 怎么处理深拷贝? 现在默认浅拷贝
public:
    /// @brief 获取状态类型
    /// @return EStateType 状态类型
    virtual EStateType getStateType() const = 0;

    /// @brief 获取状态
    /// @param state 状态
    /// @return errc_t 错误码
    virtual errc_t getState(CartState& state) const = 0;

    /// @brief 获取状态
    /// @param orbElem 轨道根数
    /// @return errc_t 错误码
    virtual errc_t getState(ModOrbElem& orbElem) const = 0;

    /// @brief 设置状态
    /// @param state 状态
    /// @return errc_t 错误码
    virtual errc_t setState(const CartState& state) = 0;

    /// @brief 设置状态
    /// @param orbElem 轨道根数
    /// @return errc_t 错误码
    virtual errc_t setState(const ModOrbElem& orbElem) = 0;


    /// @brief 获取在给定坐标系下的状态根数
    /// @param frame 坐标系
    /// @param orbElem 轨道根数
    /// @return errc_t 错误码
    errc_t getStateIn(Frame* frame, ModOrbElem& orbElem) const;


    /// @brief 获取在给定坐标系下的状态量
    /// @param frame 坐标系
    /// @param state 状态
    /// @return errc_t 错误码
    errc_t getStateIn(Frame* frame, CartState& state) const;

    /// @brief 获取在给定天体惯性坐标系下的状态量
    /// @param body 天体
    /// @param state 状态
    /// @return errc_t 错误码
    errc_t getStateInBodyInertial(Body* body, CartState& state) const;


public:

    /// @brief 设置参考坐标系
    /// @param frameName 坐标系名称
    errc_t setFrameByName(StringView frameName);

    /// @brief 改变参考坐标系
    /// @param frame 新的参考坐标系
    /// @return errc_t 错误码
    errc_t changeFrame(Frame* frame);



    /// @brief 设置状态历元时间
    /// @param stateEpoch 状态历元时间
    void setStateEpoch(const TimePoint& stateEpoch);

    /// @brief 获取状态历元时间
    /// @param stateEpoch 状态历元时间
    /// @return errc_t 错误码
    errc_t getStateEpoch(TimePoint& stateEpoch) const;

    /// @brief 获取状态历元时间
    /// @return TimePoint 状态历元时间
    TimePoint getStateEpoch_TimePoint() const;

    /// @brief 获取状态历元时间句柄
    /// @return SharedPtr<EventTime>& 状态历元时间句柄
    SharedPtr<EventTime>& getStateEpochHandle(){ return stateEpoch_; }

    /// @brief 获取天体
    Body* getBody() const;

    /// @brief 获取天体半径
    /// @return double 天体半径
    double getBodyRadius() const;

    /// @brief 获取引力常数
    /// @return double 引力常数
    double getGM() const { return gm_; }

    /// @brief 设置引力常数
    /// @param gm 引力常数
    void setGM(double gm){ gm_ = gm; }
public:

public:


#if 0 
// 这些与历元坐标系定义相关的接口增加了类设计的复杂度
// 应该考虑如何将历元坐标系相关逻辑放在该类的顶层模块，不在这里处理相关逻辑
public: // 与历元坐标系定义相关的接口：
    void  setCoordEpoch(EventTime* coordEpoch);

    void  setCoordEpoch(const TimePoint& coordEpoch);

    errc_t getCoordEpoch(TimePoint& coordEpoch) const;

    bool  getUseCoordEpoch() const;

    void  setUseCoordEpoch(bool useCoordEpoch);

    void  setCoordAxes(Axes* axes);

    errc_t  changeCoordAxes(Axes* axes);

    Axes* getCoordAxes() const;
#endif

PROPERTIES:
    /// @brief 获取参考坐标系
    Frame* getFrame() const{ return frame_.get(); }

    /// @brief 设置参考坐标系
    void setFrame(Frame* frame);

    /// @brief 获取状态历元时间
    EventTime* getStateEpoch() const{ return stateEpoch_.get(); }

    /// @brief 设置状态历元时间
    /// @param stateEpoch 状态历元时间
    void setStateEpoch(EventTime* stateEpoch);

protected:
    SharedPtr<Frame>        frame_;                ///< 参考坐标系
    double                  gm_{0};                ///< 引力常数
    SharedPtr<EventTime>    stateEpoch_;           ///< 状态历元时间
};

/*! @} */

AST_NAMESPACE_END

///
/// @file      HPOP.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-16
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
#include "AstUtil/Constants.h"
#include "AstUtil/ScopedPtr.hpp"
#include "AstUtil/Object.hpp"
#include "AstUtil/ObjectNamed.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/StateMapper.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstCore/HPOPForceModel.hpp"
#include "AstCore/SpacecraftParam.hpp"
#include "AstCore/Frame.hpp"
#include <string>
#include <vector>


AST_NAMESPACE_BEGIN

/*!
    @addtogroup Propagator
    @{
*/

class HPOPEquation; 
class ODEIntegrator;
class HPOPForceModel;
class HPOPStateMapper;

/// @brief 高精度轨道预报器
class AST_CORE_API HPOP: public ObjectNamed
{
public:
    AST_OBJECT(HPOP)

    HPOP();
    ~HPOP();
public:
    /// @brief 设置力模型
    errc_t setForceModel(HPOPForceModel&& forcemodel);
    errc_t setForceModel(const HPOPForceModel& forcemodel);

    /// @brief 获取力模型(只读，不支持修改)
    const HPOPForceModel& forceModel() const;

    void setSpacecraftParam(const SpacecraftParam& spacecraftParam);

    /// @brief 获取航天器参数(只读，不支持修改)
    const SpacecraftParam& spacecraftParam() const;

    /// @brief 获取预报坐标系
    Frame* propagationFrame() const;

    /// @brief 设置预报坐标系
    errc_t setPropagationFrame(Frame* frame);

    /// @brief 设置积分器
    void setIntegrator(ODEIntegrator* integrator);

    /// @brief 获取积分器
    ODEIntegrator* getIntegrator() const;

    /// @brief 轨道预报，直到达到结束时间或者触发由addEventDetector添加的事件
    /// @details 考虑到有事件触发的预报停止条件，所以预报结束时间同时也是一个输出参数
    /// @param[in]      startTime   预报起始时间
    /// @param[in,out]  targetTime  预报结束时间
    /// @param[in,out]  position    位置向量
    /// @param[in,out]  velocity    速度向量
    /// @return errc_t  错误码
    errc_t propagate(const TimePoint& startTime, TimePoint& targetTime, Vector3d& position, Vector3d& velocity);
    errc_t propagate(const TimePoint& startTime, TimePoint& targetTime, CartState& state){return propagate(startTime, targetTime, state.position(), state.velocity());}


    /// @brief 带状态转换矩阵的轨道预报，直到达到结束时间或者触发由addEventDetector添加的事件
    /// @details 考虑到有事件触发的预报停止条件，所以预报结束时间同时也是一个输出参数
    /// @param[in]      startTime   预报起始时间
    /// @param[in,out]  targetTime  预报结束时间
    /// @param[in,out]  state       状态向量
    /// @param[in,out]  stm         状态转换矩阵
    /// @return errc_t  错误码
    errc_t propagate(const TimePoint& startTime, TimePoint& targetTime, CartState& state, Matrix6d& stm);

    /// @brief 带状态转换矩阵和参数敏感度的轨道预报，直到达到结束时间或者触发由addEventDetector添加的事件
    /// @details 考虑到有事件触发的预报停止条件，所以预报结束时间同时也是一个输出参数
    /// 同时预报 6×6 STM 和两个 6×1 参数敏感度向量：
    ///   - 末端状态对阻力弹道系数(B = Cd·A/m)的偏导 ∂[r,v]/∂B
    ///   - 末端状态对SRP综合参数(K = Cr·A/m)的偏导 ∂[r,v]/∂K
    /// @param[in]      startTime         预报起始时间
    /// @param[in,out]  targetTime        预报结束时间
    /// @param[in,out]  state             位置速度状态
    /// @param[in,out]  stm               状态转换矩阵 Φ (6×6)
    /// @param[in,out]  stateSensWrtDrag  敏感度向量 ∂[r,v]/∂B (6×1)
    /// @param[in,out]  stateSensWrtSRP   敏感度向量 ∂[r,v]/∂K (6×1)
    /// @return errc_t  错误码
    errc_t propagate(const TimePoint& startTime, TimePoint& targetTime,
                     CartState& state, Matrix6d& stm,
                     Vector6d& stateSensWrtDrag, Vector6d& stateSensWrtSRP);
    
    /// @brief 初始化
    errc_t initialize();

    /// @brief 获取状态映射器
    StateMapper* stateMapper() const;

    /// @brief 添加事件检测器
    /// @param[in] eventDetector 事件检测器实例指针
    void addEventDetector(EventDetector* eventDetector);

    /// @brief 清除所有事件检测器
    void clearEventDetectors();

private:
    /// @brief 保证方程存在：若为空则用当前配置创建，并返回其指针
    HPOPEquation* ensureEquation() const;

    /// @brief 设置参考历元并执行数值积分
    /// @param[in]     startTime   预报起始时间
    /// @param[in,out] targetTime  预报结束时间（事件触发时会被截断）
    /// @param[in,out] y           状态向量
    /// @return errc_t 错误码
    errc_t integrateState(const TimePoint& startTime, TimePoint& targetTime, double* y);

private:
    HPOPForceModel  forceModel_{};                     ///< 力模型配置
    SpacecraftParam spacecraftParam_{};                ///< 航天器参数配置
    HFrame          propagationFrame_{};               ///< 预报坐标系配置

    mutable ScopedPtr<HPOPEquation> equation_{};       ///< 高精度轨道预报方程（缓存；配置变更时为 nullptr）
    mutable SharedPtr<ODEIntegrator> integrator_{};    ///< 高精度轨道预报积分器
    ScopedPtr<HPOPStateMapper> stateMapper_{};         ///< 状态映射器
};


/*! @} */

AST_NAMESPACE_END

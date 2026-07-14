///
/// @file      StateMapper.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-24
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
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
#include "AstCore/TimePoint.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstMath/Vector.hpp"
#include "AstMath/Matrix.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 状态映射器
/// @details 状态映射器用于将积分器的状态向量映射到物理量，例如位置、速度等
class StateMapper
{
public:
    StateMapper() = default;
    virtual ~StateMapper() = default;

    /// @brief 将状态向量映射到状态量
    /// @param[in] y 状态向量指针
    /// @param[out] state 航天器状态
    virtual void toState(const double* y, double x, SpacecraftState& state) const = 0;

    /// @brief 将状态量映射到状态向量
    /// @param[in] state 航天器状态
    /// @param[out] y 状态向量指针
    virtual void fromState(const SpacecraftState& state, double* y, double& x) const = 0;

    /// @brief 将状态向量映射到笛卡尔状态
    /// @param[in] y 状态向量指针
    /// @param[out] state 笛卡尔状态
    virtual void toState(const double* y, CartState& state) const = 0;
    CartState toCartState(const double* y) const{CartState state{}; toState(y, state); return state;}


    /// @brief 将状态向量映射到状态转移矩阵
    /// @param[in] y 状态向量指针
    /// @param[out] stm 状态转移矩阵
    /// @return errc_t 错误码
    virtual errc_t toStateTransitionMatrix(const double* y, Matrix6d& stm) const = 0;
    Matrix6d toStateTransitionMatrix(const double* y) const{Matrix6d stm{}; toStateTransitionMatrix(y, stm); return stm;}

    /// @brief 将状态向量映射到状态相对于弹道系数B的敏感性向量(笛卡尔状态量对阻力弹道系数(B = Cd·A/m)的偏导)
    /// @param[in] y 状态向量指针
    /// @param[out] stateSensToDrag 状态敏感性向量
    /// @return errc_t 错误码
    virtual errc_t toStateSensitivityWrtDrag(const double* y, Vector6d& stateSensWrtDrag) const = 0;
    Vector6d toStateSensitivityWrtDrag(const double* y) const{Vector6d stateSensWrtDrag{}; toStateSensitivityWrtDrag(y, stateSensWrtDrag); return stateSensWrtDrag;}

    /// @brief 将状态向量映射到状态相对于SRP综合参数K的敏感性向量(笛卡尔状态量对SRP综合参数(K = Cr·A/m)的偏导)
    /// @param[in] y 状态向量指针
    /// @param[out] stateSensWrtSRP 状态敏感性向量
    /// @return errc_t 错误码
    virtual errc_t toStateSensitivityWrtSRP(const double* y, Vector6d& stateSensWrtSRP) const = 0;
    Vector6d toStateSensitivityWrtSRP(const double* y) const{Vector6d stateSensWrtSRP{}; toStateSensitivityWrtSRP(y, stateSensWrtSRP); return stateSensWrtSRP;}


    /// @brief 将自变量映射到时间点
    /// @param[in] x 自变量
    /// @param[out] time 时间点
    void toTime(double x, TimePoint& time) const
    {
        time = epoch_ + x;
    }

    /// @brief 将时间点映射到自变量
    /// @param[in] time 时间点
    /// @param[out] x 自变量
    void fromTime(const TimePoint& time, double& x) const
    {
        x = time - epoch_;
    }
public:

    /// @brief 获取参考历元
    /// @return 参考历元
    const TimePoint& epoch() const{return epoch_;}

    /// @brief 设置参考历元
    /// @param[in] epoch 参考历元
    void setEpoch(TimePoint epoch){epoch_ = epoch;}
private:
    TimePoint epoch_;   ///< 参考历元
};

/*! @} */

AST_NAMESPACE_END

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
#include <string>
#include <vector>


AST_NAMESPACE_BEGIN

class HPOPEquation; 
class ODEIntegrator;

/// @brief 高精度轨道预报力模型
class HPOPForceModel
{
public:
    struct Gravity
    {
        std::string model_{};               ///< 中心天体重力场模型(模型名称或者引力场文件路径)
        int degree_{2};                     ///< 中心天体重力场计算阶数
        int order_{0};                      ///< 中心天体重力场计算次数
    };
    struct Drag
    {
        // @todo;
    };
    struct SRP
    {
        // @todo;
    };
    struct ThirdBody
    {
        // @todo;
    };
    using ThirdBodyList = std::vector<ThirdBody>;
public:
    bool            useDrag_{false};                    ///< 是否使用阻力模型
    bool            useSRP_{false};                     ///< 是否使用太阳辐射压模型
    bool            useMoonGravity_{false};             ///< 是否使用月球引力
    Gravity         gravity_;                           ///< 中心天体重力场
    Drag            drag_;                              ///< 阻力模型
    SRP             srp_;                               ///< 太阳辐射压模型
    double          moonGravity_{kMoonGrav};            ///< 月球引力系数
    ThirdBodyList   thirdBodies_;                       ///< 三体引力
};

/// @brief 高精度轨道预报接口类
class AST_CORE_API HPOP
{
public:
    HPOP() = default;
    ~HPOP();
public:
    /// @brief 设置力模型
    err_t setForceModel(const HPOPForceModel& forcemodel);

    /// @brief 设置积分器
    void setIntegrator(ODEIntegrator* integrator);

    /// @brief 获取积分器
    ODEIntegrator* getIntegrator() const;

    /// @brief 轨道预报
    /// 考虑到有停止条件，所以预报结束时间同时也是一个输出参数
    /// @param[in]      startTime   预报起始时间
    /// @param[in,out]  targetTime  预报结束时间
    /// @param[in,out]  position    输出位置向量
    /// @param[in,out]  velocity    输出速度向量
    /// @return err_t  错误码
    err_t propagate(const TimePoint& startTime, TimePoint& targetTime, Vector3d& position, Vector3d& velocity);

    /// @brief 初始化
    err_t initialize();
protected:
    HPOPEquation* equation_{nullptr};               ///< 高精度轨道预报方程
    mutable ODEIntegrator* integrator_{nullptr};    ///< 高精度轨道预报积分器
    HPOPForceModel forcemodel_;                     ///< 力模型配置
};

AST_NAMESPACE_END

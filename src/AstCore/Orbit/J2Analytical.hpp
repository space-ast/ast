///
/// @file      J2Analytical.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-31
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
#include "AstCore/OrbitElement.hpp"
#include "AstCore/TimePoint.hpp"

AST_NAMESPACE_BEGIN


class ModOrbElem;       ///< 改进轨道根数
class TimePoint;        ///< 时间点

class J2Analytical
{
public:
    /// @brief     构造函数
    /// @details   ~
    /// @param     modOrbElem 改进轨道根数
    /// @param     epoch      参考时间点
    /// @param     gm         引力常数
    /// @param     j2         J2项
    /// @param     re         天体半径
    J2Analytical(const ModOrbElem& modOrbElem, const TimePoint& epoch, double gm, double j2, double re);

    /// @brief     构造函数
    /// @details   ~
    /// @param     pos        位置向量
    /// @param     vel        速度向量
    /// @param     epoch      参考时间点
    /// @param     gm         引力常数
    /// @param     j2         J2项
    /// @param     re         天体半径
    J2Analytical(const Vector3d& pos, const Vector3d& vel, const TimePoint& epoch, double gm, double j2, double re);

    /// @brief     轨道预报函数
    /// @details   ~
    /// @param     duration 时间间隔
    /// @param     gm       引力常数
    /// @param     j2       J2项
    /// @param     r        位置向量
    /// @param     v        速度向量
    /// @return    错误码
    err_t prop(double duration, Vector3d& r, Vector3d& v);

    /// @brief     轨道预报函数
    /// @details   ~
    /// @param     duration 时间间隔
    /// @param     gm       引力常数
    /// @param     j2       J2项
    /// @param     modOrbElem 改进轨道根数
    /// @return    错误码
    err_t prop(double duration, ModOrbElem& modOrbElem);


    /// @brief     轨道预报函数
    /// @details   ~
    /// @param     epoch    时间点
    /// @param     gm       引力常数
    /// @param     j2       J2项
    /// @param     r        位置向量
    /// @param     v        速度向量
    /// @return    错误码
    err_t prop(const TimePoint& epoch, Vector3d& r, Vector3d& v);

    /// @brief     轨道预报函数
    /// @details   ~
    /// @param     epoch    时间点
    /// @param     gm       引力常数
    /// @param     j2       J2项
    /// @param     modOrbElem 改进轨道根数
    /// @return    错误码
    err_t prop(const TimePoint& epoch, ModOrbElem& modOrbElem);
protected:
    /// @brief     轨道预报函数（不规范化角度）
    /// @details   ~
    /// @param     duration 时间间隔
    /// @param     modOrbElem 改进轨道根数
    /// @return    错误码
    err_t propNoNormalize(double duration, ModOrbElem& modOrbElem);


protected:
    ModOrbElem modOrbElem_;  ///< 改进轨道根数
    TimePoint epoch_;        ///< 参考时间点
    double gm_;              ///< 引力常数
    double j2_;              ///< J2项
    double re_;              ///< 天体半径
    double raanDot_;         ///< 升交点角速度
    double argPeriDot_;      ///< 近心点角速度
    double meanAnomalyDot_;  ///< 平近点角速度
};


/// @brief     ~
/// @details   ~
/// @param     duration 时间间隔
/// @param     gm       引力常数
/// @param     j2       J2项
/// @param     r        位置向量
/// @param     v        速度向量
/// @return    错误码
AST_CORE_API err_t aJ2AnalyticalProp(double duration, double gm, double j2, double re, Vector3d& r, Vector3d& v);



AST_CORE_API err_t aJ2AnalyticalProp(double duration, double gm, double j2, double re,ModOrbElem& modOrbElem);


AST_NAMESPACE_END

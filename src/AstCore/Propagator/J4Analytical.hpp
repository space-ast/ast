///
/// @file      J4Analytical.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-05
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
#include "J2J4Analytical.hpp"

AST_NAMESPACE_BEGIN


/*!
    @addtogroup Propagator
    @{
*/

class ModOrbElem;

class AST_CORE_API J4Analytical: public J2J4Analytical
{
public:
    /// @brief     构造函数
    /// @details   ~
    /// @param     modOrbElem 改进轨道根数
    /// @param     epoch      历元
    /// @param     gm         引力常数
    /// @param     j2         J2项
    /// @param     j4         J4项
    /// @param     re         天体半径
    J4Analytical(const ModOrbElem& modOrbElem, const TimePoint& epoch, double gm, double j2, double j4, double re);

    /// @brief     构造函数
    /// @details   ~
    /// @param     pos       位置向量
    /// @param     vel       速度向量
    /// @param     epoch     历元
    /// @param     gm        引力常数
    /// @param     j2        J2项
    /// @param     j4        J4项
    /// @param     re        天体半径
    J4Analytical(const Vector3d& pos, const Vector3d& vel, const TimePoint& epoch, double gm, double j2,  double j4, double re);

private:
    // double j2_;
    // double j4_;
    // double re_;
};

/// @brief     J4轨道预报
/// @details   
/// @param     duration 时间间隔
/// @param     gm       引力常数
/// @param     j2       J2项
/// @param     j4       J4项
/// @param     re       天体半径
/// @param     r        位置向量
/// @param     v        速度向量
/// @return    错误码
AST_CORE_API err_t aJ4AnalyticalProp(double duration, double gm, double j2, double j4, double re, Vector3d& r, Vector3d& v);


/// @brief     J4轨道预报
/// @details   
/// @param     duration 时间间隔
/// @param     gm       引力常数
/// @param     j2       J2项
/// @param     j4       J4项
/// @param     re       天体半径
/// @param     modOrbElem 改进轨道根数
/// @return    错误码
AST_CORE_API err_t aJ4AnalyticalProp(double duration, double gm, double j2, double j4, double re, ModOrbElem& modOrbElem);



/*! @} */


AST_NAMESPACE_END

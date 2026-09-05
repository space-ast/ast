///
/// @file      Vinti.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-09-01
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

#include "Vinti.hpp"
#include "AstCore/Vinti6.h"
#include "AstMath/Vector.hpp"
#include <cmath>                    // for std::isnan
#include <limits>                   // for std::numeric_limits

AST_NAMESPACE_BEGIN

errc_t aVinti(double duration, double gm, double j2, double j3, double re, Vector3d &r, Vector3d &v)
{
    double planet[4] = {re, gm, j2, j3};
    double x0[6] = {r.x(), r.y(), r.z(), v.x(), v.y(), v.z()};
    // 用nan检查是否成功赋值x1数组，若未赋值，则说明算法提前退出，返回错误
    double x1[6] = {std::numeric_limits<double>::quiet_NaN()};
    double oe[6] = {};
    Vinti6(planet, 0.0, x0, duration, x1, oe);
    r = {x1[0], x1[1], x1[2]};
    v = {x1[3], x1[4], x1[5]};
    if(std::isnan(r.norm()) || std::isnan(v.norm()))
        return eErrorInvalidParam;
    return eNoError;
}


AST_NAMESPACE_END

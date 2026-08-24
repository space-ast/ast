///
/// @file      CloseApproachFilter.hpp
/// @brief     接近分析预过滤抽象
/// @author    axel
/// @date      2026-08-19
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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
#include "AstCore/TLE.hpp"
#include "AstCore/OrbitElement.hpp"

AST_NAMESPACE_BEGIN

struct CloseApproachOptions;

/// @brief 预过滤共享上下文
/// @details 由编排器在每次分析时构造，供各级过滤器共享参考对象根数与常量。
struct CloseApproachContext
{
    const TLE* ref{nullptr};  ///< 参考对象 TLE
    OrbElem    refElem{};     ///< 参考对象二体椭圆根数（a,e,i,raan,argper）
    double     gm{0.0};       ///< 引力常数 [m^3/s^2]
};

/// @brief 预过滤抽象基类
/// @details 每个过滤器判定候选对象是否应保留进入后续处理。
///          过滤器必须保守：允许保留非接近对象（假阳性），但不得误删真实接近（假阴性）。
class AST_CORE_API CloseApproachFilter
{
public:
    virtual ~CloseApproachFilter() = default;

    /// @brief 判定候选对象是否保留
    /// @param candidate 候选对象 TLE
    /// @return true = 保留，false = 剔除
    virtual bool keep(const TLE& candidate) const = 0;
};

AST_NAMESPACE_END

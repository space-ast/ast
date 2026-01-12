///
/// @file      NutationSeries.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-10
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
#include "NutationTerm.hpp"
#include "AstUtil/Polynomial.hpp"
#include <vector>

AST_NAMESPACE_BEGIN

class BKVParser;

class AST_CORE_API NutationSeries{
public:
    /// @brief 从文件加载nutation series
    /// @param filepath 文件路径
    /// @return err_t
    err_t load(StringView filepath);

    /// @brief 从STK文件加载nutation series
    /// @param filepath STK文件路径
    /// @return err_t
    err_t loadSTK(StringView filepath);

    /// @brief 从IERS文件加载nutation series
    /// @param filepath IERS文件路径
    /// @return err_t
    err_t loadIERS(StringView filepath);


    double eval();
protected:
    
    err_t loadSTK(BKVParser& parser);

    err_t loadIERS(BKVParser& parser);

protected:
    Polynomial polynomial_;             ///< nutation series多项式
    std::vector<NutationTerm> terms_;   ///< 
    std::vector<int> jlist_;         ///< 
};


AST_NAMESPACE_END

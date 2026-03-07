///
/// @file      SpiceBodyRegistry.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-06
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
#include "AstCore/CelestialBody.hpp"
#include <unordered_map>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Spice
    @{
*/

class AST_SPICE_API SpiceBodyRegistry
{
public:
    SpiceBodyRegistry() = default;
    SpiceBodyRegistry(bool whetherInit);
    ~SpiceBodyRegistry() = default;
    static SpiceBodyRegistry& Instance();

    PBody findBody(StringView name) const;
    PBody findBody(int id) const;
    err_t init();
protected:
    using BodyNameMap = std::unordered_map<std::string, HBody>;
    using BodyIDMap = std::unordered_map<int, HBody>;
    BodyNameMap bodyMap_;
    BodyIDMap   bodyIDMap_;
};

/*! @} */

AST_NAMESPACE_END

///
/// @file      Database.hpp
/// @brief     数据库抽象基类
/// @details   作为所有数据库实现类的公共基类，用于类型标识和统一引用。
/// @author    axel
/// @date      2026-07-23
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

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 数据库抽象基类
/// @details 作为数据库实现的公共基类，用于类型统一标识。
///          派生类负责实现具体的数据加载和查询功能。
class AST_CORE_API Database
{
public:
    Database() = default;
    virtual ~Database() = default;
};

/*! @} */

AST_NAMESPACE_END

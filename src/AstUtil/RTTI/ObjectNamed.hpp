///
/// @file      ObjectNames.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-10
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
#include "Object.hpp"
#include "AstUtil/StringView.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 命名对象
class AST_UTIL_API ObjectNamed: public Object
{
public:
    /// @brief 默认构造函数
    ObjectNamed() = default;

    /// @brief 构造函数
    /// @param name 对象名称
    explicit ObjectNamed(StringView name)
        : name_(name)
    {}

    /// @brief 获取对象名称
    const std::string& getName() const override { return name_; }

    /// @brief 设置对象名称
    void setName(StringView name) override { name_ = std::string(name); }
private:
    std::string name_;      ///< 对象名称
};


/*! @} */

AST_NAMESPACE_END

///
/// @file      BKVItem.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-08
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
#include "BKVNode.hpp"
#include "AstUtil/GenericValue.hpp"
#include "AstUtil/ValueView.hpp"
#include <string>

AST_NAMESPACE_BEGIN

/// @brief 键值对项节点（BlockKeyValueItemNode）
/// @ingroup ParseFormat
class BKVItem : public BKVNode
{
public:
    BKVItem() : BKVNode(eItem) {}
    /// @brief 键值对项节点（BlockKeyValueItemNode）
    BKVItem(StringView key, ValueView value)
        : BKVNode{eItem}
        , key_(key.to_string())
        , value_(value.toValue()) 
    {}
    
public:
    std::string key_;       // 键
    GenericValue value_;    // 值
};



AST_NAMESPACE_END

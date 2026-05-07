///
/// @file      ValMap.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-15
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
#include "Value.hpp"
#include "AstUtil/OrderedMap.hpp"
#include <string>
#include <map>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 字典值
class AST_SCRIPT_API ValDict: public Value
{
    using MapType = OrderedMap<std::string, SharedPtr<Value>>;
public:
    AST_EXPR(ValDict)
    static ValDict* New();

    ValDict() = default;
    ~ValDict() override = default;
    Value* find(const std::string& name);
    void insert(const std::string& name, Value* value);
    using Value::insert;
    std::string toJsonString() const;
    std::string toJsonString(int indent) const;
    std::string getExpression(Object* context=nullptr) const override;
    const ValueMapType& getMap() const{return map_;}
protected:
    std::string toJsonString(int indent, int currentIndent) const;
protected:
    MapType map_;
};

/*! @} */

AST_NAMESPACE_END
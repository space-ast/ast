///
/// @file      ValNamedVector.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-01
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

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 命名向量值类
/// @todo 完善命名向量值类的实现 
class AST_SCRIPT_API ValNamedVector: public Value
{
public:
    using ItemType = std::pair<std::string, SharedPtr<Value>>;
    using VectorType = std::vector<ItemType>;
    static ValNamedVector* New();

    AST_EXPR(ValNamedVector)

    ValNamedVector() = default;
    ~ValNamedVector() = default;
    /// @brief 获取向量元素数量
    /// @return size_t 向量元素数量
    size_t size() const{return vector_.size();}


    VectorType& getVector() {return vector_;}
    const VectorType& getVector() const {return vector_;}

    std::string getExpression(Object* object) const override;
private:
    VectorType vector_;
};


/*! @} */

AST_NAMESPACE_END


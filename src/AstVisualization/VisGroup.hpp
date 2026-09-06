///
/// @file      VisGroup.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-09-06
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
#include "VisObject.hpp"
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/


class AST_VISUALIZATION_API VisGroup final: public VisObject
{
public:
    VisGroup() = default;
    ~VisGroup() = default;
    A_DISABLE_COPY(VisGroup)

    /// @brief 访问者分派
    void accept(VisVisitor& visitor) override;

    /// @brief 添加子对象（拥有权移交给本组）
    /// @param object 子对象指针；可能为 nullptr
    void add(VisObject* object);

    /// @brief 添加子对象
    /// @param object 子对象句柄
    void add(const HVisObject& object);

    /// @brief 子对象列表
    const std::vector<HVisObject>& children() const { return children_; }

    /// @brief 子对象数量
    size_t size() const { return children_.size(); }

    /// @brief 是否为空
    bool empty() const { return children_.empty(); }

    /// @brief 预留子对象容量
    void reserve(size_t count) { children_.reserve(count); }

private:
    std::vector<HVisObject> children_;
};


/*! @} */

AST_NAMESPACE_END

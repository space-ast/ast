///
/// @file      PropertyObjectList.hpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-11-05
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
#include "Property.hpp"
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class AST_UTIL_API PropertyObjectList: public Property
{
public:
    PropertyObjectList(FPropertyGetSize getSize, FPropertyGetItemByIndex getItemByIndex, FPropertyReplaceItems replaceItems, Class* elementClass);
public:
    errc_t accept(PropertyVisitor& visitor, const void* container) override;
    errc_t getValueDouble(const void* container, double& value) override;
    errc_t setValueDouble(void* container, double value) override;
    errc_t getValueInt(const void* container, int& value) override;
    errc_t setValueInt(void* container, int value) override;
    errc_t getValueBool(const void* container, bool& value) override;
    errc_t setValueBool(void* container, bool value) override;
    errc_t getValueString(const void* container, std::string& value) override;
    errc_t setValueString(void* container, StringView value) override;
    EValueType getValueType() const override;
public:
    size_t getSize(const void* container);

    errc_t getItem(const void* container, size_t index, Object*& value);
    errc_t getItems(const void* container, std::vector<Object*>& items);

    errc_t setItem(void* container, size_t index, Object* item);
    errc_t setItems(void* container, Span<Object*> items);

    errc_t insertItem(void* container, size_t index, Object* item);
    errc_t appendItem(void* container, Object* item);

    errc_t removeItem(void* container, size_t index);
    errc_t clearItems(void* container);
private:
    FPropertyGetSize getSize_{nullptr};
    FPropertyGetItemByIndex getItemByIndex_{nullptr};
    FPropertyReplaceItems replaceItems_{nullptr};
    Class* elementClass_{nullptr};       ///< 元素类型
};


/*! @} */

AST_NAMESPACE_END

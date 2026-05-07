///
/// @file      PropertyQuantity.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-18
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
#include "AstUtil/Quantity.hpp"
#include "AstUtil/PropertyDouble.hpp"
#include "AstUtil/Dimension.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup RTTI
    @{
*/


/// @brief 数量值属性
/// @details 数量值属性，包含属性的名称、描述等信息
/// @ingroup RTTI
class AST_UTIL_API PropertyQuantity: public PropertyDouble
{
public:
    PropertyQuantity(FPropertyGet getter, FPropertySet setter, Dimension dimension);
    PropertyQuantity(StringView name, StringView desc, FPropertyGet getter, FPropertySet setter, Dimension dimension);
    
    errc_t accept(PropertyVisitor& visitor, const void* container) override;
    errc_t getValueString(const void* container, std::string& value) override;
    errc_t setValueString(void* container, StringView value) override;
    EValueType getValueType() const override{return EValueType::eQuantity;}
protected:
    Dimension dimension_{Dimension::Unit()};   ///< 数量值属性的量纲
};

/*! @} */

AST_NAMESPACE_END

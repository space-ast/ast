///
/// @file      PropertyBool.hpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-09
/// @copyright 版权所有 (C) 2025-present, ast项目.
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
#include "Property.hpp"

AST_NAMESPACE_BEGIN


/// @brief 反射属性类（bool类型）
class PropertyBool final: public Property
{
public:
    using Property::Property;
    
    err_t getValueBool(void* container, bool& value) override;
    err_t setValueBool(void* container, bool value) override;
    err_t getValueInt(void* container, int& value) override;
    err_t setValueInt(void* container, int value) override;
    err_t getValueString(void* container, std::string& value) override;
    err_t setValueString(void* container, StringView value) override;
    err_t getValueDouble(void* container, double& value) override;
    err_t setValueDouble(void* container, double value) override;
};


AST_NAMESPACE_END
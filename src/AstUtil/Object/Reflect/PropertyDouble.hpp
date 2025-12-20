///
/// @file      PropertyDouble.hpp
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

/// @brief 双精度浮点数属性
class PropertyDouble: public Property
{
public:
    using Property::Property;
    using InputType = double;
    using OutputType = double;

    err_t getValueBool(const void* container, bool& value) override;
    err_t setValueBool(void* container, bool value) override;
    err_t getValueInt(const void* container, int& value) override;
    err_t setValueInt(void* container, int value) override;
    err_t getValueString(const void* container, std::string& value) override;
    err_t setValueString(void* container, StringView value) override;
    err_t getValueDouble(const void* container, double& value) override;
    err_t setValueDouble(void* container, double value) override;
protected:
    /// @brief 设置属性值（double类型）
    /// @param container 容器指针
    /// @param value 属性值指针
    /// @return 0 成功，其他值 失败
    A_ALWAYS_INLINE
    err_t setValue(void* container, const InputType* value)
    {
        return setter_(container, value);
    }
    /// @brief 获取属性值（double类型）
    /// @param container 容器指针
    /// @param value 属性值指针
    /// @return 0 成功，其他值 失败
    A_ALWAYS_INLINE
    err_t getValue(const void* container, OutputType* value)
    {
        return getter_(container, value);
    }

};


AST_NAMESPACE_END
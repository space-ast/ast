///
/// @file      PropertyTimePoint.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-29
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
#include "AstUtil/Property.hpp"
#include "AstCore/TimePoint.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup RTTI
    @{
*/

/// @brief 时间点属性
/// @details 时间点属性，包含属性的名称、描述等信息
class AST_CORE_API PropertyTimePoint : public Property
{
public:
    using InputType  = TimePoint;
    using OutputType = TimePoint;
public:
    using Property::Property;
    using Property::getValue;
    errc_t getValueBool(const void* container, bool& value) override;
    errc_t setValueBool(void* container, bool value) override;
    errc_t getValueInt(const void* container, int& value) override;
    errc_t setValueInt(void* container, int value) override;
    errc_t getValueDouble(const void* container, double& value) override;
    errc_t setValueDouble(void* container, double value) override;
    errc_t getValueString(const void* container, std::string& value) override;
    errc_t setValueString(void* container, StringView value) override;
    EValueType getValueType() const override{return EValueType::eTimePoint;}
    
    /// @brief 接受访问者
    /// @param visitor 访问者对象
    /// @param container 容器对象指针
    /// @return errc_t 错误码
    errc_t accept(PropertyVisitor& visitor, const void* container) override;
public:
    /// @brief 设置属性值（时间点类型）
    /// @param container 容器指针
    /// @param value 属性值指针
    /// @return 0 成功，其他值 失败
    errc_t setValue(void* container, const InputType* value)
    {
        return setter_(container, value);
    }
    /// @brief 获取属性值（时间点类型）
    /// @param container 容器指针
    /// @param value 属性值指针
    /// @return 0 成功，其他值 失败
    errc_t getValue(const void* container, OutputType* value)
    {
        return getter_(container, value);
    }
};

/*! @} */

AST_NAMESPACE_END
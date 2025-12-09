///
/// @file      Property.hpp
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
#include "Field.hpp"

AST_NAMESPACE_BEGIN

typedef err_t (*FPropertyGet)(void* obj, void* value);      ///< 获取属性值的函数指针
typedef err_t (*FPropertySet)(void* obj, void* value);      ///< 设置属性值的函数指针

enum EValueType{
    eDouble,        ///< double类型
    eInt,           ///< int类型
    eBool,          ///< bool类型
    eString,        ///< std::string类型
};



/// @brief 反射属性类
class Property: public Field
{
public:
    /// @brief 属性标志位
    enum EPropertyFlag{
        eReadable = 0x01,   ///< 可读属性
        eWritable = 0x02,   ///< 可写属性
    };
public:
    Property(StringView name, StringView desc = "", FPropertyGet getter = nullptr, FPropertySet setter = nullptr)
        : Field(name, desc)
        , getter_(getter)
        , setter_(setter)
    {}
    Property() = default;
    ~Property() = default;

    /// @param container 对象指针
    /// @param value 指向double类型变量的引用，用于存储属性值
    /// @return err_t 错误码
    virtual err_t getValueDouble(void* container, double& value) = 0;

    /// @brief 设置属性值（double类型）
    /// @param container 对象指针
    /// @param value double类型变量，用于设置属性值
    /// @return err_t 错误码
    virtual err_t setValueDouble(void* container, double value) = 0;

    /// @brief 获取属性值（int类型）
    /// @param container 对象指针
    /// @param value 指向int类型变量的引用，用于存储属性值
    /// @return err_t 错误码
    virtual err_t getValueInt(void* container, int& value) = 0;

    /// @brief 设置属性值（int类型）
    /// @param container 对象指针
    /// @param value int类型变量，用于设置属性值
    /// @return err_t 错误码
    virtual err_t setValueInt(void* container, int value) = 0;

    /// @brief 获取属性值（bool类型）
    /// @param container 对象指针
    /// @param value 指向bool类型变量的引用，用于存储属性值
    /// @return err_t 错误码
    virtual err_t getValueBool(void* container, bool& value) = 0;

    /// @brief 设置属性值（bool类型）
    /// @param container 对象指针
    /// @param value bool类型变量，用于设置属性值
    /// @return err_t 错误码
    virtual err_t setValueBool(void* container, bool value) = 0;

    /// @brief 获取属性值（String类型）
    /// @param container 对象指针
    /// @param value 指向std::string类型变量的引用，用于存储属性值
    /// @return err_t 错误码
    virtual err_t getValueString(void* container, std::string& value) = 0;

    /// @brief 设置属性值（String类型）
    /// @param container 对象指针
    /// @param value StringView类型变量，用于设置属性值
    /// @return err_t 错误码
    virtual err_t setValueString(void* container, StringView value) = 0;

protected:
    /// @brief 获取属性值
    /// @param container 对象指针
    /// @param value 指向void类型变量的指针，用于存储属性值
    /// @return err_t 错误码
    A_ALWAYS_INLINE
    err_t getValue(void* container, void* value)
    {
        return getter_(container, value);
    }

    /// @brief 设置属性值
    /// @param container 对象指针
    /// @param value 指向void类型变量的指针，用于设置属性值
    /// @return err_t 错误码
    A_ALWAYS_INLINE
    err_t setValue(void* container, void* value)
    {
        return setter_(container, value);
    }
protected:
    FPropertyGet getter_ {nullptr};                                 // 获取属性值的函数指针
    FPropertySet setter_ {nullptr};                                 // 设置属性值的函数指针
};

AST_NAMESPACE_END

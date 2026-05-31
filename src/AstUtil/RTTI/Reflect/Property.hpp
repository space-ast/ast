///
/// @file      Property.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
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
#include "ReflectAPI.hpp"

AST_NAMESPACE_BEGIN

class PropertyVisitor;

/*!
    @addtogroup RTTI
    @{
*/


enum class EValueType
{
    eBool,          ///< bool类型
    eInt,           ///< int类型
    eDouble,        ///< double类型
    eQuantity,      ///< 数量值类型
    eString,        ///< std::string类型
    eObject,        ///< 对象类型
    eTimePoint,     ///< 时间点类型
    eInvalid,       ///< 无效类型
};


/// @brief 模拟获取属性值的函数
/// @param container 对象指针
/// @param value 指向属性值的指针
/// @return errc_t 错误码
AST_UTIL_API errc_t aFakeGet(const void* container, void* value);


/// @brief 模拟设置属性值的函数
/// @param container 对象指针
/// @param value 指向属性值的指针
/// @return errc_t 错误码
AST_UTIL_API errc_t aFakeSet(void* container, const void* value);



/// @brief 反射属性类
class AST_UTIL_API Property: public Field
{
public:
    /// @brief 属性标志位
    enum EPropertyFlag{
        eReadable = 0x01,   ///< 可读属性
        eWritable = 0x02,   ///< 可写属性
    };
public:
    Property()
        : Field()
        , getter_(&aFakeGet)
        , setter_(&aFakeSet)
    {}
    Property(StringView name, StringView desc, FPropertyGet getter, FPropertySet setter)
        : Field(name, desc)
        , getter_(getter?getter:&aFakeGet)
        , setter_(setter?setter:&aFakeSet)
    {}
    Property(FPropertyGet getter, FPropertySet setter)
        : Field()
        , getter_(getter?getter:&aFakeGet)
        , setter_(setter?setter:&aFakeSet)
    {}
    ~Property() = default;
    
    /// @brief 接受访问者
    /// @param visitor 访问者对象
    /// @param container 容器对象指针
    /// @return errc_t 错误码
    virtual errc_t accept(PropertyVisitor& visitor, const void* container) = 0;

    /// @param container 容器对象指针
    /// @param value 指向double类型变量的引用，用于存储属性值
    /// @return errc_t 错误码
    virtual errc_t getValueDouble(const void* container, double& value) = 0;

    /// @brief 设置属性值（double类型）
    /// @param container 容器对象指针
    /// @param value double类型变量，用于设置属性值
    /// @return errc_t 错误码
    virtual errc_t setValueDouble(void* container, double value) = 0;

    /// @brief 获取属性值（int类型）
    /// @param container 容器对象指针
    /// @param value 指向int类型变量的引用，用于存储属性值
    /// @return errc_t 错误码
    virtual errc_t getValueInt(const void* container, int& value) = 0;

    /// @brief 设置属性值（int类型）
    /// @param container 容器对象指针
    /// @param value int类型变量，用于设置属性值
    /// @return errc_t 错误码
    virtual errc_t setValueInt(void* container, int value) = 0;

    /// @brief 获取属性值（bool类型）
    /// @param container 容器对象指针
    /// @param value 指向bool类型变量的引用，用于存储属性值
    /// @return errc_t 错误码
    virtual errc_t getValueBool(const void* container, bool& value) = 0;

    /// @brief 设置属性值（bool类型）
    /// @param container 容器对象指针
    /// @param value bool类型变量，用于设置属性值
    /// @return errc_t 错误码
    virtual errc_t setValueBool(void* container, bool value) = 0;

    /// @brief 获取属性值（String类型）
    /// @param container 容器对象指针
    /// @param value 指向std::string类型变量的引用，用于存储属性值
    /// @return errc_t 错误码
    virtual errc_t getValueString(const void* container, std::string& value) = 0;

    /// @brief 设置属性值（String类型）
    /// @param container 容器对象指针
    /// @param value StringView类型变量，用于设置属性值
    /// @return errc_t 错误码
    virtual errc_t setValueString(void* container, StringView value) = 0;


    /// @brief 获取属性值类型
    /// @return EValueType 属性值类型
    virtual EValueType getValueType() const = 0;
public:
    double getValueDouble(const void* container);
    int getValueInt(const void* container);
    bool getValueBool(const void* container);
    std::string getValueString(const void* container);
    
    template<typename T>
    T getValue(const void* container);
public:
    /// @brief 检查属性是否为对象类型
    /// @return bool 是否为对象类型
    bool isObject() const;

    /// @brief 设置属性值（对象类型）
    errc_t setValueObject(void* container, Object* value);

    /// @brief 获取属性值（对象类型）
    errc_t getValueObject(const void* container, Object*& value);

public:
    /// @brief 检查属性是否为只读属性
    bool readOnly() const { return setter_ == nullptr && getter_ != nullptr; }
    /// @brief 检查属性是否为只写属性
    bool writeOnly() const { return setter_ != nullptr && getter_ == nullptr; }
protected:
    /// @brief 获取属性值
    /// @param container 容器对象指针
    /// @param value 指向void类型变量的指针，用于存储属性值
    /// @return errc_t 错误码
    A_ALWAYS_INLINE
    errc_t getValue(void* container, void* value)
    {
        return getter_(container, value);
    }

    /// @brief 设置属性值
    /// @param container 容器对象指针
    /// @param value 指向void类型变量的指针，用于设置属性值
    /// @return errc_t 错误码
    A_ALWAYS_INLINE
    errc_t setValue(void* container, const void* value)
    {
        return setter_(container, value);
    }
protected:
    FPropertyGet getter_ {nullptr};                                 // 获取属性值的函数指针
    FPropertySet setter_ {nullptr};                                 // 设置属性值的函数指针
};


template<>
inline double Property::getValue<double>(const void* container)
{
    return getValueDouble(container);
}
template<>
inline int Property::getValue<int>(const void* container)
{
    return getValueInt(container);
}
template<>
inline bool Property::getValue<bool>(const void* container)
{
    return getValueBool(container);
}

template<>
inline std::string Property::getValue<std::string>(const void* container)
{
    return getValueString(container);
}



/*! @} */

AST_NAMESPACE_END
///
/// @file      Attribute.hpp
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
#include "AstUtil/Property.hpp"
#include "AstUtil/SharedPtr.hpp"
#include "AstUtil/WeakPtr.hpp"
#include "AstUtil/Object.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


class Property;
class Object;
/// @brief 对象的特定属性
template<typename ObjectPtrType=WeakPtr<Object>, typename PropertyType=Property>
class AttributeBasic
{
public:
    AttributeBasic() = default;
    
    AttributeBasic(ObjectPtrType object, PropertyType* property)
        : object_(object)
        , property_(property)
    {
    }
    
    bool isValid() const {return object_ && property_;}

    EValueType getValueType() const {
        if(!property_) return EValueType::eInvalid;
        return property_->getValueType();
    }

    errc_t getValueDouble(double& value) const
    {
        auto object = getObject();
        if(!property_ || !object) return eErrorInvalidParam;
        return property_->getValueDouble(object, value);
    }
    errc_t setValueDouble(double value)
    {
        auto object = getObject();
        if(!property_ || !object) return eErrorInvalidParam;
        return property_->setValueDouble(object, value);
    }
    errc_t getValueInt(int& value) const
    {
        auto object = getObject();
        if(!property_ || !object) return eErrorInvalidParam;
        return property_->getValueInt(object, value);
    }
    errc_t setValueInt(int value)
    {
        auto object = getObject();
        if(!property_ || !object) return eErrorInvalidParam;
        return property_->setValueInt(object, value);
    }
    errc_t getValueBool(bool& value) const
    {
        auto object = getObject();
        if(!property_ || !object) return eErrorInvalidParam;
        return property_->getValueBool(object, value);
    }
    errc_t setValueBool(bool value)
    {
        auto object = getObject();
        if(!property_ || !object) return eErrorInvalidParam;
        return property_->setValueBool(object, value);
    }
    errc_t getValueString(std::string& value) const
    {
        auto object = getObject();  
        if(!property_ || !object) return eErrorInvalidParam;
        return property_->getValueString(object, value);
    }
    errc_t setValueString(StringView value)
    {
        auto object = getObject();
        if(!property_ || !object) return eErrorInvalidParam;
        return property_->setValueString(object, value);
    }
    errc_t getValueObject(Object*& value) const
    {
        auto object = getObject();
        if(!property_ || !object) return eErrorInvalidParam;
        return property_->getValueObject(object, value);
    }
    errc_t setValueObject(Object* value)
    {
        auto object = getObject();
        if(!property_ || !object) return eErrorInvalidParam;
        return property_->setValueObject(object, value);
    }
public:
    double getValueDouble() const
    {
        double value = 0.0;
        getValueDouble(value);
        return value;
    }
    int getValueInt() const
    {
        int value = 0;
        getValueInt(value);
        return value;
    }
    bool getValueBool() const
    {
        bool value = false;
        getValueBool(value);
        return value;
    }
    std::string getValueString() const
    {
        std::string value;
        getValueString(value);
        return value;
    }
    Object* getValueObject() const
    {
        Object* value = nullptr;
        getValueObject(value);
        return value;
    }
public:
    operator double() const
    {
        return getValueDouble();
    }
    operator int() const
    {
        return getValueInt();
    }
    operator bool() const
    {
        return getValueBool();
    }
    operator std::string() const
    {
        return getValueString();
    }
public:
    AttributeBasic& operator=(double value)
    {
        setValueDouble(value);
        return *this;
    }
    AttributeBasic& operator=(int value)
    {
        setValueInt(value);
        return *this;
    }
    // 避免对bool类型的隐式转换
    template<typename T, typename = typename std::enable_if<std::is_same<T, bool>::value>::type>
    AttributeBasic& operator=(T value)
    {
        setValueBool(value);
        return *this;
    }
    AttributeBasic& operator=(StringView value)
    {
        setValueString(value);
        return *this;
    }
protected:
    void* getObject() const;
protected:
    ObjectPtrType object_{};
    PropertyType* property_{nullptr};
};

template<>
inline void* AttributeBasic<WeakPtr<Object>, Property>::getObject() const
{
    return object_.get();
}

template<typename ObjectPtrType, typename PropertyType>
inline void* AttributeBasic<ObjectPtrType, PropertyType>::getObject() const
{
    return object_;
}

// typedef AttributeBasic<> Attribute;

/*! @} */

AST_NAMESPACE_END

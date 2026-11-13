///
/// @file      PropertyVisitor.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-30
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
#include "AstUtil/StringView.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/
class Property;
class PropertyBool;
class PropertyDouble;
class PropertyInt;
class PropertyString;
class PropertyObject;
class PropertyObjectList;
class PropertyStruct;
class PropertyQuantity;
class PropertyPOD;
class PropertyTimePoint;
class Object;

class PropertyVisitor {
public:
    virtual ~PropertyVisitor() = default;
    
    /// @brief 访问属性（默认方法）
    /// @param property 属性对象
    /// @param container 容器对象指针
    /// @return errc_t 错误码
    virtual errc_t visit(Property& property, const void* container) = 0;
    
    /// @brief 访问Bool类型属性
    /// @param property Bool类型属性对象
    /// @param container 容器对象指针
    /// @return errc_t 错误码
    virtual errc_t visit(PropertyBool& property, const void* container) = 0;
    
    /// @brief 访问Double类型属性
    /// @param property Double类型属性对象
    /// @param container 容器对象指针
    /// @return errc_t 错误码
    virtual errc_t visit(PropertyDouble& property, const void* container) = 0;
    
    /// @brief 访问Int类型属性
    /// @param property Int类型属性对象
    /// @param container 容器对象指针
    /// @return errc_t 错误码
    virtual errc_t visit(PropertyInt& property, const void* container) = 0;
    
    /// @brief 访问String类型属性
    /// @param property String类型属性对象
    /// @param container 容器对象指针
    /// @return errc_t 错误码
    virtual errc_t visit(PropertyString& property, const void* container) = 0;
    
    /// @brief 访问Object类型属性
    /// @param property Object类型属性对象
    /// @param container 容器对象指针
    /// @return errc_t 错误码
    virtual errc_t visit(PropertyObject& property, const void* container) = 0;
    
    /// @brief 访问Struct类型属性
    /// @param property Struct类型属性对象
    /// @param container 容器对象指针
    /// @return errc_t 错误码
    virtual errc_t visit(PropertyStruct& property, const void* container) = 0;
    
    /// @brief 访问Quantity类型属性
    /// @param property Quantity类型属性对象
    /// @param container 容器对象指针
    /// @return errc_t 错误码
    virtual errc_t visit(PropertyQuantity& property, const void* container) = 0;
    
    /// @brief 访问POD类型属性
    /// @param property POD类型属性对象
    /// @param container 容器对象指针
    /// @return errc_t 错误码
    virtual errc_t visit(PropertyPOD& property, const void* container) = 0;
    
    /// @brief 访问TimePoint类型属性
    /// @param property TimePoint类型属性对象
    /// @param container 容器对象指针
    /// @return errc_t 错误码
    virtual errc_t visit(PropertyTimePoint& property, const void* container) = 0;

    /// @brief 访问ObjectList类型属性
    /// @param property ObjectList类型属性对象
    /// @param container 容器对象指针
    /// @return errc_t 错误码
    virtual errc_t visit(PropertyObjectList& property, const void* container) {A_UNUSED(property); A_UNUSED(container); return 0;}
    
};

/*! @} */

AST_NAMESPACE_END
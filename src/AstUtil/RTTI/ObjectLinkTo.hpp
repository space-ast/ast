///
/// @file      ObjectLinkTo.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-10
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
#include "ObjectNamed.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 链接到对象
/// @details 用于链接对象
class AST_UTIL_API ObjectLinkTo final: public ObjectNamed
{
public:
    AST_OBJECT(ObjectLinkTo)

    ObjectLinkTo() = default;

    ObjectLinkTo(const ObjectLinkTo&) = delete;
    ObjectLinkTo& operator=(const ObjectLinkTo&) = delete;

    ~ObjectLinkTo() = default;

    void setResolvedName(const std::string& name);
    const std::string& getResolvedName() const;

    void setResolvedType(Class* type);
    Class* getResolvedType() const;
    
    void setResolvedObject(Object* object){resolvedObject_ = object;}
    
    Object* resolve() const;
private:
    mutable WeakPtr<Object> resolvedObject_{};        ///< 已解析的对象
    std::string resolvedName_{};                      ///< 用于解析的名称
    Class* resolvedType_{nullptr};                  ///< 用于解析的类型
};


/*! @} */

AST_NAMESPACE_END



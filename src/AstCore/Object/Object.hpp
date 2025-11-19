///
/// @file      Object.hpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      15.11.2025
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
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。
 
#pragma once
 
#include "AstGlobal.hpp"
#include <string>
#include <stdint.h>     // for uint32_t

AST_NAMESPACE_BEGIN
 
class Type;

class AST_CORE_API Object
{
public:
    Object(Type* tp)
        :m_type{tp}
        ,m_refcnt{0}
        ,m_weakrefcnt{1}
    {}
    virtual ~Object(){}
    
    err_t getAttrString(const std::string& path, std::string& value) const;
    
    uint32_t refCount() const{return m_refcnt;}
    uint32_t weakRefCount() const{return m_weakrefcnt;}

protected:
    Type*       m_type;                 ///< 类型元信息
    uint32_t    m_refcnt;               ///< 强引用计数
    uint32_t    m_weakrefcnt;           ///< 弱引用计数
};


AST_NAMESPACE_END

AST_DECL_TYPE_ALIAS(Object)



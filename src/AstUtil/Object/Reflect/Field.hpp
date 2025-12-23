///
/// @file      Field.hpp
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
#include <string>                   // for std::string
#include "AstUtil/StringView.hpp"   // for StringView

AST_NAMESPACE_BEGIN


/// @brief 反射字段类
class Field
{
public:
    Field(){}

    Field(StringView name, StringView desc = ""): name_(name), desc_(desc){}
    
    virtual ~Field() = default;
    
    /// @brief 获取字段名称
    const std::string& name() const{return name_;}
    /// @brief 获取字段描述
    const std::string& desc() const{return desc_;}
protected:
    //@todo: 考虑使用StringView来提升效率
    
    std::string name_;  ///< 名称
    std::string desc_;  ///< 描述
};


AST_NAMESPACE_END
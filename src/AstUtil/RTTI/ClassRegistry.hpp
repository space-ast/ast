///
/// @file      ClassRegistry.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-02
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
#include <unordered_map>
#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class Class;

/// @brief 类注册器
/// @details 用于注册和查找类的元数据
class ClassRegistry
{
public:
    using ClassMap = std::unordered_map<std::string, Class*>;

    ClassRegistry() = default;
    ~ClassRegistry() = default;
    static ClassRegistry* Instance();

    Class* getClass(StringView name) const;
    void getAllClassNames(std::vector<std::string>& names) const;
    const ClassMap& getAllClasses() const { return classMap_; }
    void registerClass(Class* cls);
    void registerClass(Class* cls, StringView name);
protected:
    ClassMap classMap_;
};

/*! @} */

AST_NAMESPACE_END

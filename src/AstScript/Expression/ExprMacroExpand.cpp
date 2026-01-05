///
/// @file      ExprMacroExpand.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-24
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

#include "ExprMacroExpand.hpp"
#include "AstScript/ExprVisitor.hpp"
#include "AstScript/Macro.hpp"

AST_NAMESPACE_BEGIN


Value* ExprMacroExpand::eval() const
{
    // Macro类目前只是一个空基类，没有实际的展开功能
    // 这里可以根据实际需求添加展开逻辑
    // 暂时返回nullptr
    return nullptr;
}

std::string ExprMacroExpand::getExpression(Object* context) const
{
    std::string result;
    
    // Macro类目前没有getName方法
    if (macro_) {
        result += "macro";
    } else {
        result += "unknown_macro";
    }
    
    // 添加参数列表
    result += "(";
    for (size_t i = 0; i < args_.size(); ++i) {
        if (i > 0) {
            result += ", ";
        }
        result += args_[i]->getExpression(context);
    }
    result += ")";
    
    return result;
}

AST_NAMESPACE_END
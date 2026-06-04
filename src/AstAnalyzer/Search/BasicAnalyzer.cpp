///
/// @file      BasicAnalyzer.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-03
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

#include "BasicAnalyzer.hpp"
#include "AstScript/SymbolScope.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN


namespace
{
    class BasicAnalyzerSymbolScope : public ISymbolScope
    {
    public:
        BasicAnalyzerSymbolScope(BasicAnalyzer* analyzer)
            : analyzer_(analyzer)
        {}
        errc_t addSymbol(StringView name, Expr* expr) override
        {
            aError("addSymbol not implemented");
            return eErrorNotImplemented;
        }
        Expr* findSymbol(StringView name, bool searchParent = true) const override
        {
            auto analyzer = analyzer_.get();
            if(!analyzer)
            {
                aError("analyzer is null");
                return nullptr;
            }
            // 1. 从inputs_查找符号
            for(const auto& input : analyzer->inputs())
            {
                auto var = input.get();
                if(var && name == var->name())
                {
                    return var;
                }
            }
            // 2. 从outputs_查找符号
            for(const auto& output : analyzer->outputs())
            {
                auto var = output.get();
                if(var && name == var->name())
                {
                    return var;
                }
            }
            return nullptr;
        }
        Expr* resolveSymbol(StringView name) override
        {
            return findSymbol(name);
        }
    private:
        WeakPtr<BasicAnalyzer> analyzer_;
    };
}

BasicAnalyzer::BasicAnalyzer()
    : interpreter_(aNewInterpreter(new BasicAnalyzerSymbolScope(this)))
{
    
}

errc_t BasicAnalyzer::execute()
{
    return eNoError;
}

AST_NAMESPACE_END


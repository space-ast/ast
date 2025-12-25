///
/// @file      main.cpp
/// @brief     AST Script REPL 主程序
/// @details   交互式脚本解释器的入口程序
/// @author    jinke18
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

#include "repl.hpp"
#include "AstScript/ScriptAPI.hpp"
#include "AstScript/Value.hpp"
#include "AstUtil/SharedPtr.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/Quantity.hpp"
#include <cstdio>
#include <string>
#include <fstream>
#include <iostream>

AST_USING_NAMESPACE

int main(int argc, char* argv[])
{
    // 如果有命令行参数，执行脚本文件
    if (argc > 1) {
        const char* filename = argv[1];
        
        // 打开文件
        FILE* file = ast_fopen(filename, "rb");
        if (file == nullptr) {
            ast_printf(aText("无法打开文件: %s\n"), filename);
            return 1;
        }
        
        // 读取文件内容
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);
        
        std::string script(fileSize, '\0');
        size_t bytesRead = fread(&script[0], 1, fileSize, file);
        fclose(file);
        
        if (bytesRead != fileSize) {
            ast_printf(aText("读取文件失败: %s\n"), filename);
            return 1;
        }
        
        // 执行脚本
        try {
            SharedPtr<Expr> expr = aParseExpr(script);
            if (expr == nullptr) {
                ast_printf(aText("解析脚本失败: %s\n"), filename);
                return 1;
            }
            #ifndef NDEBUG
            std::string exprStr = aFormatExpr(expr);
            ast_printf(aText("parse expr: \n%s\n"), exprStr.c_str());
            
            #endif
            SharedPtr<Value> result = aEvalExpr(expr);
            
            if (result != nullptr) {
                // 输出结果
                if (aValueIsBool(result)) {
                    ast_printf("=> %s\n", (aValueUnboxBool(result) ? "true" : "false"));
                } else if (aValueIsInt(result)) {
                    ast_printf("=> %d\n", aValueUnboxInt(result));
                } else if (aValueIsDouble(result)) {
                    ast_printf("=> %g\n", aValueUnboxDouble(result));
                } else if (aValueIsQuantity(result)) {
                    std::string quantityStr = aFormatQuantity(aValueUnboxQuantity(result));
                    ast_printf(aText("=> quantity %s\n"), quantityStr.c_str());
                } else {
                    ast_printf(aText("=> value\n"));
                }
                return 0;
            } else {
                ast_printf(aText("执行结果为空\n"));
                return 1;
            }
        } catch (const std::exception& e) {
            ast_printf(aText("错误: %s\n"), e.what());
            return 1;
        } catch (...) {
            ast_printf(aText("发生未知错误\n"));
            return 1;
        }
        
        return -1;
    }
    
    // 没有命令行参数时，启动REPL
    Repl repl;
    return repl.run();
}
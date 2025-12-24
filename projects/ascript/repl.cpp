///
/// @file      repl.cpp
/// @brief     交互式脚本解释器实现
/// @details   使用replxx库提供命令行编辑功能的交互式脚本解释器实现
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
#include "AstUtil/IO.hpp"
#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
#include <fstream>

#ifdef AST_WITH_REPLXX
#include <replxx.hxx>
#endif

AST_NAMESPACE_BEGIN

Repl::Repl()
#ifdef AST_WITH_REPLXX
    : m_replxx(nullptr)
#endif
{}

Repl::~Repl()
{
    cleanup();
}

void Repl::init()
{
#ifdef AST_WITH_REPLXX
    // 初始化replxx
    m_replxx = new replxx::Replxx();
    m_replxx->install_window_change_handler();
    
    // 启用颜色支持
    m_replxx->set_no_color(false);
#endif
    
    ast_printf("=====================================\n");
    ast_printf("         AST Script REPL             \n");
    ast_printf("=====================================\n");
    ast_printf("输入表达式进行计算，输入 '.exit' 退出\n");
    ast_printf("支持的操作符: +, -, *, /, ^, %, &&, ||, ==, !=, <, <=, >, >= 等\n");
    ast_printf("输入 '.help' 查看帮助信息\n");
    ast_printf("=====================================\n");
}

int Repl::run()
{
    init();
    
    std::string line;
    
    while (true) {
        std::string prompt = "> ";
        
#ifdef AST_WITH_REPLXX
        // 使用replxx读取输入
        const char* input = m_replxx->input(prompt.c_str());
        if (input == nullptr) {
            // 用户按下了Ctrl+D
            ast_printf("\n退出AST Script REPL\n");
            break;
        }
        
        line = input;
        // 注意：replxx.input()返回的内存由库自己管理，不需要手动释放
        
        // 添加到历史记录
        if (!line.empty()) {
            m_replxx->history_add(line);
        }
#else
        // 不使用replxx时的简单输入
        ast_printf("%s", prompt.c_str());
        if (!std::getline(std::cin, line)) {
            // 用户按下了Ctrl+D
            ast_printf("\n退出AST Script REPL\n");
            break;
        }
#endif
        
        handleCommand(line);
    }
    
    cleanup();
    return 0;
}

void Repl::cleanup()
{
#ifdef AST_WITH_REPLXX
    if (m_replxx != nullptr) {
        delete m_replxx;
        m_replxx = nullptr;
    }
#endif
}

// 辅助函数：移除字符串前后空格
std::string trim(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

void Repl::handleCommand(const std::string& input)
{
    // 移除前后空格
    std::string trimmed = trim(input);
    
    if (trimmed.empty()) {
        return;
    }
    
    // 处理内置命令
    if (trimmed[0] == '.') {
        std::string command = trimmed.substr(1);
        
        if (command == "exit" || command == "quit") {
            ast_printf("退出AST Script REPL\n");
            exit(0);
        } else if (command == "help") {
            ast_printf("AST Script REPL 帮助:\n");
            ast_printf("  .exit   - 退出REPL\n");
            ast_printf("  .help   - 显示帮助信息\n");
            ast_printf("  .clear  - 清除屏幕\n");
            ast_printf("  表达式  - 直接输入表达式进行计算\n");
        } else if (command == "clear") {
#ifdef AST_WITH_REPLXX
            m_replxx->clear_screen();
#else
            system("cls");
#endif
        } else {
            ast_printf("未知命令: %s\n", trimmed.c_str());
            ast_printf("输入 '.help' 查看可用命令\n");
        }
        return;
    }
    
    // 执行脚本表达式
    executeScript(trimmed);
}

void Repl::executeScript(const std::string& script)
{
    try {
        // 解析并执行表达式
        Value* result = aEval(script);
        
        if (result != nullptr) {
            // 输出结果
            if (aValueIsBool(result)) {
                ast_printf("=> %s\n", (aValueUnboxBool(result) ? "true" : "false"));
            } else if (aValueIsInt(result)) {
                ast_printf("=> %d\n", aValueUnboxInt(result));
            } else if (aValueIsDouble(result)) {
                ast_printf("=> %g\n", aValueUnboxDouble(result));
            } else if (aValueIsQuantity(result)) {
                ast_printf("=> quantity\n");
            } else {
                ast_printf("=> value\n");
            }
            
            // 注意：Value类是前向声明，不要直接delete，依赖API内部管理
        } else {
            ast_printf("执行结果为空\n");
        }
    } catch (const std::exception& e) {
        ast_printf("错误: %s\n", e.what());
    } catch (...) {
        ast_printf("发生未知错误\n");
    }
}

AST_NAMESPACE_END
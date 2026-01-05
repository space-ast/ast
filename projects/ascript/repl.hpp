///
/// @file      repl.hpp
/// @brief     交互式脚本解释器
/// @details   使用replxx库提供命令行编辑功能的交互式脚本解释器
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

#pragma once

#include "AstGlobal.h"
#include <string>

// 前置声明replxx::Replxx类，避免在头文件中包含依赖
#ifdef AST_WITH_REPLXX
namespace replxx {
    class Replxx;
}
#endif

AST_NAMESPACE_BEGIN

class Repl
{
public:
    Repl();
    ~Repl();
    
    /// @brief 初始化REPL
    void init();
    
    /// @brief 运行REPL主循环
    int run();
    
    /// @brief 清理REPL资源
    void cleanup();
    
private:
    /// @brief 处理用户输入的命令
    void handleCommand(const std::string& input);
    
    /// @brief 执行脚本表达式
    void executeScript(const std::string& script);
    
#ifdef AST_WITH_REPLXX
    replxx::Replxx* m_replxx;
#endif
};

AST_NAMESPACE_END
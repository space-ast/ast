/// @file      Logger.cpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      17.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

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
 
#include "Logger.hpp"
#include "AstUtil/IO.hpp"
#include <stdarg.h> 

 
AST_NAMESPACE_BEGIN
 
 


void aLogMessageV(ELogLevel level, const MessageLogContext& context, const char* format, va_list ap)
{
	ast_printf("\n%s(%d): %s\n", context.file, context.line, context.function);
	ast_vprintf(format, ap);
	ast_printf("\n");
}

void aLogMessage(ELogLevel level, const MessageLogContext& context, const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	aLogMessageV(level, context, format, ap);
	va_end(ap);
}


AST_NAMESPACE_END
 


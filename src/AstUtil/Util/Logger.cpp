/// @file      Logger.cpp
/// @brief     
/// @details   ~
/// @author    axel
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
#include "AstUtil/Posix.hpp"
#include "AstUtil/ColoredPrint.hpp"   // for cprintf / EColor
#include <stdarg.h>

 
AST_NAMESPACE_BEGIN
 
 


namespace {

const char* aLevelTag(ELogLevel level) noexcept
{
	switch (level)
	{
	case ELogLevel::eDebug:    return aText("详情");
	case ELogLevel::eInfo:     return aText("提示");
	case ELogLevel::eWarning:  return aText("警告");
	case ELogLevel::eError:    return aText("错误");
	case ELogLevel::eCritical: return aText("严重");
	case ELogLevel::eFatal:    return aText("崩溃");
	default:                   return aText("日志");
	}
}

EColor aLevelColor(ELogLevel level) noexcept
{
	switch (level)
	{
	case ELogLevel::eDebug:    return eGray;      ///< 低调的调试
	case ELogLevel::eInfo:     return eCyan;      ///< 常规信息
	case ELogLevel::eWarning:  return eYellow;
	case ELogLevel::eError:    return eRed;
	case ELogLevel::eCritical: return eRed;
	case ELogLevel::eFatal:    return eRed;
	default:                   return eGray;
	}
}

} // namespace


void aLogMessageV(ELogLevel level, const MessageLogContext& context, const char* format, va_list ap)
{
	const char* file = context.file_     ? context.file_     : aText("<未知源文件>");
	const char* func = context.function_ ? context.function_ : aText("<未知函数>");

	// 仅着色等级标签,来源与消息用默认色
	cfprintf(aLevelColor(level), stderr, "\n[%s] ", aLevelTag(level));
	if (format)
	{
		int count = posix::vfprintf(stderr, format, ap);
		// 消息内容超过70个字符时换行
		if(count >= 70)
			posix::fprintf(stderr, "\n");
		else
			posix::fprintf(stderr, " ");
	}
	posix::fprintf(stderr, "(%s:%d)[%s]\n", file, context.line_, func);
}

void aLogMessage(ELogLevel level, const MessageLogContext& context, const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	aLogMessageV(level, context, format, ap);
	va_end(ap);
}


AST_NAMESPACE_END
 


///
/// @file      Logger.hpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      17.11.2025
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
 
#include "AstGlobal.h"
#include <iostream>     // for std::cout
#include <sstream>      // for std::ostringstream
#include <stdarg.h>     // for va_list

 
AST_NAMESPACE_BEGIN

/*!
    @addtogroup Util
	@{
*/



/// @brief 日志等级
enum class ELogLevel
{
    eDebug,     ///< 调试信息
    eInfo,      ///< 一般信息  
    eWarning,   ///< 警告信息
    eError,     ///< 错误信息
    eCritical,  ///< 严重错误
    eFatal      ///< 致命错误
};




/// @brief 日志消息的上下文信息
class MessageLogContext
{
public:
    MessageLogContext()
        : line_{0}
        , file_{nullptr}
        , function_{nullptr}
        , category_{nullptr}
    {}
    MessageLogContext(const char* fileName, int lineNumber, const char* functionName, const char* categoryName = nullptr)
        : line_{lineNumber}
        , file_{fileName}
        , function_{functionName}
        , category_{categoryName}
    {}

    int   line_{};             ///< 行数
    const char* file_{};       ///< 文件
    const char* function_{};   ///< 函数
    const char* category_{};   ///< 分类
};


AST_UTIL_CAPI void aLogMessageV(ELogLevel level, const MessageLogContext& context, const char* format, va_list ap);
AST_UTIL_CAPI void aLogMessage(ELogLevel level, const MessageLogContext& context, const char* format, ...);


class LoggerStream
{
public:
    LoggerStream(ELogLevel level, const MessageLogContext& context)
        : context_{ context }
        , level_{ level }
        , stream_{}
    {}
    LoggerStream(ELogLevel level, const char* fileName, int lineNumber, const char* functionName, const char* categoryName = nullptr)
        : context_{ fileName, lineNumber, functionName, categoryName }
        , level_{ level }
        , stream_{}
    {}
    LoggerStream(const LoggerStream& other)
        : context_{ other.context_ }
        , level_{ other.level_ }
        , stream_{}
    {}
    ~LoggerStream()
    {
        aLogMessage(level_, context_, stream_.str().c_str());
    }
    inline LoggerStream& space()
    {
        stream() << ' '; return *this;
    }
    inline LoggerStream& operator<<(bool t)
    {
        stream() << (t ? "true" : "false"); return space();
    }
    inline LoggerStream& operator<<(char t)
    {
        stream() << t; return space();
    }
    inline LoggerStream& operator<<(signed short t)
    {
        stream() << t; return space();
    }
    inline LoggerStream& operator<<(unsigned short t)
    {
        stream() << t; return space();
    }
    inline LoggerStream& operator<<(signed int t)
    {
        stream() << t; return space();
    }
    inline LoggerStream& operator<<(unsigned int t)
    {
        stream() << t; return space();
    }
    inline LoggerStream& operator<<(signed long t)
    {
        stream() << t; return space();
    }
    inline LoggerStream& operator<<(unsigned long t)
    {
        stream() << t; return space();
    }
    inline LoggerStream& operator<<(float t)
    {
        stream() << t; return space();
    }
    inline LoggerStream& operator<<(double t)
    {
        stream() << t; return space();
    }
    inline LoggerStream& operator<<(const char* t)
    {
        stream() << (t); return space();
    }
    inline LoggerStream& operator<<(const void* t)
    {
        stream() << t; return space();
    }
    inline std::ostringstream& stream()
    {
        return stream_;
    }
protected:
    MessageLogContext   context_;    ///< 上下文信息
    ELogLevel           level_;      ///< 等级
    std::ostringstream  stream_;     ///< 缓冲区
};

class NoopStream
{
public:
    inline NoopStream& space()
    {
        return *this;
    }
    template<typename T>
    inline NoopStream& operator<<(const T&)
    {
        return *this;
    }
};


class MessageLogger
{
public:
    MessageLogger()
        :context_{}
    {}
    MessageLogger(const char* file, int line, const char* function)
        : context_{file, line, function}
    {}
    template<typename ...Args>
    inline void debug(const char* msg, Args&& ...args) const
    {
        aLogMessage(ELogLevel::eDebug, context_, msg, std::forward<Args>(args)...);
    };
    template<typename ...Args>
    inline void noDebug(const char*, Args&& ...args) const{}
    template<typename ...Args>
    void info(const char* msg, Args&& ...args) const
    {
        aLogMessage(ELogLevel::eInfo, context_, msg, std::forward<Args>(args)...);
    }
    template<typename ...Args>
    inline void warning(const char* msg, Args&& ...args) const
    {
        aLogMessage(ELogLevel::eWarning, context_, msg, std::forward<Args>(args)...);
    }
    template<typename ...Args>
    inline void error(const char* msg, Args&& ...args) const
    {
        aLogMessage(ELogLevel::eError, context_, msg, std::forward<Args>(args)...);
    }
    template<typename ...Args>
    inline void critical(const char* msg, Args&& ...args) const
    {
        aLogMessage(ELogLevel::eCritical, context_, msg, std::forward<Args>(args)...);
    }
    template<typename ...Args>
    inline void fatal(const char* msg, Args&& ...args) const
    {
        aLogMessage(ELogLevel::eFatal, context_, msg, std::forward<Args>(args)...);
    }
    inline LoggerStream debug() const{return LoggerStream(ELogLevel::eDebug, context_); }
    inline NoopStream   noDebug() const{ return NoopStream(); }
    inline LoggerStream info() const{return LoggerStream(ELogLevel::eInfo, context_); }
    inline LoggerStream warning() const{return LoggerStream(ELogLevel::eWarning, context_);}
    inline LoggerStream error() const{return LoggerStream(ELogLevel::eError, context_);}
    inline LoggerStream critical() const{return LoggerStream(ELogLevel::eCritical, context_); };
    inline LoggerStream fatal() const{return LoggerStream(ELogLevel::eFatal, context_); };
private:
    MessageLogContext context_;
};

/*! @} */


AST_NAMESPACE_END


// 日志宏定义

#ifdef NDEBUG
#define aDebug(...)    while(false) AST_PREPEND_NAMESPACE(MessageLogger)().noDebug(__VA_ARGS__)
#else
#define aDebug(...)    AST_PREPEND_NAMESPACE(MessageLogger)(__FILE__, __LINE__, __FUNCTION__).debug(__VA_ARGS__)
#endif
#define aInfo(...)     AST_PREPEND_NAMESPACE(MessageLogger)(__FILE__, __LINE__, __FUNCTION__).info(__VA_ARGS__)
#define aWarning(...)  AST_PREPEND_NAMESPACE(MessageLogger)(__FILE__, __LINE__, __FUNCTION__).warning(__VA_ARGS__)
#define aError(...)    AST_PREPEND_NAMESPACE(MessageLogger)(__FILE__, __LINE__, __FUNCTION__).error(__VA_ARGS__)
#define aCritical(...) AST_PREPEND_NAMESPACE(MessageLogger)(__FILE__, __LINE__, __FUNCTION__).critical(__VA_ARGS__)
#define aFatal(...)    AST_PREPEND_NAMESPACE(MessageLogger)(__FILE__, __LINE__, __FUNCTION__).fatal(__VA_ARGS__)


#define AST_CHECK_NULLPTR(variable) if(variable == nullptr){aError(#variable " is notset(nullptr)"); return eErrorNullPtr;}
#define AST_CHECK_ERRCODE(rc, msg) if(rc){aError(msg); return rc;}
#define AST_CHECK_INVALID(condition) if(condition){aError("invalid parameter, with " #condition); return eErrorInvalidParam;}



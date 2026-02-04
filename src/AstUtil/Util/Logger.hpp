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
 


/// @brief 日志等级
enum ELogLevel
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
        : line(0)
        , file(nullptr)
        , function(nullptr)
        , category(nullptr)
    {}
    MessageLogContext(const char* fileName, int lineNumber, const char* functionName, const char* categoryName = nullptr)
        : line(lineNumber)
        , file(fileName)
        , function(functionName)
        , category(categoryName)
    {}

    int   line;             ///< 行数
    const char* file;       ///< 文件
    const char* function;   ///< 函数 
    const char* category;   ///< 分类
};


AST_UTIL_CAPI void aLogMessageV(ELogLevel level, const MessageLogContext& context, const char* format, va_list ap);
AST_UTIL_CAPI void aLogMessage(ELogLevel level, const MessageLogContext& context, const char* format, ...);


class LoggerStream
{
public:
    LoggerStream(ELogLevel level, const MessageLogContext& context)
        : m_context{ context }
        , m_level{ level }
    {}
    LoggerStream(ELogLevel level, const char* fileName, int lineNumber, const char* functionName, const char* categoryName = nullptr)
        :m_context{ fileName, lineNumber, functionName, categoryName }
        , m_level{ level }
    {}
    LoggerStream(const LoggerStream& other)
        :m_context{ other.m_context }
        , m_level{ other.m_level }
    {}
    ~LoggerStream()
    {
        aLogMessage(m_level, m_context, m_stream.str().c_str());
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
        return m_stream;
    }
protected:
    MessageLogContext   m_context;    ///< 上下文信息
    ELogLevel		    m_level;      ///< 等级
    std::ostringstream  m_stream;     ///< 缓冲区
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
        :m_context{}
    {}
    MessageLogger(const char* file, int line, const char* function)
        : m_context{file, line, function}
    {}
    template<typename ...Args>
    inline void debug(const char* msg, Args&& ...args) const
    {
        aLogMessage(eDebug, m_context, msg, std::forward<Args>(args)...);
    };
    template<typename ...Args>
    inline void noDebug(const char*, Args&& ...args) const{}
    template<typename ...Args>
    void info(const char* msg, Args&& ...args) const
    {
        aLogMessage(eInfo, m_context, msg, std::forward<Args>(args)...);
    }
    template<typename ...Args>
    inline void warning(const char* msg, Args&& ...args) const
    {
        aLogMessage(eWarning, m_context, msg, std::forward<Args>(args)...);
    }
    template<typename ...Args>
    inline void error(const char* msg, Args&& ...args) const
    {
        aLogMessage(eError, m_context, msg, std::forward<Args>(args)...);
    }
    template<typename ...Args>
    inline void critical(const char* msg, Args&& ...args) const
    {
        aLogMessage(eCritical, m_context, msg, std::forward<Args>(args)...);
    }
    template<typename ...Args>
    inline void fatal(const char* msg, Args&& ...args) const
    {
        aLogMessage(eFatal, m_context, msg, std::forward<Args>(args)...);
    }
    inline LoggerStream debug() const{return LoggerStream(eDebug, m_context); }
    inline NoopStream   noDebug() const{ return NoopStream(); }
    inline LoggerStream info() const{return LoggerStream(eInfo, m_context); }
    inline LoggerStream warning() const{return LoggerStream(eWarning, m_context);}
    inline LoggerStream error() const{return LoggerStream(eError, m_context);}
    inline LoggerStream critical() const{return LoggerStream(eCritical, m_context); };
    inline LoggerStream fatal() const{return LoggerStream(eFatal, m_context); };
private:
    MessageLogContext m_context;
};



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





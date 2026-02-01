///
/// @file      posix.hpp
/// @brief     posix 相关函数
/// @details   可移植操作系统接口(Portable Operating System Interface)
/// @author    axel
/// @date      2026-02-01
/// @copyright 版权所有 (C) 2026-present, ast项目.
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
#ifdef _WIN32
#include <io.h>  
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

AST_NAMESPACE_BEGIN

namespace posix
{
    A_ALWAYS_INLINE
    int isatty(int fd)
    {
        #ifdef _WIN32
        return ::_isatty(fd);
        #else
        return ::isatty(fd);
        #endif
    }

    A_ALWAYS_INLINE
    int fileno(FILE* file)
    {
        #ifdef _WIN32
        return ::_fileno(file);
        #else
        return ::fileno(file);
        #endif
    }

    A_ALWAYS_INLINE
    const char* getenv(const char* name){
        #if defined(__BORLANDC__) || defined(__SunOS_5_8) || defined(__SunOS_5_9)
        const char* const env = std::getenv(name);
        return (env != nullptr && env[0] != '\0') ? env : nullptr;
        #else
        return std::getenv(name);
        #endif
    }
}
AST_NAMESPACE_END

///
/// @file      Environment.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-20
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

#include "Environment.hpp"
#include "AstUtil/Posix.hpp"
#include "AstUtil/StringView.hpp"
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#endif



AST_NAMESPACE_BEGIN


static bool _aHasCIVar()
{
    // #pragma warning(suppress: 4996)
    const char* ci_env = posix::getenv("CI");
    if(ci_env && ci_env[0] != '\0')
        return true;
    return false;
}

bool aIsCI()
{
    return aIsGithubCI() || aIsGitlabCI() || _aHasCIVar();
}

bool aIsGithubCI()
{
    // #pragma warning(suppress: 4996)
    const char* ci_env = posix::getenv("GITHUB_ACTION");
    if(ci_env && ci_env[0] != '\0')
        return true;
    return false;
}

bool aIsGitlabCI()
{
    // #pragma warning(suppress: 4996)
    const char* ci_env = posix::getenv("GITLAB_CI");
    if(ci_env && ci_env[0] != '\0')
        return true;
    return false;
}

bool aTerminalSupportColor()
{
    // 首先检查标准输出是否是终端
    if (!aStdOutIsTerminal()) {
        return false;
    }
    
    // 检查是否在 CI 环境中
    // if (aIsCI()) {
    //     // 大多数 CI 环境支持颜色
    //     return true;
    // }
    
    #ifdef _WIN32
    // Windows 平台
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    // 检查控制台是否支持颜色
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        // 所有现代 Windows 控制台都支持颜色
        return true;
    }
    return false;

    #elif defined(A_WASM)
    // WebAssembly 平台不支持颜色
    return false;
    #else
    // Unix/Linux/macOS 平台
    
    // 检查 TERM 环境变量
    const char* term = posix::getenv("TERM");
    if (term) {
        // 常见的支持颜色的终端类型
        const char* color_terms[] = {
            "xterm", "xterm-color", "xterm-256color",
            "vt100", "vt102", "vt220", "vt320",
            "ansi", "screen", "screen-256color",
            "tmux", "tmux-256color", "rxvt-unicode", "rxvt-unicode-256color",
            "linux", "cygwin", "msys", "git-bash"
        };
        
        for (size_t i = 0; i < sizeof(color_terms) / sizeof(color_terms[0]); i++) {
            if (std::strcmp(term, color_terms[i]) == 0) {
                return true;
            }
        }
    }
    
    // 检查 COLORTERM 环境变量
    const char* colorterm = posix::getenv("COLORTERM");
    if (colorterm && colorterm[0] != '\0') {
        return true;
    }
    
    return false;
    #endif
}

StringView aProjectName()
{
    return AST_PROJECT_NAME;
}

bool is_term(int fd)
{
    if (fd < 0) {
        return false;
    }
#ifdef _WIN32    
    HANDLE handle = (HANDLE)_get_osfhandle(fd);
    if (handle == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    DWORD mode;
    // 检查是否为控制台（终端）
    return GetConsoleMode(handle, &mode) != 0;
#else
    // Unix/Linux/macOS平台
    return isatty(fd) != 0;
#endif
}

bool is_file(int fd)
{
    if (fd < 0) {
        return false;
    }
#ifdef _WIN32
    // Windows平台
    
    HANDLE handle = (HANDLE)_get_osfhandle(fd);
    if (handle == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    // 获取文件类型
    DWORD fileType = GetFileType(handle);
    
    // FILE_TYPE_DISK 表示磁盘文件（普通文件）
    if (fileType == FILE_TYPE_DISK) {
        return true;
    }
    
    return false;
#else
    // Unix/Linux/macOS平台
    
    struct stat statbuf;
    // 获取文件描述符的状态信息
    if (fstat(fd, &statbuf) == 0) {
        // 使用宏判断是否为普通文件
        return S_ISREG(statbuf.st_mode) != 0;
    }
    
    return false;
#endif
}


bool aIsTerminal(FILE* file) {
    if (file == NULL) {
        return false;
    }
    int fd = posix::fileno(file);
    return is_term(fd);
}

bool aIsFile(FILE* file) {
    if (file == NULL) {
        return false;
    }
    
    int fd = posix::fileno(file);
    // 首先检查是否为终端
    if (is_term(fd)) {
        return false;
    }
    return is_file(fd);
}

bool aStdInIsTerminal()
{
    return aIsTerminal(stdin);
}

bool aStdInIsFile()
{
    return aIsFile(stdin);
}

bool aStdOutIsTerminal() {
    return aIsTerminal(stdout);
}

bool aStdOutIsFile() {
    return aIsFile(stdout);
}

bool aStdErrIsTerminal()
{
    return aIsTerminal(stderr);
}

bool aStdErrIsFile()
{
    return aIsFile(stderr);
}

AST_NAMESPACE_END
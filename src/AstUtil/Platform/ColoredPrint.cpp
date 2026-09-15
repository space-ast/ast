///
/// @file      ColoredPrint.cpp
/// @brief     彩色打印功能实现
/// @author    axel
/// @date      2026-04-10
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

#include "ColoredPrint.hpp"
#include "AstUtil/Color.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/Posix.hpp"
#include "Environment.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

AST_NAMESPACE_BEGIN

#ifdef _WIN32
/// @brief 将 RGB 颜色映射为最接近的控制台颜色属性
/// @note  Windows 控制台仅支持 16 色，这里使用一个简单的映射
static WORD _consoleAttrFromRGB(uint8_t r, uint8_t g, uint8_t b)
{
    WORD attr = 0;
    if (r > 127) attr |= FOREGROUND_RED;
    if (g > 127) attr |= FOREGROUND_GREEN;
    if (b > 127) attr |= FOREGROUND_BLUE;
    // 三个分量均较亮，使用高亮前景色表示浅色
    if (attr == (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE))
        attr |= FOREGROUND_INTENSITY;
    return attr;
}
#endif

int cvfprintf(Color color, FILE * stream, const char * format, va_list args)
{
    if (stream == nullptr)
        return 0;

    // 检查流是否为终端，非终端（文件、管道等）不输出颜色控制序列
    bool supportColor = aIsTerminal(stream);

    if(!supportColor)
    {
        // 流不支持颜色，使用普通打印，避免颜色控制字符污染输出
        return posix::vfprintf(stream, format, args);
    }

    // 获取 RGB 分量
    uint8_t r, g, b, a;
    aColorToRGBA(color, r, g, b, a);

    // 在 Windows 平台上使用 SetConsoleTextAttribute
    #ifdef _WIN32
    HANDLE hConsole = (HANDLE)_get_osfhandle(posix::fileno(stream));
    // 保存原有颜色，打印完成后恢复
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    bool hasOldAttr = GetConsoleScreenBufferInfo(hConsole, &csbi) != 0;
    SetConsoleTextAttribute(hConsole, _consoleAttrFromRGB(r, g, b));
    #else
    // 在 Unix/Linux 平台上使用 ANSI 转义序列
    posix::fprintf(stream, "\033[38;2;%d;%d;%dm", r, g, b);
    #endif

    // 打印内容，返回值为格式化内容的字符数（不含颜色控制序列）
    int result = posix::vfprintf(stream, format, args);

    // 恢复原有颜色
    #ifdef _WIN32
    if (hasOldAttr)
        SetConsoleTextAttribute(hConsole, csbi.wAttributes);
    else
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    #else
    posix::fprintf(stream, "\033[0m");
    #endif

    return result;
}

int cvprintf(Color color, const char* format, va_list args)
{
    return cvfprintf(color, stdout, format, args);
}

int cprintf(Color color, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int result = cvprintf(color, format, args);
    va_end(args);
    return result;
}

int cfprintf(Color color, FILE * stream, const char * format, ...)
{
    va_list args;
    va_start(args, format);
    int result = cvfprintf(color, stream, format, args);
    va_end(args);
    return result;
}

AST_NAMESPACE_END
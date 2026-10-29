///
/// @file      Escape.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-16
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "Escape.hpp"
#include "AstUtil/StringView.hpp"
#include <cctype>
#include <cstdio>

AST_NAMESPACE_BEGIN

namespace {

/// @brief 检查字符是否为可打印ASCII字符（非控制字符）
bool isPrintableAscii(char c) noexcept
{
    return static_cast<unsigned char>(c) >= 0x20 && static_cast<unsigned char>(c) <= 0x7E;
}

/// @brief 检查字符是否为ASCII空白字符
bool isAsciiWhitespace(char c) noexcept
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

/// @brief 将十六进制字符转换为对应的数值
int hexCharToValue(char c) noexcept
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return -1; // 无效的十六进制字符
}

/// @brief 将八进制字符转换为对应的数值
int octCharToValue(char c) noexcept
{
    if (c >= '0' && c <= '7') {
        return c - '0';
    }
    return -1; // 无效的八进制字符
}

} // namespace


std::string aCUnescape(StringView source)
{
    std::string dest;
    dest.reserve(source.size()); // 预分配空间，通常转义后的字符串不会更长
    
    const char* ptr = source.data();
    const char* end = ptr + source.size();
    
    while (ptr < end) {
        if (*ptr == '\\') {
            // 处理转义序列
            ++ptr; // 跳过反斜杠
            if (ptr >= end) {
                // 反斜杠在字符串末尾
                dest.push_back('\\');
                break;
            }
            
            switch (*ptr) {
                // 简单的转义序列
                case '\\': dest.push_back('\\'); break;
                case '\'': dest.push_back('\''); break;
                case '\"': dest.push_back('"'); break;
                case 'a': dest.push_back('\a'); break;
                case 'b': dest.push_back('\b'); break;
                case 'f': dest.push_back('\f'); break;
                case 'n': dest.push_back('\n'); break;
                case 'r': dest.push_back('\r'); break;
                case 't': dest.push_back('\t'); break;
                case 'v': dest.push_back('\v'); break;
                case '?': dest.push_back('?'); break;
                // 十六进制转义序列
                case 'x': {
                    ++ptr; // 跳过'x'
                    if (ptr >= end) {
                        // 无效的十六进制转义序列
                        dest.push_back('\\');
                        dest.push_back('x');
                        break;
                    }
                    
                    int value = 0;
                    int digits = 0;
                    
                    // 读取十六进制数字（至少1位，位数不限）
                    // 按 C/C++ 规则，值超出 unsigned char 范围时取低 8 位（与 GCC/Clang 一致）
                    while (ptr < end) {
                        int digit = hexCharToValue(*ptr);
                        if (digit >= 0) {
                            value = (value * 16 + digit) & 0xFF;
                            ++ptr;
                            ++digits;
                        } else {
                            // 遇到非十六进制字符
                            break;
                        }
                    }
                    
                    if (digits == 0) {
                        // 无效的十六进制转义序列
                        dest.push_back('\\');
                        dest.push_back('x');
                        --ptr; // 因为循环会再次++ptr
                    } else {
                        dest.push_back(static_cast<char>(value & 0xFF));
                        --ptr; // 因为循环会再次++ptr
                    }
                    break;
                }
                default:
                {
                    // 八进制转义序列
                    if(*ptr >= '0' && *ptr <= '7')
                    {
                        int value = octCharToValue(*ptr);
                        ++ptr;
                        
                        // 最多读取3位八进制数字
                        for (int i = 0; i < 2 && ptr < end; ++i) {
                            int digit = octCharToValue(*ptr);
                            if (digit >= 0) {
                                value = value * 8 + digit;
                                ++ptr;
                            } else {
                                break;
                            }
                        }
                        
                        dest.push_back(static_cast<char>(value & 0xFF));
                        --ptr; // 因为循环会再次++ptr
                        break;
                    }else{
                        // 未知的转义序列，保持原样
                        dest.push_back('\\');
                        dest.push_back(*ptr);
                        break;
                    }
                }
            }
        } else {
            // 普通字符
            dest.push_back(*ptr);
        }
        
        ++ptr;
    }
    
    return dest;
}


std::string aCEscape(StringView src)
{
    std::string result;
    result.reserve(src.size() * 2); // 预分配空间，转义后可能更长
    
    const char* ptr = src.data();
    const char* end = ptr + src.size();
    
    while (ptr < end) {
        unsigned char c = static_cast<unsigned char>(*ptr);
        
        switch (c) {
            // 简单的转义序列
            case '\\': result.append("\\\\"); break;
            case '\'': result.append("\\\'"); break;
            case '\"': result.append("\\\""); break;
            case '\a': result.append("\\a"); break;
            case '\b': result.append("\\b"); break;
            case '\f': result.append("\\f"); break;
            case '\n': result.append("\\n"); break;
            case '\r': result.append("\\r"); break;
            case '\t': result.append("\\t"); break;
            case '\v': result.append("\\v"); break;
            
            default:
                if (isPrintableAscii(c) && !isAsciiWhitespace(c)) {
                    // 可打印的非空白字符，直接输出
                    result.push_back(c);
                } else {
                    // 不可打印字符或空白字符，使用八进制转义
                    char buf[5];
                    std::snprintf(buf, sizeof(buf), "\\%03o", c);
                    result.append(buf);
                }
                break;
        }
        
        ++ptr;
    }
    
    return result;
}


std::string aQuote(StringView src)
{
    std::string result;
    result.reserve(src.size() * 2);
    result.push_back('"');
    for (char ch : src) {
        switch (ch) {
            case '"':  result.append("\\\""); break;
            case '\\': result.append("\\\\"); break;
            case '\n': result.append("\\n");  break;
            case '\r': result.append("\\r");  break;
            case '\t': result.append("\\t");  break;
            case '\b': result.append("\\b");  break;
            case '\f': result.append("\\f");  break;
            case '\v': result.append("\\v");  break;
            default:   result.push_back(ch); break;
        }
    }
    result.push_back('"');
    return result;
}


std::string aUnquote(StringView src)
{
    // 检查引号格式
    if (src.size() < 2 || src.front() != '"' || src.back() != '"') {
        // 不符合预期格式时，这里返回原字符串
        return std::string(src);
    }
    StringView inner = src.substr(1, src.size() - 2);
    return aCUnescape(inner);
}

void aUnescapeXML(StringView &str, std::string &result)
{
    size_t i = 0;
    const size_t n = str.size();
    result.reserve(result.size() + n);  // 预分配空间，提升性能

    while (i < n) {
        if (str[i] == '&') {
            // 寻找匹配的 ';'
            size_t end = i + 1;
            while (end < n && str[end] != ';') {
                ++end;
            }

            if (end < n) {  // 找到完整的实体
                StringView entity = str.substr(i + 1, end - i - 1);
                char c = 0;

                // 1. 处理命名实体
                if (entity == "lt") c = '<';
                else if (entity == "gt") c = '>';
                else if (entity == "amp") c = '&';
                else if (entity == "apos") c = '\'';
                else if (entity == "quot") c = '"';

                // 2. 处理数字实体 (&#dd; 或 &#xhhhh;)
                else if (!entity.empty() && entity[0] == '#') {
                    uint32_t cp = 0;
                    bool valid = false;
                    size_t pos = 1;  // 跳过 '#'

                    // 十六进制格式：&#x...
                    if (pos < entity.size() && (entity[pos] == 'x' || entity[pos] == 'X')) {
                        ++pos;
                        for (; pos < entity.size(); ++pos) {
                            char ch = entity[pos];
                            if (ch >= '0' && ch <= '9') cp = cp * 16 + (ch - '0');
                            else if (ch >= 'a' && ch <= 'f') cp = cp * 16 + (ch - 'a' + 10);
                            else if (ch >= 'A' && ch <= 'F') cp = cp * 16 + (ch - 'A' + 10);
                            else break;
                        }
                        if (pos == entity.size()) valid = true;
                    }
                    // 十进制格式：&#...
                    else {
                        for (; pos < entity.size(); ++pos) {
                            char ch = entity[pos];
                            if (ch >= '0' && ch <= '9') cp = cp * 10 + (ch - '0');
                            else break;
                        }
                        if (pos == entity.size()) valid = true;
                    }

                    // 有效的 Unicode 码点（排除非字符）
                    if (valid && cp <= 0x10FFFF && cp != 0xFFFE && cp != 0xFFFF) {
                        // 将码点编码为 UTF-8 并追加
                        if (cp < 0x80) {
                            result.push_back(static_cast<char>(cp));
                        } else if (cp < 0x800) {
                            result.push_back(static_cast<char>(0xC0 | (cp >> 6)));
                            result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
                        } else if (cp < 0x10000) {
                            result.push_back(static_cast<char>(0xE0 | (cp >> 12)));
                            result.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
                            result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
                        } else {
                            result.push_back(static_cast<char>(0xF0 | (cp >> 18)));
                            result.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
                            result.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
                            result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
                        }
                        i = end + 1;
                        continue;  // 跳过整个实体
                    }
                }

                // 如果识别到有效命名实体，追加对应字符并跳过实体
                if (c != 0) {
                    result.push_back(c);
                    i = end + 1;
                    continue;
                }
            }
        }

        // 未识别为实体或无效实体：原样拷贝当前字符
        result.push_back(str[i]);
        ++i;
    }
}

std::string aUnescapeXML(StringView src)
{
    std::string result;
    aUnescapeXML(src, result);
    return result;
}

AST_NAMESPACE_END
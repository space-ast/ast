/// @file      JsonParser.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-16
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
/// ……

#pragma once

#include "AstGlobal.h"
#include "AstUtil/BaseParser.hpp"
#include "AstUtil/JsonSax.hpp"    // 为解析函数引入 JsonSax 完整定义
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>   // strtod, strtol
#include <cerrno>
#include <cstring>
#include <cctype>
#include <algorithm>

AST_NAMESPACE_BEGIN

class JsonValue;
class JsonSax;

/// @brief JSON 解析器
class AST_UTIL_API JsonParser
{
public:
    errc_t parseFromString(StringView json, JsonValue& value);
    errc_t parseFromFile(StringView file, JsonValue& value);
    errc_t parseFromString(StringView json, JsonSax& sax);
    errc_t parseFromFile(StringView file, JsonSax& sax);

private:
    // ---- 缓冲区与游标 ----
    std::vector<char> buffer_;      ///< 字符缓冲区（包含待解析内容）
    size_t           currentPos_{0};///< 当前解析位置

    // ---- 文件读取 ----
    /// @brief 将文件内容读入缓冲区
    errc_t readFile(StringView filepath);

    // ---- 低层字符操作 ----
    /// @brief 跳过空白字符（空格、制表、换行、回车）
    void skipWhitespace();

    /// @brief 查看当前字符但不移动游标，缓冲区末尾返回 '\0'
    char peek() const;

    /// @brief 取当前字符并前移游标，缓冲区末尾返回 '\0'
    char next();

    /// @brief 期望下一个字符为 c，否则触发错误并返回 false
    bool expect(char c, JsonSax& sax);

    // ---- 递归下降解析 ----
    errc_t parseValue(JsonSax& sax);
    errc_t parseObject(JsonSax& sax);
    errc_t parseArray(JsonSax& sax);
    errc_t parseString(JsonSax& sax, bool isKey);
    errc_t parseNumber(JsonSax& sax);

    /// @brief 解析 true / false / null
    errc_t parseLiteral(const char* literal, size_t len, JsonSax& sax);
    // 由于 bool 回调需要参数，这里重载
    errc_t parseLiteralBool(bool value, JsonSax& sax);

    // ---- Unicode 辅助 ----
    /// @brief 将码点编码为 UTF-8 追加到字符串
    static void appendUtf8(std::string& out, unsigned int codepoint);

    /// @brief 从当前位置解析 4 位十六进制数
    unsigned int parseHex4();
};

/*! @} */

AST_NAMESPACE_END
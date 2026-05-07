///
/// @file      JsonParser.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-24
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


#include "JsonParser.hpp"
#include "JsonDomSax.hpp"   // 用于 parseFromString(…, JsonValue&) 的实现
#include "JsonValue.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/IO.hpp"
#include <limits>
#include <utility>

AST_NAMESPACE_BEGIN

// ======================= 公有接口 =======================

errc_t JsonParser::parseFromString(StringView json, JsonValue& value)
{
    JsonDomSax dom(value);
    return parseFromString(json, dom);
}

errc_t JsonParser::parseFromString(StringView json, JsonSax& sax)
{
    // 将 StringView 数据复制到内部缓冲区，确保解析期间数据有效
    buffer_.assign(json.begin(), json.end());
    currentPos_ = 0;

    // 跳过前导空白
    skipWhitespace();
    if (currentPos_ >= buffer_.size()) {
        // 空文档
        sax.parseError("Empty JSON document.");
        return -1;
    }

    // 一个 JSON 文档只能包含一个值
    errc_t ret = parseValue(sax);
    if (ret != 0) return ret;

    // 检查末尾是否还有多余内容
    skipWhitespace();
    if (currentPos_ < buffer_.size()) {
        sax.parseError("Extra data after root value.");
        return -1;
    }

    return 0;
}

errc_t JsonParser::parseFromFile(StringView file, JsonValue& value)
{
    JsonDomSax dom(value);
    return parseFromFile(file, dom);
}

errc_t JsonParser::parseFromFile(StringView file, JsonSax& sax)
{
    // 读取文件内容
    errc_t ret = readFile(file);
    if (ret != 0) {
        sax.parseError("Failed to read file or file is empty.");
        return -1;
    }

    // 复用字符串解析
    return parseFromString(StringView(buffer_.data(), buffer_.size()), sax);
}

// ======================= 文件读取 =======================

errc_t JsonParser::readFile(StringView filepath)
{
    std::string path(filepath);
    FILE* fp = ast_fopen(path.c_str(), "rb");
    if (!fp) return -1;

    // 获取文件大小
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    if (size < 0) { fclose(fp); return -1; }
    fseek(fp, 0, SEEK_SET);

    // 读入缓冲区
    buffer_.resize(size);
    if (size > 0) {
        size_t readSize = fread(buffer_.data(), 1, size, fp);
        if (readSize != static_cast<size_t>(size)) {
            fclose(fp);
            return -1;
        }
    }
    fclose(fp);
    currentPos_ = 0;
    return 0;
}

// ======================= 低层字符操作 =======================

void JsonParser::skipWhitespace()
{
    while (currentPos_ < buffer_.size()) {
        char c = buffer_[currentPos_];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
            ++currentPos_;
        else
            break;
    }
}

char JsonParser::peek() const
{
    return (currentPos_ < buffer_.size()) ? buffer_[currentPos_] : '\0';
}

char JsonParser::next()
{
    return (currentPos_ < buffer_.size()) ? buffer_[currentPos_++] : '\0';
}

bool JsonParser::expect(char c, JsonSax& sax)
{
    if (peek() == c) {
        next();
        return true;
    }
    // 构造错误信息
    char msg[64];
    snprintf(msg, sizeof(msg), "Expected '%c' but got '%c' (0x%02X).", c, peek(), (unsigned char)peek());
    sax.parseError(msg);
    return false;
}

// ======================= 递归下降解析 =======================

errc_t JsonParser::parseValue(JsonSax& sax)
{
    skipWhitespace();
    if (currentPos_ >= buffer_.size()) {
        sax.parseError("Unexpected end of input.");
        return -1;
    }

    char c = peek();
    switch (c) {
    case '{':  return parseObject(sax);
    case '[':  return parseArray(sax);
    case '"':  return parseString(sax, false);   // 普通字符串值
    case 't':
        if (!parseLiteral("true", 4, sax))
            return sax.boolValue(true);
        return -1;
    case 'f':
        if (!parseLiteral("false", 5, sax))
            return sax.boolValue(false);
        return -1;
    case 'n':
        if (!parseLiteral("null", 4, sax))
            return sax.nullValue();
        return -1;
    default:
        if (c == '-' || std::isdigit(static_cast<unsigned char>(c))) {
            return parseNumber(sax);
        }
        char msg[64];
        snprintf(msg, sizeof(msg), "Unexpected character: '%c' (0x%02X).", c, (unsigned char)c);
        sax.parseError(msg);
        return -1;
    }
}

errc_t JsonParser::parseObject(JsonSax& sax)
{
    if (!expect('{', sax)) return -1;

    errc_t ret = sax.startObject();
    if (ret != 0) return ret;

    skipWhitespace();
    if (peek() == '}') {
        next();                         // 空对象
        return sax.endObject();
    }

    while (true) {
        // 键
        skipWhitespace();
        ret = parseString(sax, true);   // true 表示这是 key
        if (ret != 0) return ret;

        // 冒号
        skipWhitespace();
        if (!expect(':', sax)) return -1;

        // 值
        ret = parseValue(sax);
        if (ret != 0) return ret;

        // 分隔符
        skipWhitespace();
        char c = peek();
        if (c == '}') {
            next();
            break;
        } else if (c == ',') {
            next();
        } else {
            sax.parseError("Expected ',' or '}' in object.");
            return -1;
        }
    }

    return sax.endObject();
}

errc_t JsonParser::parseArray(JsonSax& sax)
{
    if (!expect('[', sax)) return -1;

    errc_t ret = sax.startArray();
    if (ret != 0) return ret;

    skipWhitespace();
    if (peek() == ']') {
        next();                         // 空数组
        return sax.endArray();
    }
    size_t index = 0;
    while (true) {
        
        ret = sax.index(index);
        if (ret != 0) return ret;
        index++;

        ret = parseValue(sax);
        if (ret != 0) return ret;

        skipWhitespace();
        char c = peek();
        if (c == ']') {
            next();
            break;
        } else if (c == ',') {
            next();
        } else {
            sax.parseError("Expected ',' or ']' in array.");
            return -1;
        }
    }

    return sax.endArray();
}

errc_t JsonParser::parseString(JsonSax& sax, bool isKey)
{
    if (!expect('"', sax)) return -1;
    std::string result;
    while (currentPos_ < buffer_.size()) {
        char c = next();
        if (c == '"') {
            // 字符串结束
            break;
        } else if (c == '\\') {
            // 转义字符
            char escape = next();
            switch (escape) {
                case '"':  result += '"';  break;
                case '\\': result += '\\'; break;
                case '/':  result += '/';  break;
                case 'b':  result += '\b'; break;
                case 'f':  result += '\f'; break;
                case 'n':  result += '\n'; break;
                case 'r':  result += '\r'; break;
                case 't':  result += '\t'; break;
                case 'u': {
                    unsigned int code = parseHex4();
                    // 处理 UTF-16 代理对（简化：仅支持基本多文种平面）
                    if (code >= 0xD800 && code <= 0xDBFF) {
                        // 高代理，预期后面紧跟 \uXXXX 低代理
                        if (next() != '\\' || next() != 'u') {
                            sax.parseError("Invalid UTF-16 surrogate pair.");
                            return -1;
                        }
                        unsigned int low = parseHex4();
                        if (low < 0xDC00 || low > 0xDFFF) {
                            sax.parseError("Invalid low surrogate.");
                            return -1;
                        }
                        code = 0x10000 + ((code - 0xD800) << 10) + (low - 0xDC00);
                        appendUtf8(result, code);
                    } else {
                        appendUtf8(result, code);
                    }
                    break;
                }
                default:
                    // 非法转义，忽略错误，保留原字符
                    result += escape;
                    break;
            }
        } else if (static_cast<unsigned char>(c) < 0x20) {
            // 未转义的控制字符
            sax.parseError("Unescaped control character in string.");
            return -1;
        } else {
            result += c;
        }
    }

    // 触发回调
    if (isKey) {
        return sax.key(StringView(result.data(), result.size()));
    } else {
        return sax.stringValue(StringView(result.data(), result.size()));
    }
}

errc_t JsonParser::parseNumber(JsonSax& sax)
{
    // 记录数字起始位置
    size_t start = currentPos_;

    // 根据 JSON 规范读取完整数字
    if (peek() == '-') next();

    // 整数部分
    if (peek() == '0') {
        next();
    } else if (std::isdigit(static_cast<unsigned char>(peek()))) {
        while (std::isdigit(static_cast<unsigned char>(peek())))
            next();
    } else {
        sax.parseError("Invalid number.");
        return -1;
    }

    // 小数部分
    bool isFloat = false;
    if (peek() == '.') {
        isFloat = true;
        next();
        if (!std::isdigit(static_cast<unsigned char>(peek()))) {
            sax.parseError("Expected digit after decimal point.");
            return -1;
        }
        while (std::isdigit(static_cast<unsigned char>(peek())))
            next();
    }

    // 指数部分
    if (peek() == 'e' || peek() == 'E') {
        isFloat = true;
        next();
        if (peek() == '+' || peek() == '-') next();
        if (!std::isdigit(static_cast<unsigned char>(peek()))) {
            sax.parseError("Expected digit in exponent.");
            return -1;
        }
        while (std::isdigit(static_cast<unsigned char>(peek())))
            next();
    }

    // 提取数字子串
    std::string numberStr(buffer_.data() + start, currentPos_ - start);

    // 根据是否为浮点数选择回调
    if (isFloat) {
        char* end;
        errno = 0;
        double val = std::strtod(numberStr.c_str(), &end);
        if (errno == ERANGE || end != numberStr.c_str() + numberStr.size()) {
            sax.parseError("Invalid floating point number.");
            return -1;
        }
        return sax.doubleValue(val);
    } else {
        char* end;
        errno = 0;
        long long val = std::strtoll(numberStr.c_str(), &end, 10);
        if (errno == ERANGE || end != numberStr.c_str() + numberStr.size()) {
            // 尝试用 double 解析
            errno = 0;
            double val = std::strtod(numberStr.c_str(), &end);
            if (errno == ERANGE || end != numberStr.c_str() + numberStr.size()) {
                sax.parseError("Invalid integer number.");
                return -1;
            }
            return sax.doubleValue(val);
        }
        // 尝试放入 int 范围
        if (val >= (std::numeric_limits<int>::min)() && val <= (std::numeric_limits<int>::max)()) {
            return sax.intValue(static_cast<int>(val));
        } else {
            // 如果 int 装不下，降级为 double 回调
            return sax.doubleValue(static_cast<double>(val));
        }
    }
}

errc_t JsonParser::parseLiteral(const char* literal, size_t len, JsonSax& sax)
{
    for (size_t i = 0; i < len; ++i) {
        if (peek() != literal[i]) {
            char msg[64];
            snprintf(msg, sizeof(msg), "Expected literal '%s'.", literal);
            sax.parseError(msg);
            return -1;
        }
        next();
    }
    return 0;
}

// ======================= Unicode 辅助 =======================

unsigned int JsonParser::parseHex4()
{
    unsigned int code = 0;
    for (int i = 0; i < 4; ++i) {
        char digit = next();
        code <<= 4;
        if (digit >= '0' && digit <= '9')
            code |= (digit - '0');
        else if (digit >= 'a' && digit <= 'f')
            code |= (digit - 'a' + 10);
        else if (digit >= 'A' && digit <= 'F')
            code |= (digit - 'A' + 10);
        else {
            // 非法的十六进制字符，返回 0 并让上层报错
            return 0;
        }
    }
    return code;
}

void JsonParser::appendUtf8(std::string& out, unsigned int codepoint)
{
    if (codepoint <= 0x7F) {
        out += static_cast<char>(codepoint);
    } else if (codepoint <= 0x7FF) {
        out += static_cast<char>(0xC0 | (codepoint >> 6));
        out += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else if (codepoint <= 0xFFFF) {
        out += static_cast<char>(0xE0 | (codepoint >> 12));
        out += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
        out += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else if (codepoint <= 0x10FFFF) {
        out += static_cast<char>(0xF0 | (codepoint >> 18));
        out += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
        out += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
        out += static_cast<char>(0x80 | (codepoint & 0x3F));
    }
    // 超出 Unicode 范围的码点直接丢弃（不追加）
}

AST_NAMESPACE_END
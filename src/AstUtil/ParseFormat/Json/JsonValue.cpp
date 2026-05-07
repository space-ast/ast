///
/// @file      JsonValue.cpp
/// @brief     JSON 值类实现
/// @details   提供 JSON 值的表示和操作实现
/// @author    axel
/// @date      2026-04-16
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

#include "JsonValue.hpp"
#include "AstUtil/StringUtil.hpp"
#include "AstUtil/ParseFormat.hpp"
#include "AstUtil/JsonParser.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <limits>

AST_NAMESPACE_BEGIN

// 静态空值实例
JsonValue& JsonValue::NullValue()
{
    static JsonValue nullInstance;
    return nullInstance;
}


JsonValue JsonValue::FromString(StringView json)
{
    JsonValue value;
    errc_t rc = value.parseFromString(json);
    if(rc)
        aError("failed to parse json from string");
    return value;
}


JsonValue JsonValue::FromFile(const StringView filePath)
{
    JsonValue value;
    errc_t rc = value.parseFromFile(filePath);
    if(rc)
        aError("failed to parse json from file '%.*s'", filePath.size(), filePath.data());
    return value;
}

// 默认构造函数
JsonValue::JsonValue()
    : type_(EJsonValueType::eNull)
{
    value_.boolean_ = false; // 初始化联合体
}

// 布尔值构造函数
JsonValue::JsonValue(bool value)
    : type_(EJsonValueType::eBool)
{
    value_.boolean_ = value;
}

// 整数构造函数
JsonValue::JsonValue(int value)
    : type_(EJsonValueType::eNumber)
{
    value_.number_ = static_cast<double>(value);
}

// 双精度浮点数构造函数
JsonValue::JsonValue(double value)
    : type_(EJsonValueType::eNumber)
{
    value_.number_ = value;
}

// 字符串构造函数（C字符串）
JsonValue::JsonValue(const char* value)
    : type_(EJsonValueType::eString)
{
    value_.string_ = new std::string(value);
}

// 字符串构造函数
JsonValue::JsonValue(const std::string& value)
    : type_(EJsonValueType::eString)
{
    value_.string_ = new std::string(value);
}

// 字符串视图构造函数
JsonValue::JsonValue(StringView value)
    : type_(EJsonValueType::eString)
{
    value_.string_ = new std::string(value.data(), value.size());
}

// 数组构造函数
JsonValue::JsonValue(const std::vector<JsonValue>& values)
    : type_(EJsonValueType::eArray)
{
    value_.array_ = new std::vector<JsonValue>(values);
}

// 对象构造函数
JsonValue::JsonValue(const std::map<std::string, JsonValue>& values)
    : type_(EJsonValueType::eObject)
{
    value_.object_ = new std::map<std::string, JsonValue>(values);
}

// 拷贝构造函数
JsonValue::JsonValue(const JsonValue& other)
    : type_(other.type_)
{
    switch (type_)
    {
    case EJsonValueType::eNull:
        value_.boolean_ = false;
        break;
    case EJsonValueType::eBool:
        value_.boolean_ = other.value_.boolean_;
        break;
    case EJsonValueType::eNumber:
        value_.number_ = other.value_.number_;
        break;
    case EJsonValueType::eString:
        value_.string_ = new std::string(*other.value_.string_);
        break;
    case EJsonValueType::eArray:
        value_.array_ = new std::vector<JsonValue>(*other.value_.array_);
        break;
    case EJsonValueType::eObject:
        value_.object_ = new std::map<std::string, JsonValue>(*other.value_.object_);
        break;
    }
}

// 移动构造函数
JsonValue::JsonValue(JsonValue&& other) noexcept
    : type_(other.type_)
{
    value_ = other.value_;
    other.value_ = {};
    other.type_ = EJsonValueType::eNull;
}

// 析构函数
JsonValue::~JsonValue()
{
    destroy();
}

// 拷贝赋值运算符
JsonValue& JsonValue::operator=(const JsonValue& other)
{
    if (this != &other)
    {
        destroy();
        type_ = other.type_;
        
        switch (type_)
        {
        case EJsonValueType::eNull:
            value_.boolean_ = false;
            break;
        case EJsonValueType::eBool:
            value_.boolean_ = other.value_.boolean_;
            break;
        case EJsonValueType::eNumber:
            value_.number_ = other.value_.number_;
            break;
        case EJsonValueType::eString:
            value_.string_ = new std::string(*other.value_.string_);
            break;
        case EJsonValueType::eArray:
            value_.array_ = new std::vector<JsonValue>(*other.value_.array_);
            break;
        case EJsonValueType::eObject:
            value_.object_ = new std::map<std::string, JsonValue>(*other.value_.object_);
            break;
        }
    }
    return *this;
}

// 移动赋值运算符
JsonValue& JsonValue::operator=(JsonValue&& other) noexcept
{
    if (this != &other)
    {
        std::swap(type_, other.type_);
        std::swap(value_, other.value_);
    }
    return *this;
}

// 获取值类型
EJsonValueType JsonValue::type() const
{
    return type_;
}

// 判断是否为 null 值
bool JsonValue::isNull() const
{
    return type_ == EJsonValueType::eNull;
}

// 判断是否为布尔值
bool JsonValue::isBool() const
{
    return type_ == EJsonValueType::eBool;
}

// 判断是否为数值
bool JsonValue::isNumber() const
{
    return type_ == EJsonValueType::eNumber;
}

// 判断是否为字符串
bool JsonValue::isString() const
{
    return type_ == EJsonValueType::eString;
}

// 判断是否为数组
bool JsonValue::isArray() const
{
    return type_ == EJsonValueType::eArray;
}

// 判断是否为对象
bool JsonValue::isObject() const
{
    return type_ == EJsonValueType::eObject;
}

// 转换为布尔值（安全版本）
bool JsonValue::toBool(bool defaultValue) const
{
    switch (type_)
    {
    case EJsonValueType::eNull:
        return defaultValue;
    case EJsonValueType::eBool:
        return value_.boolean_;
    case EJsonValueType::eNumber:
        return value_.number_ > 0.0;
    case EJsonValueType::eString:
    {
        bool value = false;
        errc_t ret = aParseBool(*value_.string_, value);
        if (ret == eNoError)
            return value;
        return defaultValue;
    }
    case EJsonValueType::eArray:
        return !value_.array_->empty();
    case EJsonValueType::eObject:
        return !value_.object_->empty();
    default:
        break;
    }
    return defaultValue;
}

// 转换为整数值（安全版本）
int JsonValue::toInt(int defaultValue) const
{
    switch (type_)
    {
    case EJsonValueType::eNull:
        return defaultValue;
    case EJsonValueType::eBool:
        return value_.boolean_ ? 1 : 0;
    case EJsonValueType::eNumber:
        return static_cast<int>(value_.number_);
    case EJsonValueType::eString:
    {
        int value = 0;
        errc_t ret = aParseInt(*value_.string_, value);
        if (ret == eNoError)
            return value;
        return defaultValue;
    }
    case EJsonValueType::eArray:
    case EJsonValueType::eObject:
    default:
        break;
    }
    return defaultValue;
}

// 转换为双精度浮点数值（安全版本）
double JsonValue::toDouble(double defaultValue) const
{
    switch (type_)
    {
    case EJsonValueType::eNull:
        return defaultValue;
    case EJsonValueType::eNumber:
        return value_.number_;
    case EJsonValueType::eBool:
        return value_.boolean_ ? 1.0 : 0.0;
    case EJsonValueType::eString:
    {
        double value = 0.0;
        errc_t ret = aParseDouble(*value_.string_, value);
        if (ret == eNoError)
            return value;
        return defaultValue;
    }
    case EJsonValueType::eArray:
    case EJsonValueType::eObject:
    default:
        break;
    }
    return defaultValue;
}

// 转换为字符串值（安全版本）
std::string JsonValue::toString(StringView defaultValue) const
{
    switch (type_)
    {
    case EJsonValueType::eNull:
        return "null";
    case EJsonValueType::eBool:
        return value_.boolean_ ? "true" : "false";
    case EJsonValueType::eNumber:
    {
        return aFormatDouble(value_.number_);
    }
    case EJsonValueType::eString:
        return *value_.string_;
    case EJsonValueType::eArray:
        return "[array of " + aFormatInt((int)value_.array_->size()) + " elements]";
    case EJsonValueType::eObject:
        return "[object with " + aFormatInt((int)value_.object_->size()) + " properties]";
    }
    return std::string(defaultValue);
}

// 安全获取数组引用
const std::vector<JsonValue>& JsonValue::getArray(const std::vector<JsonValue>& defaultValue) const
{
    if (type_ == EJsonValueType::eArray)
    {
        return *value_.array_;
    }
    return defaultValue;
}


// 获取数组或对象的元素数量
size_t JsonValue::size() const
{
    if (type_ == EJsonValueType::eArray)
    {
        return value_.array_->size();
    }
    if (type_ == EJsonValueType::eObject)
    {
        return value_.object_->size();
    }
    return 0;
}


// 安全获取对象引用
const std::map<std::string, JsonValue>& JsonValue::getObject(const std::map<std::string, JsonValue>& defaultValue) const
{
    if (type_ == EJsonValueType::eObject)
    {
        return *value_.object_;
    }
    return defaultValue;
}

// 转换为布尔值运算符
JsonValue::operator bool() const
{
    return toBool();
}

// 转换为整数值运算符
JsonValue::operator int() const
{
    return toInt();
}

// 转换为双精度浮点数值运算符
JsonValue::operator double() const
{
    return toDouble();
}

// 转换为字符串值运算符
JsonValue::operator std::string() const
{
    return toString();
}


// 数组下标运算符
JsonValue& JsonValue::operator[](size_t index)
{
    if(type_ == EJsonValueType::eNull)
    {
        this->setArray({});
    }
    if (type_ == EJsonValueType::eArray)
    {
        if(index >= value_.array_->size())
            value_.array_->resize(index + 1);
        return (*value_.array_)[index];
    }
    return NullValue();
}


JsonValue& JsonValue::operator[](int index)
{
    return this->operator[](size_t(index));
}

// 数组下标运算符（常量版本）
const JsonValue& JsonValue::operator[](size_t index) const
{
    if (type_ == EJsonValueType::eArray && index < value_.array_->size())
    {
        return (*value_.array_)[index];
    }
    return NullValue();
}

const JsonValue& JsonValue::operator[](int index) const
{
    return this->operator[](size_t(index));
}


// 对象下标运算符（std::string）
JsonValue& JsonValue::operator[](const std::string& key)
{
    if(type_ == EJsonValueType::eNull)
    {
        this->setObject({});
    }
    return const_cast<JsonValue&>(const_cast<const JsonValue&>(*this)[key]);
}

JsonValue& JsonValue::operator[](const char* key)
{
    return this->operator[](std::string(key));
}

const JsonValue& JsonValue::operator[](const std::string& key) const
{
    if (type_ == EJsonValueType::eObject)
    {
        return (*value_.object_)[key];
    }
    return NullValue();
}

const JsonValue& JsonValue::operator[](const char* key) const
{
    return this->operator[](std::string(key));
}



// 设置 null 值
void JsonValue::setNull()
{
    destroy();
    type_ = EJsonValueType::eNull;
    value_.boolean_ = false;
}

// 设置布尔值
void JsonValue::setBool(bool value)
{
    destroy();
    type_ = EJsonValueType::eBool;
    value_.boolean_ = value;
}

// 设置整数值
void JsonValue::setInt(int value)
{
    destroy();
    type_ = EJsonValueType::eNumber;
    value_.number_ = static_cast<double>(value);
}

// 设置双精度浮点数值
void JsonValue::setDouble(double value)
{
    destroy();
    type_ = EJsonValueType::eNumber;
    value_.number_ = value;
}

// 设置字符串值
void JsonValue::setString(StringView value)
{
    destroy();
    type_ = EJsonValueType::eString;
    value_.string_ = new std::string(value);
}

// 设置数组值
void JsonValue::setArray(const std::vector<JsonValue>& values)
{
    destroy();
    type_ = EJsonValueType::eArray;
    value_.array_ = new std::vector<JsonValue>(values);
}

// 设置对象值
void JsonValue::setObject(const std::map<std::string, JsonValue>& values)
{
    destroy();
    type_ = EJsonValueType::eObject;
    value_.object_ = new std::map<std::string, JsonValue>(values);
}

// 清空值
void JsonValue::clear()
{
    destroy();
    type_ = EJsonValueType::eNull;
    value_.boolean_ = false;
}

void JsonValue::insert(const std::string& name, JsonValue value)
{
    if(type_ == EJsonValueType::eNull)
    {
        this->setObject({});
    }
    if (type_ == EJsonValueType::eObject)
    {
        value_.object_->insert({name, value});
    }
}

void JsonValue::append(JsonValue value)
{
    if(type_ == EJsonValueType::eNull)
    {
        this->setArray({});
    }
    if (type_ == EJsonValueType::eArray)
    {
        value_.array_->push_back(value);
    }
}


// 转换为 JSON 字符串表示
std::string JsonValue::toJsonString(int indent) const
{
    return formatString(0, indent);
}


// 释放资源
void JsonValue::destroy()
{
    switch (type_)
    {
    case EJsonValueType::eString:
        delete value_.string_;
        break;
    case EJsonValueType::eArray:
        delete value_.array_;
        break;
    case EJsonValueType::eObject:
        delete value_.object_;
        break;
    default:
        break;
    }
    type_ = EJsonValueType::eNull;
}

// 格式化字符串辅助函数
std::string JsonValue::formatString(int indent, int indentSize) const
{
    std::ostringstream oss;
    
    switch (type_)
    {
    case EJsonValueType::eNull:
        oss << "null";
        break;
    
    case EJsonValueType::eBool:
        oss << (value_.boolean_ ? "true" : "false");
        break;
    
    case EJsonValueType::eNumber:
        {
            // 检查是否为整数
            double intPart;
            if (std::modf(value_.number_, &intPart) == 0.0)
            {
                // 整数
                oss << static_cast<int>(value_.number_);
            }
            else
            {
                // 浮点数
                oss << std::fixed << std::setprecision(15) << value_.number_;
                // 移除多余的零
                std::string str = oss.str();
                size_t pos = str.find_last_not_of('0');
                if (pos != std::string::npos && str[pos] == '.')
                {
                    pos++;
                }
                return str.substr(0, pos + 1);
            }
        }
        break;
    
    case EJsonValueType::eString:
        {
            oss << '"';
            for (char c : *value_.string_)
            {
                switch (c)
                {
                case '"': oss << "\\\""; break;
                case '\\': oss << "\\\\"; break;
                case '\b': oss << "\\b"; break;
                case '\f': oss << "\\f"; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;
                default:
                    if (static_cast<unsigned char>(c) < 0x20)
                    {
                        oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') 
                            << static_cast<int>(c);
                    }
                    else
                    {
                        oss << c;
                    }
                    break;
                }
            }
            oss << '"';
        }
        break;
    
    case EJsonValueType::eArray:
        {
            oss << '[';
            bool first = true;
            for (const auto& item : *value_.array_)
            {
                if (!first)
                {
                    oss << ',';
                }
                if (indentSize > 0)
                {
                    oss << '\n' << std::string((indent + 1) * indentSize, ' ');
                }
                oss << item.formatString(indent + 1, indentSize);
                first = false;
            }
            if (indentSize > 0 && !value_.array_->empty())
            {
                oss << '\n' << std::string(indent * indentSize, ' ');
            }
            oss << ']';
        }
        break;
    
    case EJsonValueType::eObject:
        {
            oss << '{';
            bool first = true;
            for (const auto& pair : *value_.object_)
            {
                if (!first)
                {
                    oss << ',';
                }
                if (indentSize > 0)
                {
                    oss << '\n' << std::string((indent + 1) * indentSize, ' ');
                }
                oss << '"' << pair.first << "\": ";
                oss << pair.second.formatString(indent + 1, indentSize);
                first = false;
            }
            if (indentSize > 0 && !value_.object_->empty())
            {
                oss << '\n' << std::string(indent * indentSize, ' ');
            }
            oss << '}';
        }
        break;
    }
    
    return oss.str();
}


errc_t JsonValue::parseFromString(StringView json)
{
    JsonParser parser;
    return parser.parseFromString(json, *this);
}

errc_t JsonValue::parseFromFile(StringView filepath)
{
    JsonParser parser;
    return parser.parseFromFile(filepath, *this);
}

AST_NAMESPACE_END



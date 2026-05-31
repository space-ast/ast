///
/// @file      CommandCommon.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-11
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

#pragma once

#include "AstGlobal.h"
#include "AstUtil/RTTIAPI.hpp"
#include "AstUtil/ValueView.hpp"
#include "AstUtil/Span.hpp"
#include "AstUtil/ParseFormat.hpp"
#include "CommandAPI.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <type_traits>


AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


#ifndef SWIG

namespace detail{

/// @brief 查找命令参数结束位置
constexpr int find_closing(const char* s, int p) {
    return s[p] == '>' ? p : find_closing(s, p + 1);
}

#ifdef A_CXX14
/// @brief 检查命令参数是否为指定类型
constexpr bool is_type(const char* s, int p, const char* type) {
    int i = 0;
    while (type[i] != '\0') {
        if (s[p + i] != type[i]) return false;
        ++i;
    }
    // 允许类型后紧跟 '>' 或 ':'（名称注释）
    return s[p + i] == '>' || s[p + i] == ':';
}
#else
// 辅助递归函数，比较类型字符串并检查终止字符
constexpr bool is_type_impl(const char* s, int p, const char* type, int i) {
    return type[i] == '\0'
               ? (s[p + i] == '>' || s[p + i] == ':')
               : (s[p + i] == type[i] && is_type_impl(s, p, type, i + 1));
}

/// @brief 检查命令参数是否为指定类型（C++11 兼容）
constexpr bool is_type(const char* s, int p, const char* type) {
    return is_type_impl(s, p, type, 0);
}
#endif

/// @brief 统计命令参数个数
constexpr int count_args(const char* s, int p = 0) {
    return s[p] == '\0' ? 0 :
           s[p] == '<'  ? 1 + count_args(s, p + 1) :
           count_args(s, p + 1);
}


enum{
    type_none = 0,
    type_bool = 1,
    type_int = 2,
    type_double = 3,
    type_string = 4,
    type_object = 5,
    num_types = 6,
};

constexpr uint64_t get_parameter_tag(const char* s, int p = 0) {
    return s[p] == '\0' ? 0 :
           s[p] == '<'  ? (
               is_type(s, p+1, "bool")   ? get_parameter_tag(s, find_closing(s, p)+1) * num_types + type_bool :
               is_type(s, p+1, "int")    ? get_parameter_tag(s, find_closing(s, p)+1) * num_types + type_int :
               is_type(s, p+1, "double") ? get_parameter_tag(s, find_closing(s, p)+1) * num_types + type_double :
               is_type(s, p+1, "string") ? get_parameter_tag(s, find_closing(s, p)+1) * num_types + type_string :
               is_type(s, p+1, "object") ? get_parameter_tag(s, find_closing(s, p)+1) * num_types + type_object :
               throw "invalid parameter type"
           ) :
           get_parameter_tag(s, p + 1);
}

template <int Pos, typename T>
struct arg_pair {
    static constexpr int pos = Pos;
    using type = T;
};


// 1. 定义类型码到 C++ 类型的映射 traits
template <int TypeCode>
struct type_from_tag;

template <> struct type_from_tag<type_bool> { using type = bool;           }; // <bool>   → bool
template <> struct type_from_tag<type_int> { using type = int;             }; // <int>    → int
template <> struct type_from_tag<type_double> { using type = double;       }; // <double> → double
template <> struct type_from_tag<type_string> { using type = StringView;   }; // <string> → StringView
template <> struct type_from_tag<type_object> { using type = Object*;      }; // <object> → Object*


// 2. 极度简化的 make_arg_pair
template <uint64_t Tag, int Pos>
struct make_arg_pair {
    using type = arg_pair<Pos, typename type_from_tag<Tag % num_types>::type>;
};

// 递归解码，用 std::tuple 累积 arg_pair 类型（避免 seq 可变参数包问题）
template <uint64_t Tag, int Pos, int Count>
struct decode_impl {
    using type = decltype(
        std::tuple_cat(
            std::tuple<typename make_arg_pair<Tag % num_types, Pos>::type>(),
            typename decode_impl<Tag / num_types, Pos + 1, Count - 1>::type()
        )
    );
};

template <uint64_t Tag, int Pos>
struct decode_impl<Tag, Pos, 0> {
    using type = std::tuple<>;
};



// 对外接口：给定 tag 和参数个数 N，得到 std::tuple<arg_pair<Pos, Type>...>
template <uint64_t Tag, int N>
using decode_tag = typename decode_impl<Tag, 0, N>::type;


inline std::vector<StringView> split(StringView str) 
{
    std::vector<StringView> result;
    const char* p = str.data();
    const char* end = p + str.size();

    while (p < end) {
        // 跳过前导空白
        while (p < end && std::isspace(static_cast<unsigned char>(*p))) {
            ++p;
        }
        if (p >= end) break;

        if (*p == '"') {
            ++p; // 跳过左引号
            const char* start = p;
            while (p < end && *p != '"') {
                ++p;
            }
            // 添加引号内的内容（可能为空）
            result.emplace_back(start, static_cast<std::size_t>(p - start));
            ++p; // 跳过右引号
        } else {
            const char* start = p;
            while (p < end && !std::isspace(static_cast<unsigned char>(*p)) && *p != '"') {
                ++p;
            }
            result.emplace_back(start, static_cast<std::size_t>(p - start));
        }
    }
    return result;
}

inline std::string extract_command_name(const char* tmpl) {
    std::string name;
    while (*tmpl && *tmpl != ' ' && *tmpl != '<') {
        name += *tmpl++;
    }
    return name;
}


// ================== 运行时字符串转类型 ==================

template <typename T>
T convert_token(StringView s);

template <>
inline bool convert_token<bool>(StringView s) {
    bool value;
    if (aParseBool(s, value))
        throw std::runtime_error(std::string("invalid bool value: ") + std::string(s));
    return value;
}

template <>
inline int convert_token<int>(StringView s) {
    int value;
    if (aParseInt(s, value))
        throw std::runtime_error(std::string("invalid int value: ") + std::string(s));
    return value;
}


template <>
inline double convert_token<double>(StringView s) {
    double value;
    if (aParseDouble(s, value))
        throw std::runtime_error(std::string("invalid double value: ") + std::string(s));
    return value;
}

template<>
inline Object* convert_token<Object*>(StringView s) {
    Object* value = aResolveObject(s);
    return value;
}

template <>
inline StringView convert_token<StringView>(StringView s) {
    return s;
}


// 处理 void 返回值
inline errc_t fill_result(CommandResult& /*result*/) {
    return 0;
}

// 处理 int（及其他整数类型）
template <typename T>
typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value, errc_t>::type
fill_result(CommandResult& /*result*/, T ret) {
    return static_cast<errc_t>(ret);
}

// 处理 std::vector<std::string>
inline errc_t fill_result(CommandResult& result, std::vector<std::string> vec) {
    result = std::move(vec);
    return 0;
}

// 处理 std::string
inline errc_t fill_result(CommandResult& result, const std::string& str) {
    result.push_back(str);
    return 0;
}
inline errc_t fill_result(CommandResult& result, std::string&& str) {
    result.push_back(std::move(str));
    return 0;
}
// 处理 const char*
inline errc_t fill_result(CommandResult& result, const char* str) {
    result.push_back(str);
    return 0;
}
template <typename T>
typename std::enable_if<!std::is_integral<T>::value &&
                        !std::is_same<T, std::vector<std::string>>::value &&
                        !std::is_same<T, std::string>::value &&
                        !std::is_same<T, const char*>::value, errc_t>::type
fill_result(CommandResult& /*result*/, T ret) {
    return static_cast<errc_t>(ret);
}


} // namespace detail

#endif

/*! @} */

AST_NAMESPACE_END

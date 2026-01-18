///
/// @file      Identifier.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-18
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
#include "AstUtil/IdentifierAPI.hpp"
#include "AstUtil/StringView.hpp"
#include <string>
#include <memory.h>
#include <cstring>

AST_NAMESPACE_BEGIN


/// @brief     标识符类
/// @details   标识符类表示一个字符串，用于存储和操作标识符。
///            标识符类是不可复制和赋值的，只能通过IdentifierTable创建。
///            相同名称的标识符的内存地址是相同的。
class Identifier {
private:
    uint32_t length_;         // 字符串长度（字节数）
    char data_[1];          // 内联数据（柔性数组）
    
    // 禁止拷贝和赋值
    Identifier(const Identifier&) = delete;
    Identifier& operator=(const Identifier&) = delete;
    
    // 私有构造函数，只能通过IdentifierTable创建
    Identifier(uint32_t len) : length_(len) {}
    
public:
    // 获取数据指针
    const char* data() const { return data_; }
    char* data()  { return data_; }

    
    // 获取长度
    uint32_t length() const { return length_; }
    
    // 转换为C字符串
    const char* c_str() const {
        return data_;
    }
    
    // 转换为std::string
    std::string toString() const {
        return std::string(data_, length_);
    }
    
    // 比较操作
    bool equals(const char* str, size_t len) const {
        if (len != length_) return false;
        return memcmp(data_, str, len) == 0;
    }
    
    bool equals(const char* str) const {
        return equals(str, std::char_traits<char>::length(str));
    }
    
    bool equals(const std::string& str) const {
        return equals(str.c_str(), str.length());
    }
    
    bool equals(StringView sv) const {
        return equals(sv.data(), sv.size());
    }

    /// @brief 计算哈希值
    static size_t hash(const char* data, size_t len) {
        // DJB2哈希算法
        size_t h = 5381;
        for (size_t i = 0; i < len; ++i) {
            h = ((h << 5) + h) + static_cast<unsigned char>(data[i]);
        }
        return h;
    }

    // 计算哈希值
    size_t hash() const {
        // DJB2哈希算法
        return hash(data_, length_);
    }
    
    
    // 检查是否以特定前缀开头
    bool starts_with(const char* prefix) const {
        size_t prefix_len = strlen(prefix);
        if (static_cast<size_t>(length_) < prefix_len) return false;
        return memcmp(data_, prefix, prefix_len) == 0;
    }
    
    bool starts_with(const std::string& prefix) const {
        return starts_with(prefix.c_str());
    }
    
    // 检查是否以特定后缀结尾
    bool ends_with(const char* suffix) const {
        size_t suffix_len = strlen(suffix);
        if (static_cast<size_t>(length_) < suffix_len) return false;
        return memcmp(data_ + length_ - suffix_len, suffix, suffix_len) == 0;
    }
    
    bool ends_with(const std::string& suffix) const {
        return ends_with(suffix.c_str());
    }
    
    // 友元类，允许IdentifierTable访问私有构造函数
    friend class IdentifierTable;
};


AST_NAMESPACE_END

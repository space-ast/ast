///
/// @file      IdentifierTable.hpp
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
#include "Identifier.hpp"
#include <vector>
#include <mutex>
#include <algorithm>        
#include <unordered_map>    //  for std::unordered_map
#include <memory>           //  for std::unique_ptr
#include <new>              //  for ::operator new

AST_NAMESPACE_BEGIN

// #define _AST_ENABLE_IDENTIFIER_TABLE_MUTEX


class IdentifierTable {
private:
    // 释放变长分配的 Identifier：先析构，再交给无尺寸的全局 operator delete。
    // 不能用 unique_ptr 的默认 deleter，否则 -delete 表达式会选中 sized operator delete，
    // 传入的尺寸是 sizeof(Identifier) 而非实际分配尺寸，ASan 会报 new-delete-type-mismatch。
    struct IdentifierDeleter {
        void operator()(Identifier* sym) const noexcept {
            if (sym) {
                sym->~Identifier();
                ::operator delete(sym);
            }
        }
    };
    using IdentifierPtr = std::unique_ptr<Identifier, IdentifierDeleter>;

    // 使用哈希表存储符号
    std::unordered_multimap<size_t, IdentifierPtr> table_{};
#ifdef _AST_ENABLE_IDENTIFIER_TABLE_MUTEX
    mutable std::mutex mutex_;
#endif
    
    // 计算字符串的哈希值（DJB2哈希算法）
    size_t compute_hash(const char* data, size_t length) const {
        return Identifier::hash(data, length);
    }
    
public:
    // 查找或创建符号
    Identifier* intern(const char* data, size_t length) {
        size_t hash = compute_hash(data, length);

#ifdef _AST_ENABLE_IDENTIFIER_TABLE_MUTEX
        std::lock_guard<std::mutex> lock(mutex_);
#endif
        // 查找现有符号
        auto range = table_.equal_range(hash);
        for (auto it = range.first; it != range.second; ++it) {
            Identifier* sym = it->second.get();
            if (sym->length() == length && 
                memcmp(sym->data(), data, length) == 0) {
                return sym;  // 返回现有符号
            }
            // ++it;
        }
        
        // 创建新符号
        // 标识符是变长对象（sizeof(Identifier) 之后是多出来的字符），
        // 用 ::operator new 分配、在缓冲区上定位构造；
        // 释放统一交给 IdentifierDeleter，显式走无尺寸的::operator delete，
        // 避免 delete 表达式带来尺寸不符的 sized deallocation
        size_t alloc_size = sizeof(Identifier) + length;
        void* memory = ::operator new(alloc_size);
        Identifier* sym = new (memory) Identifier(static_cast<uint32_t>(length));
        memcpy(sym->data(), data, length);
        sym->data()[length] = '\0';

        table_.emplace(hash, IdentifierPtr(sym));
        return sym;
    }
    
    // 重载版本，接受C字符串
    Identifier* intern(const char* str) {
        return intern(str, strlen(str));
    }
    
    // 重载版本，接受std::string
    Identifier* intern(const std::string& str) {
        return intern(str.c_str(), str.length());
    }
    
    // 查找符号（不创建）
    Identifier* lookup(const char* data, size_t length) const {
        size_t hash = compute_hash(data, length);
        
#ifdef _AST_ENABLE_IDENTIFIER_TABLE_MUTEX
        std::lock_guard<std::mutex> lock(mutex_);
#endif
        auto range = table_.equal_range(hash);
        
        for (auto it = range.first; it != range.second; ++it) {
            Identifier* sym = it->second.get();
            if (sym->length() == length && 
                memcmp(sym->data(), data, length) == 0) {
                return sym;
            }
        }
        
        return nullptr;
    }
    
    // 获取符号数量
    size_t size() const {
#ifdef _AST_ENABLE_IDENTIFIER_TABLE_MUTEX
        std::lock_guard<std::mutex> lock(mutex_);
#endif
        return table_.size();
    }
    
    // 清理符号表（移除所有符号）
    void clear() {
#ifdef _AST_ENABLE_IDENTIFIER_TABLE_MUTEX
        std::lock_guard<std::mutex> lock(mutex_);
#endif
        table_.clear();
    }
};


AST_NAMESPACE_END

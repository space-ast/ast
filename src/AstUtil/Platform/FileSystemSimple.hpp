///
/// @file      SimpleFileSystem.hpp
/// @brief     c++17 标准库 filesystem的简化实现，保证api规范一致，使得在c++11时也能使用filesystem
/// @details   ~
/// @author    axel
/// @date      23.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.
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

#include <string>
#include <vector>
#include <system_error>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <algorithm>
#include <cstring>
#include <memory>

 
AST_NAMESPACE_BEGIN
 


#define _AST_FS _AST fs_simple::


/// @brief 简化的文件系统操作类
/// @details  本类提供了简化的文件系统操作接口，与c++17标准库filesystem的api规范保持一致。
///           主要功能包括路径操作、文件属性查询、目录遍历等。
/// @ingroup Platform
namespace fs_simple
{
    // 定义平台相关的字符串类型
    using string_type = std::string;
    using char_type = char;

    // 错误码
    class filesystem_error : public std::system_error
    {
    public:
        using std::system_error::system_error;
        explicit filesystem_error(
            const std::string& what, 
            std::error_code ec=std::make_error_code(std::errc::operation_not_supported)
        ): std::system_error(ec, what)
        {}
    };

    // 路径类
    class path
    {
    public:
        path() = default;
        
        path(const char_type* source) : path_(source)
        {}
        path(const string_type& source) : path_(source)
        {}
        
        std::string string() const{return path_;}
        
        // 返回平台相关的字符串类型
        const string_type& native() const{return path_;}
        
        // 返回平台相关的字符指针
        const char_type* c_str() const{return path_.c_str();}

        // 辅助函数：判断是否为路径分隔符
        static bool is_separator(char_type c)
        {
            #ifdef _WIN32
            return c == '/' || c == '\\';
            #else
            return c == '/';
            #endif
        }

        static const char_type* separators()
        {
            #ifdef _WIN32
            return "/\\";
            #else
            return "/";
            #endif
        }

        static char_type preferred_separator()
        {
            #ifdef _WIN32
            return '\\';
            #else
            return '/';
            #endif
        }

        // 路径操作
        path filename() const
        {
            auto pos = path_.find_last_of(separators());
            if (pos == string_type::npos) return path_;
            return path_.substr(pos + 1);
        }

        path extension() const
        {
            auto name = filename().native();
            auto pos = name.find_last_of(char_type('.'));
            if (pos == string_type::npos) return string_type();
            return name.substr(pos);
        }

        path stem() const
        {
            auto name = filename().native();
            auto pos = name.find_last_of(char_type('.'));
            if (pos == string_type::npos) return name;
            return name.substr(0, pos);
        }

        path parent_path() const
        {
            auto pos = path_.find_last_of(separators());
            if (pos == string_type::npos) return string_type();
            return path_.substr(0, pos);
        }

        bool empty() const
        {
            return path_.empty();
        }

        // 操作符重载
        path operator/(const path& other) const
        {
            if (path_.empty()) return other;
            if (other.path_.empty()) return *this;

            // 确保路径分隔符正确
            bool needs_separator = !is_separator(path_.back()) && !is_separator(other.path_.front());
            if (needs_separator) {
                return path_ + preferred_separator() + other.path_;
            }
            return path_ + other.path_;
        }

        path& operator/=(const path& other)
        {
            *this = *this / other;
            return *this;
        }

        operator string_type() const
        {	
            return (path_);
        }
    private:
        string_type path_;
    };

    // 文件类型枚举
    enum class file_type
    {
        none,
        not_found,
        regular,
        directory,
        symlink,
        unknown
    };

    // 文件状态
    class file_status
    {
    public:
        file_status() : type_(file_type::none)
        {}
        explicit file_status(file_type type) : type_(type)
        {}

        file_type type() const
        {
            return type_;
        }

    private:
        file_type type_;
    };

    // 目录条目
    class directory_entry
    {

    public:
        directory_entry() = default;
        explicit directory_entry(const _AST_FS path& p) : path_(p)
        {}

        const _AST_FS path& path() const
        {
            return path_;
        }
        file_status status() const;
    private:
        _AST_FS path path_;
    };

    // 目录迭代器
    class AST_UTIL_API directory_iterator
    {
    public:
        directory_iterator() : impl_(nullptr)
        {}
        explicit directory_iterator(const path& p);
        ~directory_iterator() = default;

        directory_iterator(const directory_iterator&);
        directory_iterator& operator=(const directory_iterator&);
        directory_iterator(directory_iterator&& other) noexcept;
        directory_iterator& operator=(directory_iterator&& other) noexcept;

        const directory_entry& operator*() const
        {
            return entry_;
        }
        const directory_entry* operator->() const
        {
            return &entry_;
        }

        directory_iterator& operator++();

        bool operator==(const directory_iterator& other) const
        {
            return impl_ == other.impl_;
        }
        bool operator!=(const directory_iterator& other) const
        {
            return !(*this == other);
        }

    private:
        struct impl;
        std::shared_ptr<impl> impl_;
        directory_entry entry_;

        void read_next_entry();
    };

    // 基础文件操作
    AST_UTIL_API bool exists(const path& p);
    AST_UTIL_API bool is_directory(const path& p);
    AST_UTIL_API bool is_regular_file(const path& p);
    AST_UTIL_API uintmax_t file_size(const path& p);
    AST_UTIL_API file_status status(const path& p);

    // 目录操作
    AST_UTIL_API bool create_directory(const path& p);
    AST_UTIL_API bool create_directories(const path& p);
    AST_UTIL_API bool remove(const path& p);
    AST_UTIL_API uintmax_t remove_all(const path& p);

    // 文件操作
    AST_UTIL_API bool copy_file(const path& from, const path& to);
    AST_UTIL_API bool rename(const path& old_p, const path& new_p);

    // 目录迭代器相关
    inline directory_iterator begin(directory_iterator iter) noexcept
    {
        return iter;
    }
    inline directory_iterator end(const directory_iterator&) noexcept
    {
        return directory_iterator();
    }

    // 空间信息
    struct space_info
    {
        uintmax_t capacity;
        uintmax_t free;
        uintmax_t available;
    };
    inline bool operator==(const path& left, const path& right) 
    {	
        return (left.native() == right.native());
    }
    inline bool operator!=(const path& left, const path& right)
    {
        return (left.native() != right.native());
    }
    AST_UTIL_API space_info space(const path& p);

    // 最后修改时间（简化版，返回time_t）
    AST_UTIL_API std::time_t last_write_time(const path& p);

    // 当前路径相关函数
    AST_UTIL_API path current_path();
    AST_UTIL_API path current_path(std::error_code& ec) noexcept;
    AST_UTIL_API void current_path(const path& new_path);
    AST_UTIL_API void current_path(const path& new_path, std::error_code& ec) noexcept;

} // namespace simple_fs



AST_NAMESPACE_END
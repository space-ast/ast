///
/// @file      Scanner.hpp
/// @brief     扫描器抽象基类和具体实现
/// @details   提供统一的字符读取接口，支持不同的输入源
/// @author    jinke18
/// @date      2025-12-19
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
#include "AstUtil/StringView.hpp"
#include <cstdio>
#include <string>
#include <fstream>

AST_NAMESPACE_BEGIN

/// @brief 扫描器抽象基类
/// @details 提供统一的字符读取接口，支持不同的输入源（FILE*、std::string、std::fstream等）
class Scanner
{
public:
    virtual ~Scanner() = default;
    
    /// @brief 读取并消耗当前字符
    /// @return 当前字符，如果到达结尾返回EOF
    virtual char consume() = 0;
    
    /// @brief 查看当前字符（不移动指针）
    /// @return 当前字符，如果到达结尾返回EOF
    virtual char peek() const = 0;
    
    /// @brief 查看下一个字符（不移动指针）
    /// @return 下一个字符，如果到达结尾返回EOF
    virtual char peekNext() const = 0;
    
    /// @brief 检查是否到达结尾
    /// @return 如果到达结尾返回true，否则返回false
    virtual bool atEnd() const = 0;
    
    /// @brief 跳过当前字符
    virtual void skip() = 0;
    

};

/// @brief 字符串扫描器
/// @details 处理std::string类型的输入源
class StringScanner : public Scanner
{
public:
    StringScanner(const std::string& str) 
        : str_(str), current_(0) 
    {}
    
    StringScanner(StringView str) 
        : str_(str.to_string()), current_(0) 
    {}
    
    char consume() override;
    char peek() const override;
    char peekNext() const override;
    bool atEnd() const override;
    void skip() override;

    
private:
    std::string str_;  ///< 存储输入字符串
    size_t current_;   ///< 当前位置
};

/// @brief 文件扫描器
/// @details 处理FILE*类型的输入源
class FileScanner : public Scanner
{
public:
    FileScanner(FILE* file) 
        : file_(file), current_char_(0), next_char_(0), at_end_(false) 
    {
        // 预读取两个字符
        current_char_ = std::fgetc(file_);
        next_char_ = std::fgetc(file_);
        at_end_ = (current_char_ == EOF);
    }
    
    ~FileScanner() override;
    
    char consume() override;
    char peek() const override;
    char peekNext() const override;
    bool atEnd() const override;
    void skip() override;

    
private:
    FILE* file_;         ///< 文件指针
    int current_char_;   ///< 当前字符
    int next_char_;      ///< 下一个字符
    bool at_end_;        ///< 是否到达文件结尾
};

/// @brief 流扫描器
/// @details 处理std::fstream类型的输入源
class StreamScanner : public Scanner
{
public:
    StreamScanner(std::fstream& stream) 
        : stream_(stream), current_char_(0), next_char_(0), at_end_(false) 
    {
        // 预读取两个字符
        current_char_ = stream_.get();
        next_char_ = stream_.get();
        at_end_ = (current_char_ == EOF);
    }
    
    char consume() override;
    char peek() const override;
    char peekNext() const override;
    bool atEnd() const override;
    void skip() override;
    
private:
    std::fstream& stream_;  ///< 文件流引用
    int current_char_;      ///< 当前字符
    int next_char_;         ///< 下一个字符
    bool at_end_;           ///< 是否到达流结尾
};

AST_NAMESPACE_END

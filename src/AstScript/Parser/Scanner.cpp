///
/// @file      Scanner.cpp
/// @brief     扫描器具体实现
/// @details   实现Scanner抽象类的子类，支持不同的输入源
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

#include "Scanner.hpp"

AST_NAMESPACE_BEGIN

// StringScanner implementation

char StringScanner::consume()
{
    if (atEnd())
        return EOF;
    return str_[current_++];
}

char StringScanner::peek() const
{
    if (atEnd())
        return EOF;
    return str_[current_];
}

char StringScanner::peekNext() const
{
    if (current_ + 1 >= str_.size())
        return EOF;
    return str_[current_ + 1];
}

bool StringScanner::atEnd() const
{
    return current_ >= str_.size();
}

void StringScanner::skip()
{
    if (!atEnd())
        current_++;
}



// FileScanner implementation

FileScanner::~FileScanner()
{
    // 不需要关闭文件，因为FileScanner不拥有文件指针的所有权
}

char FileScanner::consume()
{
    if (atEnd())
        return EOF;
    
    char result = static_cast<char>(current_char_);
    current_char_ = next_char_;
    next_char_ = std::fgetc(file_);
    at_end_ = (current_char_ == EOF);
    return result;
}

char FileScanner::peek() const
{
    if (atEnd())
        return EOF;
    return static_cast<char>(current_char_);
}

char FileScanner::peekNext() const
{
    if (atEnd())
        return EOF;
    return (next_char_ == EOF) ? EOF : static_cast<char>(next_char_);
}

bool FileScanner::atEnd() const
{
    return at_end_;
}

void FileScanner::skip()
{
    consume(); // 跳过当前字符，使用consume()来更新状态
}



// StreamScanner implementation

char StreamScanner::consume()
{
    if (atEnd())
        return EOF;
    
    char result = static_cast<char>(current_char_);
    current_char_ = next_char_;
    next_char_ = stream_.get();
    at_end_ = (current_char_ == EOF);
    return result;
}

char StreamScanner::peek() const
{
    if (atEnd())
        return EOF;
    return static_cast<char>(current_char_);
}

char StreamScanner::peekNext() const
{
    if (atEnd())
        return EOF;
    return (next_char_ == EOF) ? EOF : static_cast<char>(next_char_);
}

bool StreamScanner::atEnd() const
{
    return at_end_;
}

void StreamScanner::skip()
{
    consume(); // 跳过当前字符，使用consume()来更新状态
}



AST_NAMESPACE_END
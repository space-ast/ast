///
/// @file      BlockKeyValueParser.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-08
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
#include "AstUtil/StringView.hpp"
#include "AstUtil/BKVItemView.hpp"
#include <cstdio>
#include <vector>

#include <fstream>

AST_NAMESPACE_BEGIN

class BKVSax;
class BKVItemView;
class ValueView;

/// @brief 键值对解析器（BlockKeyValueParser）
/// @details 本类实现了键值对解析器，用于解析键值对格式的文本。
/// @ingroup ParseFormat
class AST_UTIL_API BKVParser 
{
public:
    enum EToken{
        eBlockBegin,         ///< 块开始
        eBlockEnd,           ///< 块结束
        eKeyValue,           ///< 键值对
        eEOF,                ///< 文件结束
    };

    BKVParser();
    BKVParser(StringView filepath);
    ~BKVParser();


    /// @brief 获取当前行号
    /// @details 获取当前解析器所在的行号。
    /// @return 当前行号
    int getLineNumber();

    /// @brief 设置是否允许注释
    /// @details 设置是否允许解析注释行。
    /// @param allow 是否允许注释
    void setAllowComment(bool allow){ allowComment_ = allow; }

    /// @brief 是否允许注释
    /// @details 是否允许解析注释行。
    /// @return 是否允许注释
    bool allowComment() const { return allowComment_; }

    /// @brief 获取下一个键值对项
    /// @details 获取下一个键值对项，将键值对项的键和值存储在指定的视图中。
    /// @param key 键视图，用于存储键值对项的键。
    /// @param value 值视图，用于存储键值对项的值。
    /// @return 键值对项的类型（EToken）。
    EToken getNext(StringView& key, ValueView& value);
    
    /// @brief 获取下一个键值对项
    /// @details 获取下一个键值对项，将键值对项的键和值存储在指定的视图中。
    /// @param item 键值对项视图，用于存储键值对项的键和值。
    /// @return 键值对项的类型（EToken）。
    EToken getNext(BKVItemView& item);

    /// @brief 获取当前行
    /// @details 获取当前行的内容，不包含行结束符。
    /// @return 当前行的内容
    StringView getLine();

    /// @brief 获取当前行（去除首尾空格）
    /// @details 获取当前行的内容，不包含行结束符。
    /// @return 当前行的内容（去除首尾空格）
    StringView getLineTrim();

    /// @brief 获取当前行（跳过注释行）
    /// @details 获取当前行的内容，不包含行结束符。
    /// @return 当前行的内容（去除首尾空格）
    StringView getLineSkipComment();
    
    /// @brief 解析文件
    /// @details 解析指定路径的文件，将解析结果传递给指定的 sax 解析器。
    /// @param filepath 文件路径视图，指定要解析的文件路径。
    /// @param sax sax 解析器，用于处理解析结果。
    /// @return 解析错误码（err_t）。
    err_t parseFile(const StringView filepath, BKVSax& sax);

    /// @brief 解析当前打开的文件
    /// @details 解析当前打开的文件，将解析结果传递给指定的 sax 解析器。
    /// @param sax sax 解析器，用于处理解析结果。
    /// @return 解析错误码（err_t）。
    err_t parse(BKVSax& sax);
    
    /// @brief 打开文件
    /// @details 打开指定路径的文件，用于后续的解析操作。
    /// @param filepath 文件路径视图，指定要打开的文件路径。
    void open(StringView filepath);

    /// @brief 是否打开文件
    /// @details 判断当前是否有文件打开。
    /// @return 如果有文件打开则返回 true，否则返回 false。
    bool isOpen() const { return file_ != nullptr; }

    /// @brief 关闭文件
    /// @details 关闭当前打开的文件。
    void close();

    /// @brief 移动文件指针
    /// @details 移动当前打开文件的指针到指定位置。
    /// @param pos 偏移量，指定要移动的字节数。
    /// @param dir 方向，指定移动的方向（如 std::ios::beg, std::ios::cur, std::ios::end）。
    void seek(std::streamoff pos, std::ios::seekdir dir);

    /// @brief 获取当前文件指针位置
    /// @details 获取当前打开文件的指针位置。
    /// @return 当前文件指针位置
    std::streamoff tell();

    /// @brief 是否到达文件末尾
    /// @details 判断当前文件指针是否到达文件末尾。
    /// @return 如果到达文件末尾则返回 true，否则返回 false。
    bool eof() const { return feof(file_); }

protected:
    FILE* getFile() const { return file_; }
protected:
    FILE* file_;                        ///< 文件指针
    bool allowComment_;                 ///< 是否允许注释行
    std::vector<char> keyBuffer;        ///< 内存缓冲区 for key
    std::vector<char> valueBuffer;      ///< 内存缓冲区 for value
};


AST_NAMESPACE_END

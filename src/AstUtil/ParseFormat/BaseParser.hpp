///
/// @file      BaseParser.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-07
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
#include <cstdio>
#include <fstream>
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup ParseFormat
    @{
*/


/// @brief     从文件中读取一行文本，跳过行首的空白字符，移除行尾的换行符。
/// @param     buffer  用于存储读取文本的缓冲区。
/// @param     size    缓冲区的大小。
/// @param     file    文件指针，指向要读取的文件。
/// @return    如果成功读取到一行文本，返回指向缓冲区的指针；否则返回 nullptr。
AST_UTIL_CAPI char* fgetlinetrim(char* buffer, int size, FILE* file);



/// @brief 解析器基类
class AST_UTIL_API BaseParser
{
public:
    BaseParser();
    BaseParser(StringView filepath);
    ~BaseParser();

    // 基础操作
public: 
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

public:
    /// @brief 获取当前行（包含行结束符）
    /// @details 获取当前行的内容，包含行结束符。
    /// @return 当前行的内容（包含行结束符）
    StringView getLineWithNewline();

    /// @brief 获取当前行
    /// @details 获取当前行的内容，不包含行结束符。
    /// @return 当前行的内容
    StringView getLine();

    /// @brief 获取当前行（去除首尾空格）
    /// @details 获取当前行的内容，不包含行结束符。
    /// @return 当前行的内容（去除首尾空格）
    StringView getLineTrim();

    // 辅助信息
public:
    /// @brief 获取当前行号
    /// @details 获取当前解析器所在的行号。
    /// @return 当前行号
    int getLineNumber();

    /// @brief 获取当前文件路径
    /// @details 获取当前打开文件的路径。
    /// @return 当前文件路径
    std::string getFilePath() const;

    /// @brief 设置外部借用的文件指针，不转移文件指针的所有权。
    /// @details 设置当前解析器使用的文件指针为外部借用的文件指针。
    /// @param file 外部借用的文件指针
    void setBorrowedFile(FILE* file);

    /// @brief 设置文件指针，将文件指针的所有权转移给解析器。
    /// @details 设置当前解析器使用的文件指针，并将文件指针的所有权转移给解析器。
    /// @param file 文件指针
    void setOwnedFile(FILE* file);


protected:
    FILE* getFile() const { return file_; }
protected:
    FILE*             file_{nullptr};           ///< 文件指针
    bool              fileBorrowed_{false};     ///< 是否从外部借用文件指针
    std::vector<char> lineBuffer_;               ///< 行缓冲区
};


/*! @} */

AST_NAMESPACE_END

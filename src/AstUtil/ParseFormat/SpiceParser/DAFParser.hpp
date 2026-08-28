///
/// @file      SpiceDAFParser.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-09
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
#include "AstUtil/BaseParser.hpp"
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

struct DAF_FileRecord;
struct DAF_CommentArea;
struct DAF_SummaryRecords;
struct DAF_NameRecords;
struct DAF_ElementRecords;

/// @brief 检查指定路径是否为有效SPICE双精度数组文件（DAF）
/// @param filepath 文件路径
/// @return 如果文件有效且包含有效数据，则返回true；否则返回false
AST_UTIL_CAPI bool aIsValidDAFFile(StringView filepath);

/// @brief 检查指定路径是否为有效SPK星历文件
/// @param filepath 文件路径
/// @return 如果文件有效且包含有效数据，则返回true；否则返回false
AST_UTIL_CAPI bool aIsValidSPKFile(StringView filepath);


/// @brief SPICE 双精度数组文件解析器 DAF（Double Precision Array File，双精度数组文件）
/// @todo 在读取数据时需要处理数据的大小端问题
/// @details 该类用于解析SPICE的双精度数组文件（DAF），提取其中的数组数据。
/// @warning 该类型的单个实例暂不支持多线程并发访问
class AST_UTIL_API DAFParser: public BaseParser
{
public:
    using Record = std::array<char, 1024>;            ///< 记录缓冲区

    DAFParser()=default;
    explicit DAFParser(StringView filepath);
    ~DAFParser()=default;

    using BaseParser::open;
    
    /// @brief 解析DAF文件
    /// @details 读取文件记录头、摘要记录和名称记录。
    /// @return 成功返回0，否则返回错误码
    errc_t parse();

    /// @brief 解析指定路径的DAF文件
    /// @details 打开文件、解析文件记录头、摘要记录和名称记录。
    /// @param filepath 文件路径
    /// @return 成功返回0，否则返回错误码
    errc_t parse(StringView filepath);

    /// @brief 检查解析结果是否有效
    /// @return 如果DAF文件解析成功且包含有效数据，则返回true；否则返回false
    bool isValidFile() const;

    /// @brief 读取指定记录索引的数据到缓冲区
    /// @param recordIndex 记录索引，从0开始
    /// @param buffer 指向接收数据的缓冲区指针
    /// @param size 缓冲区大小，单位为字节
    /// @return 成功返回实际读取的字节数
    size_t readRecord(int recordIndex, void* buffer, size_t size) const;

    /// @brief 读取文件记录头到缓冲区
    /// @param buffer 指向接收数据的缓冲区指针
    /// @param size 缓冲区大小，单位为字节
    /// @return 成功返回实际读取的字节数
    size_t readFileRecord(void* buffer, size_t size) const;

    /// @brief 获取文件记录头
    /// @param fileRecord 接收文件记录头的引用
    /// @return 成功返回0，否则返回错误码
    errc_t getFileRecord(Record& fileRecord) const;


    /// @brief 获取所有摘要记录
    /// @param summaryRecords 接收摘要记录的向量
    /// @return 成功返回实际读取的字节数
    errc_t getSummaryRecords(std::vector<Record>& summaryRecords) const;


    /// @brief 获取文件记录头信息
    /// @param nd 双精度分量个数 ND
    /// @param ni 整型分量个数 NI
    /// @param fward 第一个摘要记录号
    /// @param bward 最后一个摘要记录号
    /// @param free 第一个空闲字地址
    /// @return 成功返回0，否则返回错误码
    errc_t getFileRecord(int& nd, int& ni, int& fward, int& bward, int& free) const;


    /// @brief 获取文件注释
    /// @param comment 接收文件注释的字符串引用
    /// @return 成功返回0，否则返回错误码
    errc_t getComment(std::string& comment) const;

    /// @brief 获取文件注释
    /// @param comment 接收文件注释的字符串向量引用
    /// @return 成功返回0，否则返回错误码
    errc_t getComment(std::vector<std::string>& comments) const;

    /// @brief 打印文件注释到指定文件流
    /// @param fp 文件流指针，默认输出到标准输出
    void printComment(std::FILE* fp=stdout) const;

protected:
    errc_t runTest();
    errc_t readSummaryRecords(int fward, int bward, std::vector<Record>& summaryRecords) const;
    const DAF_FileRecord* getFileRecord() const;
    
protected:
    Record                      fileRecord_{};              ///< 文件记录头
    bool                        isValidFile_=false;         ///< 文件是否有效
    bool                        isSameEndian_=false;        ///< 是否与当前系统字节序相同
};

/*! @} */

AST_NAMESPACE_END

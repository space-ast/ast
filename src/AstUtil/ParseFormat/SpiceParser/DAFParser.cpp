///
/// @file      DAFParser.cpp
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

#include "DAFParser.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/StringSplit.hpp"
#include "AstUtil/ScopedPtr.hpp"
#include <cstdint>
#include <iostream>
#include <limits>

AST_NAMESPACE_BEGIN


// 强制 1 字节对齐
#pragma pack(push, 1)

/*!
参考SPICE文档 doc\html\req\daf.html
*/


/// @brief 文件记录 (Record 1, 1024 字节)
struct DAF_FileRecord {
    char    locidw[8];          ///< 标识字，例如 "DAF/SPK"
    int32_t nd;                 ///< 双精度分量个数 ND
    int32_t ni;                 ///< 整型分量个数 NI
    char    locifn[60];         ///< 内部文件名或文件描述
    int32_t fward;              ///< 第一个摘要记录号
    int32_t bward;              ///< 最后一个摘要记录号
    int32_t free;               ///< 第一个空闲字地址
    char    locfmt[8];          ///< 二进制格式 "LTL-IEEE" 或 "BIG-IEEE"
    char    prenul[603];        ///< 填充空字符 (使 ftpstr 位于偏移 699)
    char    ftpstr[28];         ///< FTP 验证字符串
    char    pstnul[297];        ///< 填充至记录末尾 (确保总大小 1024)
};


/// @brief 注释记录
struct DAF_CommentArea{
    char comment[1024];         ///< 注释字符串，最大 1024 字节
};

/// @brief 摘要记录 (每个摘要记录的前 24 字节为头部信息)
struct DAF_SummaryRecords {
    double next;                 ///< 下一个摘要记录号 (存储为 double)
    double prev;                 ///< 上一个摘要记录号 (存储为 double)
    double nsum;                 ///< 本记录中摘要个数 (存储为 double)
    union{
        double  dval[1];         ///< 双精度数据
        int32_t ival[2];         ///< 整数数据
    } summaries[125];            ///< 摘要数据，最多 125 个双精度数或 250 个整型数
};




/// @brief 名称记录
/// 它是一个连续的字符数组，长度 1024 字节。
/// 每个名称长度为 NC = 8 * (ND + (NI+1)/2)，顺序存放
struct DAF_NameRecords{
    char names[1024];           ///< 顺序存放的名称字符串
};


/// @brief 元素记录
/// 它是一个连续的双精度数组，长度 128 字节。
/// 每个元素顺序存放。
struct DAF_ElementRecords{
    double elements[128];       ///< 顺序存放的元素数据
};


// 检查 DAF 文件记录的偏移量和大小是否符合预期

static_assert(offsetof(DAF_FileRecord, locifn) == 16, "offset of locifn must be 16");
static_assert(sizeof(DAF_FileRecord) == 1024, "DAF_FileRecord size must be 1024");
static_assert(sizeof(DAF_CommentArea) == 1024, "DAF_CommentArea size must be 1024");
static_assert(sizeof(DAF_SummaryRecords) == 1024, "DAF_SummaryRecords size must be 1024");
static_assert(sizeof(DAF_NameRecords) == 1024, "DAF_NameRecords size must be 1024");
static_assert(sizeof(DAF_ElementRecords) == 1024, "DAF_ElementRecords size must be 1024");



// 类型 21 扩展 MDA 记录
// 记录大小可变，最小 71，最大由 MAXTRM 决定
// 具体布局参照类型 1，但此处不固定

#pragma pack(pop)

bool _aIsValidDAFFile(StringView filepath, StringView locidw)
{
    ScopedPtr<FILE> fp = fopen(std::string(filepath).c_str(), "rb");
    if(fp)
    {
        DAF_FileRecord record{};
        // 必须完整读取一个文件记录，否则可能是截断/损坏的文件
        if(fread(&record, sizeof(record), 1, fp.get()) != 1)
            return false;
        if(locidw.size() > sizeof(record.locidw))
            return false;
        if(strncmp(record.locidw, locidw.data(), locidw.size()) == 0)
        {
            // 结构校验：nd/ni 及由它们推导的 summary 大小须在合法范围内，
            // 防止仅签名巧合但结构被破坏/截断的文件被误判为有效。
            const int nd = record.nd;
            const int ni = record.ni;
            const int ss = nd + (ni + 1) / 2;
            if(nd >= 0 && nd <= 124 &&
               ni >= 2 && ni <= 250 &&
               ss > 0 && ss <= 125)
                return true;
        }
    }
    return false;
}

bool aIsValidDAFFile(StringView filepath)
{
    return _aIsValidDAFFile(filepath, "DAF");
}

bool aIsValidSPKFile(StringView filepath)
{
    return _aIsValidDAFFile(filepath, "DAF/SPK");
}

DAFParser::DAFParser(StringView filepath)
{
    this->parse(filepath);
}

errc_t DAFParser::parse()
{
    isValidFile_ = false;
    auto file = getFile();
    if(!file)
        return eErrorInvalidFile;
    static_assert(sizeof(DAF_FileRecord) == sizeof(fileRecord_), "fileRecord_ size must be 1024");
    readFileRecord(fileRecord_.data(), fileRecord_.size());
    const DAF_FileRecord* fileRec = getFileRecord();
    int nd = fileRec->nd;
    int ni = fileRec->ni;

    int ss = nd + (ni + 1) / 2;     /// 摘要大小 (size of a single summary)
    int ns = 125 / ss;              /// 每个摘要记录可存储的摘要个数 (number of summaries)
    int nc = 8 * ss;                /// 每个名称长度 (number of characters in a single name)
    A_UNUSED(ns);
    A_UNUSED(nc);

    // 检查参数是否合法
    if(strncmp(fileRecord_.data(), "DAF", 3) != 0)
    {
        aError("invalid DAF file, locidw is not \"DAF\"");
        return eErrorInvalidFile;
    }
    if(nd < 0 || nd > 124)
    {
        aError("invalid DAF file, nd is %d, must be in [0, 124]", nd);
        return eErrorInvalidFile;
    }
    if(ni < 2 || ni > 250)
    {
        aError("invalid DAF file, ni is %d, must be in [2, 250]", ni);
        return eErrorInvalidFile;
    }
    if(ss > 125)
    {
        aError("invalid DAF file, ss is %d, must be in [0, 125]", ss);
        return eErrorInvalidFile;
    }

    isValidFile_ = true;
    return eNoError;
}

errc_t DAFParser::parse(StringView filepath)
{
    open(filepath);
    return parse();
}

bool DAFParser::isValidFile() const
{
    return isValidFile_;
}

size_t DAFParser::readRecord(int recordIndex, void *buffer, size_t size) const
{
    if(recordIndex < 0)
        return 0;
    return read(buffer, size, recordIndex * sizeof(Record));
}

size_t DAFParser::readFileRecord(void *buffer, size_t size) const
{
    return readRecord(0, buffer, size);
}

errc_t DAFParser::getFileRecord(Record &fileRecord) const
{
    if(!isValidFile_)
        return eErrorInvalidFile;
    fileRecord = fileRecord_;
    return eNoError;
}


errc_t DAFParser::getSummaryRecords(std::vector<Record> &summaryRecords) const
{
    if(!isValidFile_)
        return eErrorInvalidFile;
    const DAF_FileRecord* fileRec = getFileRecord();
    int fward = fileRec->fward;
    int bward = fileRec->bward;
    return readSummaryRecords(fward, bward, summaryRecords);
}

errc_t DAFParser::getFileRecord(int &nd, int &ni, int &fward, int &bward, int &free) const
{
    if(!isValidFile_)
        return eErrorInvalidFile;
    const DAF_FileRecord* fileRec = reinterpret_cast<const DAF_FileRecord*>(fileRecord_.data());
    nd = fileRec->nd;
    ni = fileRec->ni;
    fward = fileRec->fward;
    bward = fileRec->bward;
    free = fileRec->free;
    return eNoError;
}

errc_t DAFParser::getComment(std::string &comment) const
{
    if(!isValidFile_)
        return eErrorInvalidFile;
    const DAF_FileRecord* fileRec = getFileRecord();
    int fward = fileRec->fward;
    size_t size = (fward - 2) * 1024;
    std::vector<char> data(size);
    readRecord(1, data.data(), size);
    for(auto& c : data)
        if(c == '\4') c = '\0';
    comment = std::string(data.data(), size);
    return eNoError;
}

errc_t DAFParser::getComment(std::vector<std::string> &comments) const
{
    std::string comment;
    errc_t rc = getComment(comment);
    if(rc != eNoError)
        return rc;
    comments = aStrSplit(comment, '\0', SkipWhitespace()).operator std::vector<std::string>();
    return eNoError;
}

void DAFParser::printComment(std::FILE *fp) const
{
    std::string comment;
    errc_t rc = getComment(comment);
    if(rc != eNoError)
        return;
    std::fwrite(comment.data(), 1, comment.size(), fp);
}

errc_t DAFParser::runTest()
{
    auto file = getFile();
    if(!file)
        return -1;
    DAF_FileRecord fileRec{};
    DAF_CommentArea commentRec1{};
    DAF_CommentArea commentRec2{};
    DAF_SummaryRecords summaryRec1{};
    DAF_NameRecords nameRec1{};
    size_t size;
    size = fread(&fileRec, sizeof(DAF_FileRecord), 1, file);
    size = fread(&commentRec1, sizeof(DAF_CommentArea), 1, file);
    size = fread(&commentRec2, sizeof(DAF_CommentArea), 1, file);
    size = fread(&summaryRec1, sizeof(DAF_SummaryRecords), 1, file);
    size = fread(&nameRec1, sizeof(DAF_NameRecords), 1, file);
    // SPK_Descriptor spkDesc[14];
    // memcpy(spkDesc, summaryRec1.summaries, sizeof(SPK_Descriptor) * 14);


    // 假设已根据 fileRec.locfmt 判断字节序，并将整数转换为主机序（此处省略转换代码）
    int nd = fileRec.nd;            // 需转换
    int ni = fileRec.ni;            // 需转换

    int ss = nd + (ni + 1) / 2;     /// 摘要大小 (size of a single summary)
    int ns = 125 / ss;              /// 每个摘要记录可存储的摘要个数 (number of summaries)
    int nc = 8 * ss;                /// 每个名称长度 (number of characters in a single name)

    std::cout << "ND=" << nd << ", NI=" << ni << ", summary size=" << ss << " double, name size=" << nc << std::endl;

    A_UNUSED(size);
    A_UNUSED(ns);

    return 0;
}


errc_t DAFParser::readSummaryRecords(int fward, int bward, std::vector<Record>& summaryRecords) const
{
    int recordIndex = fward;
    constexpr size_t max_elem = 1024 * 1024 * 1024 / sizeof(Record);    //  1GB
    const size_t max_size = (std::min)(summaryRecords.max_size(), max_elem);
    while(1)
    {
        Record record;
        size_t size = readRecord(recordIndex - 1, &record, sizeof(Record));
        if(size != sizeof(Record))
        {
            aError("read record %d failed, size=%d", recordIndex, size);
            return -1;
        }
        static_assert(sizeof(DAF_SummaryRecords) == sizeof(Record), "DAF_SummaryRecords size must be 1024");
        if(summaryRecords.size() + 1 >= max_size) {
            aError("summaryRecords size exceeds max_size");
            return eErrorInvalidFile;
        }
        summaryRecords.push_back(record);
        int next = (int)reinterpret_cast<const DAF_SummaryRecords*>(record.data())->next;
        if(next == 0 || next == fward || next == recordIndex || recordIndex == bward)
            break;
        recordIndex = next;
    }
    return eNoError;
}

const DAF_FileRecord *DAFParser::getFileRecord() const
{
    return reinterpret_cast<const DAF_FileRecord*>(fileRecord_.data());
}

AST_NAMESPACE_END



///
/// @file      TLEDatabase.cpp
/// @brief     TLE 数据库实现 — TLE 文本文件解析与查询
/// @author    axel
/// @date      2026-07-24
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


#include "TLEDatabase.hpp"
#include "TLEDatabaseQuery.hpp"

#include "AstCore/TLELines.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/ParseFormat.hpp"
#include "AstUtil/StringUtil.hpp"
#include "AstUtil/ScopedPtr.hpp"

AST_NAMESPACE_BEGIN

// ============================================================================
// 内部辅助
// ============================================================================

namespace {

bool isLine1(StringView line)
{
    // TLE line 1: 列 1 = '1', 列 2 = ' '
    return line.size() >= 2 && line[0] == '1' && line[1] == ' ';
}

bool isLine2(StringView line)
{
    // TLE line 2: 列 1 = '2', 列 2 = ' '
    return line.size() >= 2 && line[0] == '2' && line[1] == ' ';
}

} // namespace

// ============================================================================
// 构造 / 析构 / 加载
// ============================================================================

TLEDatabase::TLEDatabase() = default;

TLEDatabase::TLEDatabase(StringView filePath)
{
    load(filePath);
}

TLEDatabase::~TLEDatabase() = default;

errc_t TLEDatabase::load(StringView filePath)
{
    filePath_ = std::string(filePath);
    tles_.clear();
    loaded_ = false;

    if (filePath.empty())
        return -1;

    if (errc_t rc = loadTleFile(filePath))
        return rc;

    loaded_ = true;
    return 0;
}

errc_t TLEDatabase::reload()
{
    if (filePath_.empty()) return -1;
    return load(filePath_);
}

// ============================================================================
// 查询
// ============================================================================

TLEDatabase::EntryList TLEDatabase::getEntries(const TLEDatabaseQuery& query) const
{
    if (query.isEmpty())
        return tles_;

    EntryList results;
    for (const auto& tle : tles_)
    {
        if (query.matches(tle))
            results.push_back(tle);
    }
    return results;
}

errc_t TLEDatabase::loadTleFile(StringView filePath)
{
    tles_.clear();

    std::string filePathStr(filePath);
    ScopedPtr<std::FILE> file = ast_fopen(filePathStr.c_str(), "r");
    if (!file)
        return -1;

    // 根据文件大小预估 TLE 条目数，减少 vector 重分配
    {
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        rewind(file);

        if (fileSize > 0)
        {
            // 保守估计：按 2 行格式（~140 字节/条）计算，额外留一点余量
            size_t estimated = static_cast<size_t>(fileSize) / 140 + 10;
            tles_.reserve(estimated);
        }
    }

    char buf[256];

    std::string pendingName;     // 待匹配的名称行（3 行格式）
    std::string currentLine1;    // 当前 TLE 组的第 1 行
    bool expectingLine2 = false; // 是否已收到第 1 行，等待第 2 行

    while (fgets(buf, sizeof(buf), file))
    {
        StringView line(buf);

        line = aStripAsciiWhitespace(line);

        if (line.empty())
            continue;

        if (isLine1(line))
        {
            // 收到新的第 1 行：丢弃之前未完成的组，开始新组
            currentLine1 = std::string(line);
            expectingLine2 = true;
        }
        else if (isLine2(line) && expectingLine2 && !currentLine1.empty())
        {
            // 收到匹配的第 2 行：构建完整 TLE
            TLELines tleLines;
            tleLines.name() = pendingName;
            tleLines.line1() = currentLine1;
            tleLines.line2() = std::string(line);

            TLE tle = TLE::FromLines(tleLines);
            tles_.push_back(std::move(tle));

            // 重置状态
            pendingName.clear();
            currentLine1.clear();
            expectingLine2 = false;
        }
        else if (!isLine2(line))
        {
            // 非 TLE 行 — 当作名称行
            pendingName = std::string(line);
            if (!expectingLine2)
            {
                // 没有进行中的 TLE 组，安全重置
                currentLine1.clear();
            }
            // 如果有进行中的 TLE 组（expectingLine2 == true），
            // 保留 currentLine1，允许 line1 和 line2 之间有额外行
        }
        // else: 孤立的第 2 行（没有对应的第 1 行）— 忽略
    }

    return 0;
}

AST_NAMESPACE_END

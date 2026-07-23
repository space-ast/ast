///
/// @file      SatelliteDatabase.cpp
/// @brief     卫星数据库实现 — SATCAT Legacy Text Format 解析
/// @author    axel
/// @date      2026-07-23
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


#include "SatelliteDatabase.hpp"
#include "SatelliteDatabaseEntry.hpp"
#include "SatelliteDatabaseQuery.hpp"

#include "AstCore/TimePoint.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/ParseFormat.hpp"
#include "AstUtil/StringUtil.hpp"
#include "AstUtil/ScopedPtr.hpp"
#include "AstUtil/Literals.hpp"

#include <set>
#include <algorithm>

AST_NAMESPACE_BEGIN

const char* USER_ENTERED_TLE = "User-entered TLE";

// ============================================================================
// 构造 / 析构 / 加载
// ============================================================================

SatelliteDatabase::SatelliteDatabase() = default;

SatelliteDatabase::SatelliteDatabase(StringView filePath)
{
    load(filePath);
}

SatelliteDatabase::~SatelliteDatabase() = default;

errc_t SatelliteDatabase::load(StringView filePath)
{
    filePath_ = std::string(filePath);
    entries_.clear();
    missions_.clear();
    loaded_ = false;

    if (filePath.empty())
        return -1;

    if (errc_t rc = loadSatcatFile(filePath))
        return rc;

    loaded_ = true;
    refreshMissions();
    return 0;
}

errc_t SatelliteDatabase::reload()
{
    if (filePath_.empty()) return -1;
    return load(filePath_);
}

// ============================================================================
// 底层查询
// ============================================================================

const std::vector<std::string>& SatelliteDatabase::getMissions() const
{
    return missions_;
}

std::vector<SatelliteDatabaseEntry> SatelliteDatabase::getEntries(const SatelliteDatabaseQuery& query) const
{
    if (query.isEmpty())
        return entries_;

    std::vector<SatelliteDatabaseEntry> results;
    for (const auto& entry : entries_)
    {
        if (query.matches(entry))
            results.push_back(entry);
    }
    return results;
}

const SatelliteDatabase::EntryList& SatelliteDatabase::getEntries() const
{
    return entries_;
}

// ============================================================================
// 辅助
// ============================================================================



void SatelliteDatabase::refreshMissions()
{
    std::set<std::string> missionSet;
    for (const auto& entry : entries_)
    {
        if (!entry.mission().empty())
            missionSet.insert(entry.mission());
    }
    missions_.assign(missionSet.begin(), missionSet.end());
}

// ============================================================================
// SATCAT 文件解析
// ============================================================================

errc_t SatelliteDatabase::loadSatcatFile(StringView filePath)
{
    entries_.clear();

    std::string filePathStr(filePath);
    ScopedPtr<std::FILE> file = ast_fopen(filePathStr.c_str(), "r");
    if (!file)
        return -1;

    char buf[256];

    while (fgets(buf, sizeof(buf), file))
    {
        StringView line(buf);

        line = aStripAsciiWhitespace(line);

        if (line.empty())
            continue;

        // SATCAT 行至少约 100 字符
        if (line.size() < 100)
            continue;

        auto entry = parseSatcatLine(line);
        if (!entry.commonName().empty())
            entries_.push_back(std::move(entry));
    }

    return 0;
}

SatelliteDatabaseEntry SatelliteDatabase::parseSatcatLine(StringView line)
{
    SatelliteDatabaseEntry entry;

    // 列 001-011: 国际编号
    entry.internationalDesignator_ = std::string(aStripAsciiWhitespace(line.substr(0, 11)));

    // 列 014-018: NORAD 目录编号
    entry.noradCatId_ = aParseInt(line.substr(13, 5));

    // 列 021: 载荷标志
    entry.payloadFlag_ = (line.size() > 20 && line[20] == '*');

    // 列 024-047: 卫星名称
    entry.commonName_ = std::string(aStripAsciiWhitespace(line.substr(23, 24)));

    // 列 050-054: 所有者
    entry.owner_ = std::string(aStripAsciiWhitespace(line.substr(49, 5)));

    // 列 057-066: 发射日期
    entry.launchDate_ = std::string(aStripAsciiWhitespace(line.substr(56, 10)));

    // 列 069-073: 发射场
    entry.launchSite_ = std::string(aStripAsciiWhitespace(line.substr(68, 5)));

    // 列 076-085: 衰减日期
    entry.decayDate_ = std::string(aStripAsciiWhitespace(line.substr(75, 10)));

    // 列 088-094: 轨道周期
    entry.period_ = aParseDouble(line.substr(87, 7)) * 1_min;

    // 列 097-101: 倾角
    entry.inclination_ = aParseDouble(line.substr(96, 5)) * 1_deg;

    // 列 104-109: 远地点高度
    entry.apogee_ = aParseDouble(line.substr(103, 6)) * 1_km;

    // 列 112-117: 近地点高度
    entry.perigee_ = aParseDouble(line.substr(111, 6)) * 1_km;

    // 列 120-127: RCS — "N/A" 或数值
    {
        auto rcsStr = std::string(aStripAsciiWhitespace(line.substr(119, 8)));
        if (rcsStr.empty() || rcsStr == "N/A")
            entry.rcs_ = std::numeric_limits<double>::quiet_NaN();
        else
            entry.rcs_ = aParseDouble(rcsStr);
    }

    // 列 130-132: 轨道状态码
    entry.orbitalStatusCode_ = std::string(aStripAsciiWhitespace(line.substr(129, 3)));

    // 派生 mission
    entry.mission_ = extractMission(entry.commonName_);

    return entry;
}

std::string SatelliteDatabase::extractMission(StringView name)
{
    if (name.empty())
        return {};

    // 使用 aStripLeadingAsciiWhitespace 去除前导空格
    auto trimmed = aStripLeadingAsciiWhitespace(name);

    // 找到首个空格
    size_t wordEnd = 0;
    while (wordEnd < trimmed.size() && trimmed[wordEnd] != ' ')
        ++wordEnd;

    if (wordEnd == 0)
        return {};

    std::string mission(trimmed.data(), wordEnd);

    // 去除末尾数字和连字符（如 "SL-1" → "SL"）
    while (!mission.empty() && ((mission.back() >= '0' && mission.back() <= '9') || mission.back() == '-'))
        mission.pop_back();

    if (mission.empty())
        mission.assign(trimmed.data(), wordEnd);

    return mission;
}

AST_NAMESPACE_END

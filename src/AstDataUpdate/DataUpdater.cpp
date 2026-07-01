///
/// @file      DataUpdater.cpp
/// @brief     动态数据更新器实现
/// @details   ~
/// @author    axel
/// @date      2026-06-30
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

#include "DataUpdater.hpp"
#include "AstCore/EOP.hpp"
#include "AstCore/LeapSecond.hpp"
#include "AstCore/SpaceWeather.hpp"
#include "AstCore/RunTime.hpp"
#include "AstUtil/Network.hpp"
#include "AstUtil/NetworkRequest.hpp"
#include "AstUtil/NetworkResponse.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/IO.hpp"
#include <ctime>
#include <cstdio>
#include <cstring>
#include <fstream>

AST_NAMESPACE_BEGIN

// ============================================================
// 内部常量
// ============================================================

/// 数据文件 URL
static const char* kEOPUrl  = "https://celestrak.org/SpaceData/EOP-All.txt";
static const char* kSWUrl   = "https://celestrak.org/SpaceData/SW-Last5Years.txt";
static const char* kLSKUrl  = "https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat";

/// 过期阈值（天数），-1 表示不过期（仅检查文件是否存在）
static const int kEOPMaxAgeDays = 30;
static const int kSWMaxAgeDays  = 7;
static const int kLSKMaxAgeDays = -1;

/// 备份目录名（相对于数据目录）
static const char* kBackupDirName = ".backups";

/// 读取文件头部时最多读取的字节数
static const size_t kHeaderReadSize = 1024;

// ============================================================
// 托管数据文件列表构建
// ============================================================

/// @brief 根据初始化配置构建需要管理的数据文件列表
static void buildManagedFiles(std::vector<DataUpdater::DataFileEntry>& entries)
{
    InitalizeConfig config;
    aGetInitalizeConfig(config);

    entries.clear();
    entries.resize(3);

    entries[0].name        = "EOP (地球定向参数)";
    entries[0].description = "包含极移、UT1-UTC、LOD、章动修正和 XYS 系数修正，\n用于 ICRF 与 ITRF 之间的高精度坐标变换。";
    entries[0].url         = kEOPUrl;
    entries[0].localPath   = config.eopFile_;
    entries[0].maxAgeDays  = kEOPMaxAgeDays;

    entries[1].name        = "空间天气 (F10.7/Kp/Ap)";
    entries[1].description = "包含 F10.7 太阳射电通量、Kp/Ap 地磁指数，\n用于 Jacchia-Roberts 等大气密度模型的驱动数据。";
    entries[1].url         = kSWUrl;
    entries[1].localPath   = config.spaceWeatherFile_;
    entries[1].maxAgeDays  = kSWMaxAgeDays;

    entries[2].name        = "闰秒 (Leap Second)";
    entries[2].description = "TAI-UTC 跳秒数据，用于 UTC 与 TAI 之间的时间系统转换。\n由 IERS Bulletin C 发布，不定期更新。";
    entries[2].url         = kLSKUrl;
    entries[2].localPath   = config.leapSecondFile_;
    entries[2].maxAgeDays  = kLSKMaxAgeDays;
}

// ============================================================
// 工具函数
// ============================================================

/// @brief 获取当前 UTC 时间的年月日结构
static void getCurrentDateUTC(int& year, int& month, int& day)
{
    std::time_t now = std::time(nullptr);
    std::tm* utc = std::gmtime(&now);
    year  = utc->tm_year + 1900;
    month = utc->tm_mon + 1;
    day   = utc->tm_mday;
}

/// @brief 解析日期字符串，支持两种格式
/// - "2026 Jan 25 14:10:24" / "2026 Jan 25 14:10:24 UTC" (UPDATED 字段)
/// - "2026-07-01" (文件修改时间回退)
static bool parseUpdatedDate(const std::string& dateStr, int& year, int& month, int& day)
{
    // 尝试 "YYYY-MM-DD" 格式
    if (sscanf(dateStr.c_str(), "%d-%d-%d", &year, &month, &day) == 3
        && month >= 1 && month <= 12 && day >= 1 && day <= 31)
        return true;

    // 尝试 "YYYY Mon DD ..." 格式
    static const char* kMonthNames[] = {
        "Jan","Feb","Mar","Apr","May","Jun",
        "Jul","Aug","Sep","Oct","Nov","Dec"
    };
    char monStr[4] = {};
    if (sscanf(dateStr.c_str(), "%d %3s %d", &year, monStr, &day) != 3)
        return false;

    month = 0;
    for (int i = 0; i < 12; ++i)
    {
        if (strcmp(monStr, kMonthNames[i]) == 0) { month = i + 1; break; }
    }
    return month > 0;
}

/// @brief 计算两个日期之间的天数差（date1 - date2）
static int dateDiffDays(int y1, int m1, int d1, int y2, int m2, int d2)
{
    // 使用 Julian Day Number 简化计算
    auto jdn = [](int y, int m, int d) -> int {
        int a = (14 - m) / 12;
        int yy = y + 4800 - a;
        int mm = m + 12 * a - 3;
        return d + (153 * mm + 2) / 5 + 365 * yy + yy / 4 - yy / 100 + yy / 400 - 32045;
    };
    return jdn(y1, m1, d1) - jdn(y2, m2, d2);
}

/// @brief 生成时间戳字符串（yyyyMMdd_HHmmss）
static std::string makeTimestamp()
{
    std::time_t now = std::time(nullptr);
    std::tm* local = std::localtime(&now);
    // 缓冲区需足够大以消除 GCC format-truncation 警告
    char buf[80];
    snprintf(buf, sizeof(buf), "%04d%02d%02d_%02d%02d%02d",
             local->tm_year + 1900, local->tm_mon + 1, local->tm_mday,
             local->tm_hour, local->tm_min, local->tm_sec);
    return buf;
}

/// @brief 统计指定文件在备份目录中的备份数量
static int countBackups(const std::string& backupDir, const std::string& localPath)
{
    if (!fs::is_directory(backupDir)) return 0;
    std::string baseName = fs::path(localPath).filename().string();
    int count = 0;
    for (auto& p : fs::directory_iterator(backupDir))
    {
        std::string fname = p.path().filename().string();
        if (fname.find(baseName + ".") == 0)
            ++count;
    }
    return count;
}

// ============================================================
// DataUpdater 实现
// ============================================================

std::string DataUpdater::readUpdatedField(const std::string& filepath)
{
    // 先尝试从文件头部读取 UPDATED 字段
    FILE* fp = posix::fopen(filepath.c_str(), "r");
    if (fp)
    {
        char buf[kHeaderReadSize] = {};
        size_t n = fread(buf, 1, sizeof(buf) - 1, fp);
        fclose(fp);

        if (n > 0)
        {
            buf[n] = '\0';
            const char* key = "UPDATED";
            const char* pos = strstr(buf, key);
            if (pos)
            {
                pos += 8;
                while (*pos == ' ') ++pos;

                std::string dateStr;
                while (*pos && *pos != '\r' && *pos != '\n')
                {
                    if (strncmp(pos, "UTC", 3) == 0) break;
                    dateStr += *pos++;
                }
                while (!dateStr.empty() && dateStr.back() == ' ')
                    dateStr.pop_back();
                if (!dateStr.empty())
                    return dateStr;
            }
        }
    }

    // 回退到文件修改时间
    std::time_t mtime = fs::last_write_time(filepath);
    if (mtime <= 0) return {};

    char buf[32];
    std::tm* t = std::gmtime(&mtime);
    if (!t) return {};
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
    return buf;
}

std::string DataUpdater::backupDir()
{
    return std::string(aDataDirGet()) + "/" + kBackupDirName;
}

std::string DataUpdater::makeBackupName(const std::string& filepath)
{
    std::string baseName = fs::path(filepath).filename().string();
    return backupDir() + "/" + baseName + "." + makeTimestamp();
}

std::vector<DataUpdater::DataFileEntry> DataUpdater::refreshFileList() const
{
    std::vector<DataFileEntry> entries;
    buildManagedFiles(entries);

    std::string backupdir = backupDir();

    for (auto& entry : entries)
    {
        entry.isOutdated = false;
        entry.lastError  = eNoError;

        // 读取文件日期和判断是否过期
        entry.fileDate = readUpdatedField(entry.localPath);
        if (entry.fileDate.empty())
        {
            // 无日期信息：若文件存在且 maxAgeDays 为 -1 则不标记过期
            entry.isOutdated = !(entry.maxAgeDays < 0 && fs::is_regular_file(entry.localPath));
        }
        else
        {
            int fYear = 0, fMonth = 0, fDay = 0;
            if (parseUpdatedDate(entry.fileDate, fYear, fMonth, fDay))
            {
                if (entry.maxAgeDays < 0)
                {
                    // maxAgeDays < 0 表示永不过期，仅检查文件存在即可
                    entry.isOutdated = false;
                }
                else
                {
                    int cYear, cMonth, cDay;
                    getCurrentDateUTC(cYear, cMonth, cDay);
                    int age = dateDiffDays(cYear, cMonth, cDay, fYear, fMonth, fDay);
                    entry.isOutdated = (age > entry.maxAgeDays);
                }
            }
            else
            {
                entry.isOutdated = true;
            }
        }

        // 统计备份数量
        entry.backupCount = countBackups(backupdir, entry.localPath);
    }

    return entries;
}

std::vector<std::string> DataUpdater::listBackups(const DataFileEntry& entry) const
{
    std::vector<std::string> result;
    std::string bdir = backupDir();
    if (!fs::is_directory(bdir)) return result;

    std::string baseName = fs::path(entry.localPath).filename().string();

    for (auto& p : fs::directory_iterator(bdir))
    {
        std::string fname = p.path().filename().string();
        if (fname.find(baseName + ".") == 0)
            result.push_back(p.path().string());
    }
    return result;
}

errc_t DataUpdater::updateFile(DataFileEntry& entry)
{
    entry.lastError = eNoError;

    if (entry.localPath.empty())
    {
        aError("DataUpdater: file path is empty");
        entry.lastError = eErrorNotInit;
        return eErrorNotInit;
    }

    std::string tmpPath = entry.localPath + ".tmp";

    // ---- 1. 下载到临时文件 ----
    aInfo("DataUpdater: downloading %s from %s", entry.name.c_str(), entry.url.c_str());

    errc_t err = aDownloadFile(entry.url, tmpPath);
    if (err)
    {
        aError("DataUpdater: download failed for %s (err=%d)", entry.name.c_str(), err);
        entry.lastError = err;
        return err;
    }

    aInfo("DataUpdater: downloaded to %s", tmpPath.c_str());

    // ---- 2. 校验 ----
    errc_t validateErr = eNoError;
    if (entry.localPath.find("EOP-All") != std::string::npos)
    {
        EOP eop;
        validateErr = eop.load(StringView(tmpPath));
        if (validateErr)
            aError("DataUpdater: EOP validation failed");
        else if (eop.size() == 0)
        { aError("DataUpdater: EOP validation failed — no entries"); validateErr = eErrorInvalidFile; }
    }
    else if (entry.localPath.find("SW-") != std::string::npos)
    {
        SpaceWeather sw;
        validateErr = sw.load(StringView(tmpPath));
        if (validateErr)
            aError("DataUpdater: SW validation failed");
        else if (sw.size() == 0)
        { aError("DataUpdater: SW validation failed — no entries"); validateErr = eErrorInvalidFile; }
    }
    else if (entry.localPath.find("Leap_Second") != std::string::npos)
    {
        LeapSecond ls;
        validateErr = ls.load(StringView(tmpPath));
        if (validateErr)
            aError("DataUpdater: LSK validation failed");
        else if (ls.data().size() == 0)
        { aError("DataUpdater: LSK validation failed — no entries"); validateErr = eErrorInvalidFile; }
    }

    if (validateErr)
    {
        fs::remove(tmpPath);
        entry.lastError = validateErr;
        return validateErr;
    }

    // ---- 3. 备份旧文件 ----
    if (fs::is_regular_file(entry.localPath))
    {
        std::string bdir = backupDir();
        if (!fs::create_directories(bdir))
        {
            aError("DataUpdater: failed to create backup directory '%s'", bdir.c_str());
            fs::remove(tmpPath);
            entry.lastError = eErrorInvalidFile;
            return eErrorInvalidFile;
        }

        std::string backupPath = makeBackupName(entry.localPath);
        if (!fs::rename(entry.localPath, backupPath))
        {
            aError("DataUpdater: failed to backup old file '%s' -> '%s'",
                   entry.localPath.c_str(), backupPath.c_str());
            fs::remove(tmpPath);
            entry.lastError = eErrorInvalidFile;
            return eErrorInvalidFile;
        }
        aInfo("DataUpdater: backup saved to %s", backupPath.c_str());
    }

    // ---- 4. 安装新文件 ----
    if (!fs::rename(tmpPath, entry.localPath))
    {
        aError("DataUpdater: failed to install new file '%s'",
               entry.localPath.c_str());
        fs::remove(tmpPath);
        entry.lastError = eErrorInvalidFile;
        return eErrorInvalidFile;
    }

    // ---- 5. 刷新条目状态 ----
    entry.fileDate   = readUpdatedField(entry.localPath);
    entry.isOutdated = false;
    entry.lastError  = eNoError;

    // 更新备份计数
    std::string bdir = backupDir();
    entry.backupCount = countBackups(bdir, entry.localPath);

    aInfo("DataUpdater: %s updated successfully", entry.name.c_str());
    return eNoError;
}

int DataUpdater::updateAllOutdated()
{
    auto entries = refreshFileList();
    int count = 0;
    for (auto& e : entries)
    {
        if (e.isOutdated)
        {
            if (updateFile(e) == eNoError)
                ++count;
        }
    }
    return count;
}

AST_NAMESPACE_END

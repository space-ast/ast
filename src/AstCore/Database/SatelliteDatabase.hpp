///
/// @file      SatelliteDatabase.hpp
/// @brief     卫星数据库
/// @details   管理和查询卫星数据库，支持从 CelesTrak SATCAT Legacy Text Format 加载数据。
///           继承自 Database 空基类。
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


#pragma once

#include "Database.hpp"
#include "AstUtil/StringView.hpp"
#include "AstCore/TimePoint.hpp"
#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

/*! 
    @addtogroup 
    @{ 
*/

class SatelliteDatabaseEntry;
class SatelliteDatabaseQuery;
class TimePoint;

/// @brief 卫星数据库
/// @details 
/// 继承自 Database，可解析 CelesTrak SATCAT Legacy Text Format 格式的卫星目录文件。
/// - 每行表示一颗卫星，固定 132 字符列宽
/// - 类别（mission）从卫星名称自动派生（取名称首词）
class AST_CORE_API SatelliteDatabase : public Database
{
public:
    using Entry = SatelliteDatabaseEntry;
    using EntryList = std::vector<SatelliteDatabaseEntry>;

    SatelliteDatabase();
    explicit SatelliteDatabase(StringView filePath);
    ~SatelliteDatabase() override;

    // -- 加载 --
    errc_t load(StringView filePath);
    errc_t reload();
    bool isLoaded() const { return loaded_; }

    const TimePoint& lastUpdateTime() const { return lastUpdateTime_; }

    // -- 底层查询 --
    const std::vector<std::string>&     getMissions() const;
    EntryList                           getEntries(const SatelliteDatabaseQuery& query) const;
    const EntryList&                    getEntries() const;
    size_t                              size() const { return entries_.size(); }

    const std::string& filePath() const { return filePath_; }

private:
    void refreshMissions();

    /// @brief 解析 SATCAT Legacy Text Format 文件
    errc_t loadSatcatFile(StringView filePath);

private:
    std::string filePath_{};                          ///< 加载的文件路径
    TimePoint lastUpdateTime_{};                      ///< 最后更新时间
    std::vector<SatelliteDatabaseEntry> entries_{};   ///< 卫星数据库条目
    std::vector<std::string> missions_{};             ///< 任务类别（category of mission）
    bool loaded_{false};                              ///< 是否已加载数据
};

/*
celestrak SATCAT Legacy Text Data Format
参考：https://celestrak.org/satcat/satcat-format.php

Legacy Text Data Format:

------------------------------------------------------------------------------------------------------------------------------------
Column
000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111111111111111111111111111111111
000000000111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999000000000011111111112222222222333
123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012
------------------------------------------------------------------------------------------------------------------------------------
yyyy-nnnaaa  nnnnn M*O aaaaaaaaaaaaaaaaaaaaaaaa  aaaaa  yyyy-mm-dd  aaaaa  yyyy-mm-dd  nnnnn.n  nnn.n  nnnnnn  nnnnnn  nnn.nnnn  aaa
------------------------------------------------------------------------------------------------------------------------------------
Columns	Description
001-011	International Designator
        - Launch Year (001-004)
        - Launch of the Year (006-008)
        - Piece of the Launch (009-011)
014-018	NORAD Catalog Number
020-020	Multiple Name Flag ("M" if multiple names exist; alternate names found in satcat-annex.csv)
021-021	Payload Flag ("*" if payload; blank otherwise)
022-022	Operational Status Code
024-047	Satellite Name(s)
        - R/B(1) = Rocket body, first stage
        - R/B(2) = Rocket body, second stage
        - DEB = Debris
        - PLAT = Platform
        - Items in parentheses are alternate names
        - Items in brackets indicate type of object
          (e.g., BREEZE-M DEB [TANK] = tank)
        - An ampersand (&) indicates two or more objects are attached
050-054	Source or Ownership
057-066	Launch Date [year-month-day]
069-073	Launch Site
076-085	Decay Date, if applicable [year-month-day]
088-094	Orbital period [minutes]; truncated at 99999.9
097-101	Inclination [degrees]
104-109	Apogee Altitude [kilometers]; truncated at 999999
112-117	Perigee Altitude [kilometers]; truncated at 999999
120-127	Radar Cross Section [meters2]; N/A if no data available
130-132	Orbital Status Code
        - NCE = No Current Elements
        - NIE = No Initial Elements
        - NEA = No Elements Available
        - DOC = Permanently Docked
        - ISS = Docked to International Space Station
        - XXN = Central Body (XX) and Orbit Type (N)
            - AS = Asteroid
            - EA = Earth (default if blank)
            - EL = Earth Lagrange
            - EM = Earth-Moon Barycenter
            - JU = Jupiter
            - MA = Mars
            - ME = Mercury
            - MO = Moon (Earth)
            - NE = Neptune
            - PL = Pluto
            - SA = Saturn
            - SS = Solar System Escape
            - SU = Sun
            - UR = Uranus
            - VE = Venus
            - 0 = Orbit
            - 1 = Landing
            - 2 = Impact
            - 3 = Roundtrip
        (e.g., SU0 = Heliocentric Orbit, MO2 = Lunar Impact)

*/

/*! @} */

AST_NAMESPACE_END

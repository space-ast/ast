///
/// @file      SatelliteDatabaseEntry.hpp
/// @brief     卫星数据库条目
/// @details   存储单条 SATCAT 格式的卫星记录，包含国际编号、NORAD 编号、
///           轨道参数及可选的 TLE 数据。
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

#include "AstGlobal.h"
#include "AstUtil/StringView.hpp"
#include <string>
#include <memory>
#include <limits>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class TLE;

/// @brief 卫星数据库条目
class AST_CORE_API SatelliteDatabaseEntry
{
public:
    SatelliteDatabaseEntry();
    SatelliteDatabaseEntry(StringView commonName, StringView mission);
    ~SatelliteDatabaseEntry();


    // -- STK 兼容接口 --

    const std::string& commonName()          const { return commonName_; }
    std::string&       commonName()                { return commonName_; }
    const std::string& mission()             const { return mission_; }
    std::string&       mission()                   { return mission_; }

    // -- SATCAT 字段 --

    const std::string& internationalDesignator()  const { return internationalDesignator_; }
    int                noradCatId()               const { return noradCatId_; }
    bool               isPayload()                const { return payloadFlag_; }
    const std::string& owner()                    const { return owner_; }
    const std::string& launchDate()               const { return launchDate_; }
    const std::string& launchSite()               const { return launchSite_; }
    const std::string& decayDate()                const { return decayDate_; }
    double             period()                   const { return period_; }
    double             inclination()              const { return inclination_; }
    double             apogee()                   const { return apogee_; }
    double             perigee()                  const { return perigee_; }
    double             rcs()                      const { return rcs_; }
    const std::string& orbitalStatusCode()        const { return orbitalStatusCode_; }

    /// @brief 是否已衰减（有衰减日期）
    bool isDecayed() const { return !decayDate_.empty(); }

private:
    friend class SatelliteDatabase;

    // -- STK 兼容字段 --
    std::string commonName_{};               ///< 卫星通用名称（SATCAT 名称字段 trim 后）
    std::string mission_{};                  ///< 所属任务/类别（从名称派生）

    // -- SATCAT 字段 --
    std::string internationalDesignator_{};  ///< 国际编号 (YYYY-NNNAAA)
    int         noradCatId_{0};              ///< NORAD 目录编号
    bool        payloadFlag_{false};         ///< 载荷标志 (* = 载荷)
    std::string owner_{};                    ///< 所有者/来源 (cols 50-54)
    std::string launchDate_{};               ///< 发射日期 [YYYY-MM-DD]
    std::string launchSite_{};               ///< 发射场 (cols 69-73)
    std::string decayDate_{};                ///< 衰减日期 [YYYY-MM-DD]，空 = 在轨
    double      period_{0.0};                ///< 轨道周期 [s]
    double      inclination_{0.0};           ///< 倾角 [rad]
    double      apogee_{0.0};                ///< 远地点高度 [m]
    double      perigee_{0.0};               ///< 近地点高度 [m]
    double      rcs_{std::numeric_limits<double>::quiet_NaN()};///< RCS [m²] 或 "N/A"
    std::string orbitalStatusCode_{};        ///< 轨道状态码 (cols 130-132)
};

/*! @} */

AST_NAMESPACE_END

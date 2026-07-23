///
/// @file      SatelliteDatabaseQuery.hpp
/// @brief     卫星数据库查询条件
/// @details   定义对卫星数据库进行筛选查询的条件，支持按任务类别和通用名称过滤。
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

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class SatelliteDatabaseEntry;

/// @brief 卫星数据库查询条件
/// @details 支持按任务类别（mission）和通用名称（commonName）进行过滤匹配。
///          使用简单的通配符模式匹配（支持 * 和 ?）。
///          对应 STK Components 中的 StkSatelliteDatabaseQuery。
class AST_CORE_API SatelliteDatabaseQuery
{
public:
    /// @brief 默认构造（匹配所有条目）
    SatelliteDatabaseQuery();

    /// @brief 析构
    ~SatelliteDatabaseQuery();

    /// @brief 按任务类别过滤
    /// @param pattern 匹配模式，支持 *（匹配任意字符序列）和 ?（匹配单个字符）。
    ///                 空字符串表示不按此字段过滤。
    void setMission(StringView pattern);

    /// @brief 按通用名称过滤
    /// @param pattern 匹配模式，支持 *（匹配任意字符序列）和 ?（匹配单个字符）。
    ///                 空字符串表示不按此字段过滤。
    void setCommonName(StringView pattern);

    /// @brief 获取任务类别过滤模式
    const std::string& mission() const { return mission_; }

    /// @brief 获取通用名称过滤模式
    const std::string& commonName() const { return commonName_; }

    /// @brief 判断给定条目是否匹配当前查询条件
    /// @param entry 待检查的数据库条目
    /// @return 若匹配则返回 true
    bool matches(const SatelliteDatabaseEntry& entry) const;

    /// @brief 查询是否未设置任何过滤条件（匹配所有条目）
    bool isEmpty() const;
private:
    std::string mission_;                 ///< 任务类别过滤模式（空 = 不筛选）
    std::string commonName_;              ///< 通用名称过滤模式（空 = 不筛选）
};

/*! @} */

AST_NAMESPACE_END

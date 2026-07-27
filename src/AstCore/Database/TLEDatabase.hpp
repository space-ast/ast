///
/// @file      TLEDatabase.hpp
/// @brief     TLE 数据库
/// @details   管理和查询 TLE 数据库，支持从 TLE 文本文件（2 行或 3 行格式）加载数据。
///           继承自 Database 空基类。
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

#pragma once

#include "AstGlobal.h"
#include "Database.hpp"
#include "AstCore/TLE.hpp"
#include "AstUtil/StringView.hpp"
#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/

class TLEDatabaseQuery;

/// @brief TLE 数据库
/// @details
/// 继承自 Database，可解析标准 TLE 文本文件（支持 2 行和 3 行格式）。
/// - 3 行格式：名称行 + 第 1 行 + 第 2 行
/// - 2 行格式：第 1 行 + 第 2 行（无名称行）
/// - 格式通过行首字符 '1' / '2' 自动检测
class AST_CORE_API TLEDatabase : public Database
{
public:
    using Entry = TLE;
    using EntryList = std::vector<TLE>;

    TLEDatabase();
    explicit TLEDatabase(StringView filePath);
    ~TLEDatabase() override;

    // -- 加载 --
    errc_t load(StringView filePath);
    errc_t reload();
    bool isLoaded() const { return loaded_; }

    // -- 查询 --
    const EntryList& getEntries() const { return tles_; }
    EntryList getEntries(const TLEDatabaseQuery& query) const;
    size_t size() const { return tles_.size(); }

    const std::string& filePath() const { return filePath_; }

private:
    /// @brief 解析 TLE 文本文件
    errc_t loadTleFile(StringView filePath);

private:
    std::string filePath_{};    ///< 加载的文件路径
    std::vector<TLE> tles_{};   ///< TLE 条目
    bool loaded_{false};        ///< 是否已加载数据
};

/*! @} */

AST_NAMESPACE_END

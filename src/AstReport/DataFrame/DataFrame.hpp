///
/// @file      DataFrame.hpp
/// @brief     数据表 — 命名的多列数据集合
/// @details   持有多个 DataSeries，提供 pandas 风格的按名访问接口。
///           所有列具有相同的行数。
/// @author    axel
/// @date      2026-07-02
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
#include "AstReport/AstReportGlobal.hpp"
#include "DataSeries.hpp"

#include <string>
#include <vector>
#include <map>
#include <stdexcept>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup AstReport
    @{
*/

class AST_REPORT_API DataFrame
{
public:
    // ---- 构造 ----

    /// @brief 默认构造（空表）
    DataFrame();

    /// @brief 构造命名空表
    explicit DataFrame(const std::string& name);

    // ---- 基本信息 ----

    const std::string& name()  const { return name_; }
    void               setName(const std::string& name) { name_ = name; }

    size_t rowCount()    const;
    size_t columnCount() const { return columns_.size(); }
    bool   empty()       const { return columns_.empty(); }

    /// @brief 所有列名（按添加顺序）
    std::vector<std::string> columnNames() const;

    // ---- 列管理 ----

    /// @brief 添加空列（所有列需等长，首列决定行数）
    DataSeries& addColumn(const std::string& name, EDataType type, size_t size);

    /// @brief 移动添加已有列（列长必须与已有列一致）
    DataSeries& addColumn(DataSeries&& series);

    /// @brief 移除列
    /// @return 是否成功移除
    bool removeColumn(const std::string& name);

    /// @brief 检查列是否存在
    bool hasColumn(const std::string& name) const;

    // ---- 列访问 ----

    /// @brief 按名访问列（不存在则抛出 std::out_of_range）
    DataSeries& operator[](const std::string& name);

    /// @brief 按名访问列（const 版本）
    const DataSeries& operator[](const std::string& name) const;

    /// @brief 按索引访问列
    DataSeries& column(size_t index);

    /// @brief 按索引访问列（const 版本）
    const DataSeries& column(size_t index) const;

    // ---- 迭代器 ----

    using iterator       = std::vector<DataSeries>::iterator;
    using const_iterator = std::vector<DataSeries>::const_iterator;

    iterator       begin()       { return columns_.begin(); }
    iterator       end()         { return columns_.end(); }
    const_iterator begin() const { return columns_.begin(); }
    const_iterator end()   const { return columns_.end(); }

    void reserve(size_t size){columns_.reserve(size);}
private:
    void rebuildIndex();

    std::string                    name_{};
    std::vector<DataSeries>        columns_{};
    std::map<std::string, size_t>  index_{}; ///< 列名 → columns_ 下标
};

/*! @} */

AST_NAMESPACE_END

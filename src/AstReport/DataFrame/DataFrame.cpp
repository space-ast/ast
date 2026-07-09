///
/// @file      DataFrame.cpp
/// @brief     数据表实现
/// @details   ~
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

#include "DataFrame.hpp"

AST_NAMESPACE_BEGIN

// ---- 构造 ----

DataFrame::DataFrame() = default;

DataFrame::DataFrame(const std::string& name)
    : name_(name)
{
}

// ---- 基本信息 ----

size_t DataFrame::rowCount() const
{
    if (columns_.empty())
        return 0;
    return columns_[0].size();
}

std::vector<std::string> DataFrame::columnNames() const
{
    std::vector<std::string> names;
    names.reserve(columns_.size());
    for (const auto& col : columns_)
        names.push_back(col.name());
    return names;
}

// ---- 列管理 ----

DataSeries& DataFrame::addColumn(const std::string& name, EDataType type, size_t size)
{
    // 首列决定行数，后续列必须等长
    if (!columns_.empty() && size != rowCount())
        size = rowCount();

    columns_.emplace_back(name, type, size);
    index_[name] = columns_.size() - 1;
    return columns_.back();
}

DataSeries& DataFrame::addColumn(DataSeries&& series)
{
    // 如果表非空，调整列长以匹配
    if (!columns_.empty() && series.size() != rowCount())
        series.resize(rowCount());

    std::string name = series.name();
    columns_.push_back(std::move(series));
    index_[name] = columns_.size() - 1;
    return columns_.back();
}

bool DataFrame::removeColumn(const std::string& name)
{
    auto it = index_.find(name);
    if (it == index_.end())
        return false;

    size_t pos = it->second;
    columns_.erase(columns_.begin() + static_cast<ptrdiff_t>(pos));
    index_.erase(it);

    // 重建索引（因为后续列的位置变了）
    rebuildIndex();
    return true;
}

bool DataFrame::hasColumn(const std::string& name) const
{
    return index_.find(name) != index_.end();
}

// ---- 列访问 ----

DataSeries& DataFrame::operator[](const std::string& name)
{
    auto it = index_.find(name);
    if (it == index_.end())
        throw std::out_of_range("DataFrame: column '" + name + "' not found");
    return columns_[it->second];
}

const DataSeries& DataFrame::operator[](const std::string& name) const
{
    auto it = index_.find(name);
    if (it == index_.end())
        throw std::out_of_range("DataFrame: column '" + name + "' not found");
    return columns_[it->second];
}

DataSeries& DataFrame::column(size_t index)
{
    return columns_.at(index);
}

const DataSeries& DataFrame::column(size_t index) const
{
    return columns_.at(index);
}

// ---- 内部方法 ----

void DataFrame::rebuildIndex()
{
    index_.clear();
    for (size_t i = 0; i < columns_.size(); ++i)
        index_[columns_[i].name()] = i;
}

AST_NAMESPACE_END

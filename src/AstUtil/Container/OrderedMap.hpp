///
/// @file      OrderedMap.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-02
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
#include <vector>
#include <unordered_map>
#include <stdexcept>   // for std::out_of_range
#include <utility>     // for std::pair

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Container
    @{
*/


/// @brief 有序映射类，保持键值对的插入顺序
/// @tparam Key 键类型
/// @tparam Value 值类型
template<typename Key, typename Value>
class OrderedMap {
private:
    using vector_type = std::vector<std::pair<Key, Value>>;
    using map_type = std::unordered_map<Key, std::size_t>;
    
    // 按插入顺序存储键值对
    vector_type items_;
    // 键到下标位置的快速映射
    map_type index_map_;
public: // ---------- 迭代器支持 ----------

    using iterator = typename vector_type::iterator;
    using const_iterator = typename vector_type::const_iterator;

    iterator begin() { return items_.begin(); }
    iterator end()   { return items_.end(); }
    const_iterator begin() const { return items_.begin(); }
    const_iterator end() const   { return items_.end(); }
    const_iterator cbegin() const { return items_.cbegin(); }
    const_iterator cend() const   { return items_.cend(); }

public: // ---------- 容量 ----------

    /// @brief 检查映射是否为空
    bool empty() const noexcept {
        return items_.empty();
    }

    /// @brief 获取映射中键值对的数量
    std::size_t size() const noexcept {
        return items_.size();
    }

public: // ---------- 访问元素 ----------
    
    /// @brief 获取键对应的值
    Value& at(const Key& key) {
        auto it = index_map_.find(key);
        if (it == index_map_.end())
            throw std::out_of_range("OrderedMap::at: key not found");
        return items_[it->second].second;
    }

    /// @brief 获取键对应的值（常量版本）
    const Value& at(const Key& key) const {
        auto it = index_map_.find(key);
        if (it == index_map_.end())
            throw std::out_of_range("OrderedMap::at: key not found");
        return items_[it->second].second;
    }

    Value& operator[](const Key& key) {
        auto it = index_map_.find(key);
        if (it != index_map_.end()) {
            return items_[it->second].second;
        }
        // 不存在则插入默认构造的值
        index_map_[key] = items_.size();
        items_.emplace_back(key, Value{});
        return items_.back().second;
    }

public: // ---------- 插入/更新 ----------

    /// @brief 插入或更新键值对
    void insert(const Key& key, const Value& value) {
        auto it = index_map_.find(key);
        if (it != index_map_.end()) {
            // 已存在：更新值
            items_[it->second].second = value;
        } else {
            // 不存在：追加到末尾
            index_map_[key] = items_.size();
            items_.emplace_back(key, value);
        }
    }

    /// @brief 插入或更新键值对（移动语义）
    void insert(std::pair<Key, Value>&& pair) {
        insert(pair.first, std::move(pair.second));
    }

public: // ---------- 查找 ----------

    /// @brief 查找键对应的迭代器
    iterator find(const Key& key) {
        auto it = index_map_.find(key);
        if (it == index_map_.end())
            return items_.end();
        return items_.begin() + it->second;
    }

    /// @brief 查找键对应的迭代器（常量版本）
    const_iterator find(const Key& key) const {
        auto it = index_map_.find(key);
        if (it == index_map_.end())
            return items_.end();
        return items_.begin() + it->second;
    }

    /// @brief 检查映射是否包含指定键
    bool contains(const Key& key) const {
        return index_map_.find(key) != index_map_.end();
    }

public:
    /// @brief 清空映射
    void clear() {
        items_.clear();
        index_map_.clear();
    }

public: 
    /// @brief 获取所有键
    std::vector<Key> keys() const {
        std::vector<Key> result;
        result.reserve(items_.size());
        for (const auto& pair : items_) {
            result.push_back(pair.first);
        }
        return result;
    }

    /// @brief 获取所有值
    std::vector<Value> values() const {
        std::vector<Value> result;
        result.reserve(items_.size());
        for (const auto& pair : items_) {
            result.push_back(pair.second);
        }
        return result;
    }

    /// @brief 获取所有键值对
    const vector_type& items() const { return items_; }
};

/*! @} */

AST_NAMESPACE_END


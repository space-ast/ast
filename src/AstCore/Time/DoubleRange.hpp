///
/// @file      DoubleRange.hpp
/// @brief     按步长采样的相对秒范围
/// @details   对 [start, stop] 按 step 步长采样的惰性可迭代 double 范围。
/// @author    axel
/// @date      2026-08-24
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

#pragma once

#include "AstGlobal.h"
#include <iterator>   // for std::input_iterator_tag
#include <cstddef>    // for size_t / ptrdiff_t

AST_NAMESPACE_BEGIN


/*!
    @addtogroup Time
    @{
*/

/// @brief 按步长采样的相对秒范围（惰性可迭代）
/// @details 对 [start, stop] 闭区间按 step 步长采样：内部相邻点间距恒为 step，
///          但末尾强制把 stop 并入输出（最后一段间距可能小于 step），因此并非均匀网格。
/// @note 通常经 TimeInterval::discretize(epoch, step) 构造，值表示相对 epoch 的秒偏移。
class DoubleRange
{
public:
    /// @brief 构造相对秒采样范围
    /// @param start 起始秒偏移
    /// @param stop 结束秒偏移（强制并入）
    /// @param step 步长（秒）
    /// @param n 采样点数
    DoubleRange(double start, double stop, double step, size_t n)
        : start_(start), stop_(stop), step_(step), n_(n) {}

    class iterator
    {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type        = double;
        using difference_type   = ptrdiff_t;
        using pointer           = const double*;
        using reference         = double;     // input iterator：按值返回

        iterator() = default;
        iterator(const DoubleRange* range, size_t idx)
            : range_(range), idx_(idx) {}

        /// @brief 解引用：除末点按 step 步进，末点返回 stop_
        reference operator*() const
        {
            return (idx_ == range_->n_ - 1)
                       ? range_->stop_
                       : range_->start_ + range_->step_ * idx_;
        }

        iterator& operator++() { ++idx_; return *this; }
        iterator operator++(int) { auto tmp = *this; ++*this; return tmp; }

        bool operator==(const iterator& other) const { return idx_ == other.idx_; }
        bool operator!=(const iterator& other) const { return !(*this == other); }

    private:
        const DoubleRange* range_ = nullptr;
        size_t idx_ = 0;
    };

    iterator begin() const { return iterator(this, 0); }
    iterator end()   const { return iterator(this, n_); }
    size_t size() const { return n_; }

private:
    double start_;      ///< 起始秒偏移
    double stop_;       ///< 结束秒偏移（强制并入）
    double step_;       ///< 步长（秒）
    size_t n_;          ///< 采样点数
};


/*! @} */


AST_NAMESPACE_END

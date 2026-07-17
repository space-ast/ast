///
/// @file      TimeList.hpp
/// @brief     时间点列表
/// @details   ~
/// @author    axel
/// @date      2026-07-03
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
#include "TimePoint.hpp"
#include <iterator>
#include <vector>

AST_NAMESPACE_BEGIN


/*!
    @addtogroup Time
    @{
*/


/// @brief 时间点列表
/// @details 时间点列表以相对秒数 + 参考历元的形式存储时间序列。
///
/// 存储模型：
/// - seconds_ : std::vector<double> — 相对于 epoch_ 的秒偏移量
/// - epoch_   : TimePoint           — 参考历元
///
/// 不假设列表有序。使用方式与 std::vector<double> 一致，
/// 同时提供时间语义方法将偏移量转换为绝对 TimePoint。
///
/// @note 列表可以无序。如需二分查找等操作，请先确保排序。
class TimeList
{
public:

	// ————————————————————————
	// 工厂方法
	// ————————————————————————

	/// @brief 从 TimeInterval 离散化构造
	/// @param interval 时间区间
	/// @param epoch 参考历元
	/// @param step 离散化步长（秒）
	/// @return TimeList 离散化后的时间列表
	AST_CORE_API
	static TimeList FromInterval(const TimeInterval& interval, const TimePoint& epoch, double step);

	/// @brief 从 TimeInterval 离散化构造
	/// @param interval 时间区间
	/// @param step 离散化步长（秒）
	/// @return TimeList 离散化后的时间列表
	AST_CORE_API
	static TimeList FromInterval(const TimeInterval& interval, double step);

public:

	/// @brief 默认构造函数
	TimeList() = default;

	/// @brief 析构函数
	~TimeList() = default;

	/// @brief 拷贝构造
	TimeList(const TimeList&) = default;

	/// @brief 拷贝赋值
	TimeList& operator=(const TimeList&) = default;

	/// @brief 移动构造
	TimeList(TimeList&&) noexcept = default;

	/// @brief 移动赋值
	TimeList& operator=(TimeList&&) noexcept = default;

	/// @brief 从参考历元和秒偏移量列表构造
	/// @param epoch 参考历元
	/// @param secs 秒偏移量列表
	TimeList(const TimePoint& epoch, const std::vector<double>& secs)
		: seconds_(secs), epoch_(epoch) {}

	/// @brief 从参考历元和秒偏移量列表移动构造
	/// @param epoch 参考历元
	/// @param secs 秒偏移量列表（移动）
	TimeList(const TimePoint& epoch, std::vector<double>&& secs)
		: seconds_(std::move(secs)), epoch_(epoch) {}

	/// @brief 从参考历元构造空列表
	/// @param epoch 参考历元
	TimeList(const TimePoint& epoch)
		: epoch_(epoch)
	{
	}
	

	// ————————————————————————
	// 容量
	// ————————————————————————

	size_t size()  const noexcept { return seconds_.size(); }
	bool   empty() const noexcept { return seconds_.empty(); }

	void resize(size_t n)          { seconds_.resize(n); }
	void reserve(size_t n)         { seconds_.reserve(n); }
	void clear()                   { seconds_.clear(); }


	// ————————————————————————
	// 修改
	// ————————————————————————

	void push_back(const TimePoint& tp)        { seconds_.push_back(tp - epoch_); }
	void pop_back()                            { seconds_.pop_back(); }

	// ————————————————————————
	// 参考历元
	// ————————————————————————

	const TimePoint& epoch() const { return epoch_; }

	/// @brief 设置参考历元
	/// @warning 仅当列表为空时调用才是安全的。若列表非空，
	///          已有的秒偏移量仍相对于旧历元，将导致 timePoint() 返回错误结果。
	///          如需更改非空列表的历元，请同时手动调整 seconds_ 中的偏移量。
	void setEpoch(const TimePoint& epoch) { epoch_ = epoch; }

	// ————————————————————————
	// 底层向量访问
	// ————————————————————————

	std::vector<double>&       seconds()       noexcept { return seconds_; }
	const std::vector<double>& seconds() const noexcept { return seconds_; }

	// ————————————————————————
	// 时间语义
	// ————————————————————————

	/// @brief 获取第 i 个元素对应的绝对时间点
	/// @param i 索引
	/// @return epoch_ + seconds_[i]
	TimePoint timePoint(size_t i) const
	{
		return epoch_.shiftedBySecond(seconds_[i]);
	}

	/// @brief 访问第 i 个时间点（无边界检查）
	/// @param i 索引
	/// @return epoch_ + seconds_[i]
	TimePoint operator[](size_t i) const { return epoch_.shiftedBySecond(seconds_[i]); }

	/// @brief 访问第 i 个时间点（边界检查）
	/// @param i 索引
	/// @return epoch_ + seconds_[i]
	/// @throws std::out_of_range 若 i >= size()
	TimePoint at(size_t i) const { return epoch_.shiftedBySecond(seconds_.at(i)); }

	// ————————————————————————
	// 迭代器
	// ————————————————————————

	/// @brief 随机访问常量迭代器
	/// @details 内部持有秒偏移量指针和历元指针，
	///          解引用时计算 epoch_->shiftedBySecond(*data_) 并返回 TimePoint。
	class const_iterator
	{
	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type        = TimePoint;
		using difference_type   = std::ptrdiff_t;
		using pointer           = const TimePoint*;
		using reference         = TimePoint;  // 按值返回

		const_iterator() = default;

		const_iterator(const double* data, const TimePoint* epoch)
			: data_(data), epoch_(epoch) {}

		/// @brief 解引用：计算 epoch_->shiftedBySecond(*data_)
		TimePoint operator*() const { return epoch_->shiftedBySecond(*data_); }

		/// @brief 下标访问
		TimePoint operator[](difference_type n) const { return epoch_->shiftedBySecond(data_[n]); }

		const_iterator& operator++()                           { ++data_; return *this; }
		const_iterator  operator++(int)                        { auto tmp = *this; ++data_; return tmp; }
		const_iterator& operator--()                           { --data_; return *this; }
		const_iterator  operator--(int)                        { auto tmp = *this; --data_; return tmp; }
		const_iterator& operator+=(difference_type n)          { data_ += n; return *this; }
		const_iterator& operator-=(difference_type n)          { data_ -= n; return *this; }

		const_iterator  operator+(difference_type n)  const     { return {data_ + n, epoch_}; }
		const_iterator  operator-(difference_type n)  const     { return {data_ - n, epoch_}; }
		difference_type operator-(const const_iterator& o) const { return data_ - o.data_; }

		bool operator==(const const_iterator& o) const { return data_ == o.data_; }
		bool operator!=(const const_iterator& o) const { return data_ != o.data_; }
		bool operator< (const const_iterator& o) const { return data_ <  o.data_; }
		bool operator> (const const_iterator& o) const { return data_ >  o.data_; }
		bool operator<=(const const_iterator& o) const { return data_ <= o.data_; }
		bool operator>=(const const_iterator& o) const { return data_ >= o.data_; }

	private:
		const double*    data_  = nullptr;
		const TimePoint* epoch_ = nullptr;
	};

	/// @brief difference_type + const_iterator（非成员运算符）
	friend const_iterator operator+(const_iterator::difference_type n, const const_iterator& it)
	{
		return it + n;
	}

	using iterator               = const_iterator;
	using value_type             = TimePoint;
	using size_type              = size_t;
	using difference_type        = std::ptrdiff_t;

	iterator       begin()       { return {seconds_.data(), &epoch_}; }
	iterator       end()         { return {seconds_.data() + seconds_.size(), &epoch_}; }
	const_iterator begin() const { return {seconds_.data(), &epoch_}; }
	const_iterator end()   const { return {seconds_.data() + seconds_.size(), &epoch_}; }
	const_iterator cbegin()const { return begin(); }
	const_iterator cend()  const { return end(); }

private:
	std::vector<double> seconds_{};   ///< 相对于 epoch_ 的秒偏移量
	TimePoint           epoch_{};   ///< 参考历元
};


/*! @} */


AST_NAMESPACE_END

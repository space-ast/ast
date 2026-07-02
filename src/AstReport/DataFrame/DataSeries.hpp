///
/// @file      DataSeries.hpp
/// @brief     数据列 — 类型擦除的一维数组及其类型化子类
/// @details   DataSeries 为类型擦除基类，TypedDataSeries<T> 继承自它，
///            提供编译期类型安全的零开销视图。两者内存布局相同（子类不增加数据成员）。
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

#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <stdexcept>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup AstReport
    @{
*/

// ---- 前置声明 ----

template<typename T> class TypedDataSeries;

// ---- 类型映射 ----

template<typename T> struct DataTypeTraits;

template<> struct DataTypeTraits<double>
    { static constexpr EDataType type = EDataType::eDouble; };

template<> struct DataTypeTraits<int>
    { static constexpr EDataType type = EDataType::eInt; };

template<> struct DataTypeTraits<std::string>
    { static constexpr EDataType type = EDataType::eString; };

class TimePoint;

template<> struct DataTypeTraits<TimePoint>
    { static constexpr EDataType type = EDataType::eTimePoint; };


// ======================
// DataSeries — 类型擦除基类
// ======================

class AST_REPORT_API DataSeries
{
public:
    // ---- 构造/析构 ----

    DataSeries();
    DataSeries(const std::string& name, EDataType type, size_t size);

    /// @brief 从 std::vector 批量构造（自动推导类型）
    template<typename T>
    DataSeries(const std::string& name, const std::vector<T>& data);

    ~DataSeries();

    // ---- 拷贝 ----

    DataSeries(const DataSeries& other);
    DataSeries& operator=(const DataSeries& other);

    // ---- 移动 ----

    DataSeries(DataSeries&& other) noexcept;
    DataSeries& operator=(DataSeries&& other) noexcept;

    // ---- 基本信息 ----

    const std::string& name() const { return name_; }
    void               setName(const std::string& name) { name_ = name; }

    size_t     size()  const { return size_; }
    EDataType  type()  const { return type_; }
    bool       empty() const { return size_ == 0; }

    // ---- 元素访问（类型擦除，switch 分发）----

    double             asDouble(size_t i)    const;
    int                asInt(size_t i)       const;
    const std::string& asString(size_t i)    const;
    const TimePoint&   asTimePoint(size_t i) const;

    // ---- 设值 ----

    void set(size_t i, double v);
    void set(size_t i, int v);
    void set(size_t i, const std::string& v);
    void set(size_t i, const TimePoint& v);

    // ---- 统计方法（返回 double，仅数值类型有效）----

    double mean()   const;
    double min()    const;
    double max()    const;
    double sum()    const;
    double stddev() const;

    // ---- 修改 ----

    void resize(size_t n);
    void sort(bool ascending = true);

    // ---- 类型安全视图 ----

    /// @brief 获取类型安全的 TypedDataSeries<T> 引用
    /// @tparam T 必须与 type() 匹配，否则行为未定义（调用者负责检查）
    template<typename T>
    TypedDataSeries<T>& as()
    {
        return static_cast<TypedDataSeries<T>&>(*this);
    }

    template<typename T>
    const TypedDataSeries<T>& as() const
    {
        return static_cast<const TypedDataSeries<T>&>(*this);
    }

protected:
    friend class DataFrame;
    template<typename U> friend class TypedDataSeries;

    void allocate(size_t size);
    void allocateRaw(size_t size);  ///< 仅分配不初始化（POD 跳过 memset）
    void deallocate();

    void*       data()       { return data_; }
    const void* data() const { return data_; }

    std::string name_;
    void*       data_{nullptr};
    EDataType   type_{EDataType::eDouble};
    size_t      size_{0};
};


// ================================
// TypedDataSeries<T> — 类型化子类
// ================================

/// @brief DataSeries 的类型化子类
/// @tparam T 元素类型（double, int, std::string, TimePoint）
///
/// 继承自 DataSeries，不添加任何数据成员，仅提供类型安全的成员函数。
/// 可通过 DataSeries::as<T>() 或 static_cast 从基类引用转换。
/// 所有方法均为 inline，零运行时开销。
template<typename T>
class TypedDataSeries : public DataSeries
{
public:
    using DataSeries::DataSeries;  // 继承基类构造函数

    TypedDataSeries() = default;

    /// @brief 从 std::vector 构造（自动设置类型）
    TypedDataSeries(const std::string& name, const std::vector<T>& data);

    // ---- 元素访问（无越界检查，极致性能）----

    const T& operator[](size_t i) const { return static_cast<const T*>(data())[i]; }
    T&       operator[](size_t i)       { return static_cast<T*>(data())[i]; }

    /// @brief 带越界检查的元素访问（抛出 std::out_of_range）
    const T& at(size_t i) const;
    T&       at(size_t i);

    // ---- 迭代器 ----

    T* begin()       { return static_cast<T*>(data()); }
    T* end()         { return begin() + size(); }
    const T* begin() const { return static_cast<const T*>(data()); }
    const T* end()   const { return begin() + size(); }

    // ---- 统计方法 ----

    T mean()   const;
    T min()    const;
    T max()    const;
    T sum()    const;
    T stddev() const;

    // ---- 修改 ----

    void sort(bool ascending = true);
};


// ===================================
// DataSeries 模板构造函数（内联）
// ===================================

template<typename T>
inline DataSeries::DataSeries(const std::string& name, const std::vector<T>& data)
    : name_(name)
    , type_(DataTypeTraits<T>::type)
{
    allocateRaw(data.size());  // 跳过 POD 零填充，调用者直接覆写
    T* dst = static_cast<T*>(data_);
    for (size_t i = 0; i < size_; ++i)
        dst[i] = data[i];
}


// =========================================
// TypedDataSeries<T> 成员函数实现（内联）
// =========================================

template<typename T>
inline const T& TypedDataSeries<T>::at(size_t i) const
{
    if (i >= size())
        throw std::out_of_range("TypedDataSeries::at: index out of range");
    return static_cast<const T*>(data())[i];
}

template<typename T>
inline T& TypedDataSeries<T>::at(size_t i)
{
    if (i >= size())
        throw std::out_of_range("TypedDataSeries::at: index out of range");
    return static_cast<T*>(data())[i];
}

template<typename T>
inline TypedDataSeries<T>::TypedDataSeries(const std::string& name, const std::vector<T>& data)
{
    name_ = name;
    type_ = DataTypeTraits<T>::type;
    allocateRaw(data.size());
    T* dst = static_cast<T*>(data_);
    for (size_t i = 0; i < size_; ++i)
        dst[i] = data[i];
}

template<typename T>
inline T TypedDataSeries<T>::mean() const
{
    if (size() == 0) return T{};
    const T* p = static_cast<const T*>(data());
    double total = 0;
    for (size_t i = 0; i < size(); ++i)
        total += static_cast<double>(p[i]);
    return static_cast<T>(total / static_cast<double>(size()));
}

template<typename T>
inline T TypedDataSeries<T>::min() const
{
    if (size() == 0) return T{};
    const T* p = static_cast<const T*>(data());
    T val = p[0];
    for (size_t i = 1; i < size(); ++i)
        if (p[i] < val) val = p[i];
    return val;
}

template<typename T>
inline T TypedDataSeries<T>::max() const
{
    if (size() == 0) return T{};
    const T* p = static_cast<const T*>(data());
    T val = p[0];
    for (size_t i = 1; i < size(); ++i)
        if (val < p[i]) val = p[i];
    return val;
}

template<typename T>
inline T TypedDataSeries<T>::sum() const
{
    if (size() == 0) return T{};
    const T* p = static_cast<const T*>(data());
    double total = 0;
    for (size_t i = 0; i < size(); ++i)
        total += static_cast<double>(p[i]);
    return static_cast<T>(total);
}

template<typename T>
inline T TypedDataSeries<T>::stddev() const
{
    if (size() == 0) return T{};
    T m = mean();
    const T* p = static_cast<const T*>(data());
    double total = 0;
    for (size_t i = 0; i < size(); ++i)
    {
        double diff = static_cast<double>(p[i]) - static_cast<double>(m);
        total += diff * diff;
    }
    return static_cast<T>(std::sqrt(total / static_cast<double>(size())));
}

template<typename T>
inline void TypedDataSeries<T>::sort(bool ascending)
{
    T* p = static_cast<T*>(data());
    if (ascending)
        std::sort(p, p + size(), std::less<T>());
    else
        std::sort(p, p + size(), std::greater<T>());
}

/*! @} */

AST_NAMESPACE_END

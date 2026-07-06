///
/// @file      DataSeries.hpp
/// @brief     数据列 — 类型擦除的一维数组
/// @details   DataSeries 为类型擦除数据列，底层使用 VariantVector 存储，
///            通过 VariantVector 的模板方法提供编译期类型安全的访问。
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
#include "AstUtil/VariantVector.hpp"

#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup AstReport
    @{
*/

// ---- 前置声明 ----

class TimePoint;


/// @brief 数据列 — 类型擦除的一维数组
class AST_REPORT_API DataSeries
{
public:
    // ---- 构造/析构 ----

    DataSeries() = default;
    DataSeries(const std::string& name, EDataType type, size_t size);

    /// @brief 从 std::vector 批量构造（自动推导类型）
    template<typename T>
    DataSeries(const std::string& name, const std::vector<T>& data);

    ~DataSeries() = default;

    // ---- 拷贝 ----

    DataSeries(const DataSeries& other) = default;
    DataSeries& operator=(const DataSeries& other) = default;

    // ---- 移动 ----

    DataSeries(DataSeries&& other) noexcept = default;
    DataSeries& operator=(DataSeries&& other) noexcept = default;

    // ---- 基本信息 ----

    const std::string& name() const { return name_; }
    void               setName(const std::string& name) { name_ = name; }

    size_t                size()  const { return data_.size(); }
    const std::type_info& elementType()  const { return data_.elementType(); }
    bool                  empty() const { return data_.empty(); }

    // ---- 统计方法（返回 double，仅数值类型有效）----

    double mean()   const;
    double min()    const;
    double max()    const;
    double sum()    const;
    double stddev() const;

    // ---- 类型安全访问 ----

    /// @brief 获取类型化指针（类型不匹配返回 nullptr）
    template<typename T>
    T* as() { return data_.as<T>(); }

    template<typename T>
    const T* as() const { return data_.as<T>(); }

    /// @brief 获取类型化 Span 视图（类型不匹配返回空 Span）
    template<typename T>
    Span<T> asSpan() { return data_.asSpan<T>(); }

    template<typename T>
    Span<const T> asSpan() const { return data_.asSpan<T>(); }

    // ---- 修改 ----

    void resize(size_t n);
    void sort(bool ascending = true);

    // ---- 数据访问 ----

    const VariantVector& data() const { return data_; }
    VariantVector& data() { return data_; }
protected:
    friend class DataFrame;

    std::string name_;          ///< 数据名称
    VariantVector data_;        ///< 数据向量
};


// =========================================
// DataSeries 模板构造函数（内联）
// =========================================

template<typename T>
inline DataSeries::DataSeries(const std::string& name, const std::vector<T>& data)
    : name_(name)
    , data_(VariantVector::fromVector(data))
{
}

/*! @} */

AST_NAMESPACE_END

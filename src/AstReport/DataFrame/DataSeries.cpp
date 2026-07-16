///
/// @file      DataSeries.cpp
/// @brief     数据列实现
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

#include "DataSeries.hpp"
#include "AstCore/TimePoint.hpp"

#include <string>
#include <cmath>
#include <limits>
#include <algorithm>
#include <typeinfo>
#include <functional>   // for std::greater  std::less

AST_NAMESPACE_BEGIN

using std::string;

// ---- 辅助：EDataType → VariantVector 初始化 ----

/// @brief 按 EDataType 初始化 VariantVector 为指定大小
static void _aInitByType(VariantVector& v, EDataType type, size_t size)
{
    switch (type)
    {
    case EDataType::eDouble:
        v.resize<double>(size);
        break;
    case EDataType::eInt:
        v.resize<int>(size);
        break;
    case EDataType::eString:
        v.resize<std::string>(size);
        break;
    case EDataType::eTimePoint:
        v.resize<TimePoint>(size);
        break;
    default:
        break;  // 未知类型：保持 VariantVector 默认状态，调用方自行检查
    }
}

/// @brief 按 VariantVector 的元素类型执行 resize，默认使用 double 类型填充
static void _aResizeByElementType(VariantVector& v, size_t n)
{
    const auto& ti = v.elementType();

    if (ti == typeid(double))
        v.resize<double>(n);
    else if (ti == typeid(int))
        v.resize<int>(n);
    else if (ti == typeid(std::string))
        v.resize<std::string>(n);
    else if (ti == typeid(TimePoint))
        v.resize<TimePoint>(n);
    else
        v.resize<double>(n);  // 空/无类型 Vector：默认按 double 初始化
}

// ======================
// DataSeries 实现
// ======================

// ---- 构造 ----

DataSeries::DataSeries(const std::string& name, EDataType type, size_t size)
    : name_(name)
{
    _aInitByType(data_, type, size);
}

// ---- 统计方法 ----

double DataSeries::mean() const
{
    double s = sum();
    if (s != s) return s;  // NaN 检查：非数值类型或空序列
    return s / static_cast<double>(data_.size());
}

double DataSeries::min() const
{
    if (data_.size() == 0 || !data_.hasType())
        return std::numeric_limits<double>::quiet_NaN();
    
    if (const double* p = data_.as<double>())
    {
        double val = p[0];
        for (size_t i = 1; i < data_.size(); ++i)
            if (p[i] < val) val = p[i];
        return val;
    }
    else if (const int* p = data_.as<int>())
    {
        int val = p[0];
        for (size_t i = 1; i < data_.size(); ++i)
            if (p[i] < val) val = p[i];
        return static_cast<double>(val);
    }
    return std::numeric_limits<double>::quiet_NaN();
}

double DataSeries::max() const
{
    if (data_.size() == 0 || !data_.hasType())
        return std::numeric_limits<double>::quiet_NaN();

    if (const double* p = data_.as<double>())
    {
        double val = p[0];
        for (size_t i = 1; i < data_.size(); ++i)
            if (val < p[i]) val = p[i];
        return val;
    }
    else if (const int* p = data_.as<int>())
    {
        int val = p[0];
        for (size_t i = 1; i < data_.size(); ++i)
            if (val < p[i]) val = p[i];
        return static_cast<double>(val);
    }
    return std::numeric_limits<double>::quiet_NaN();
}

double DataSeries::sum() const
{
    if (data_.size() == 0 || !data_.hasType())
        return std::numeric_limits<double>::quiet_NaN();

    if (const double* p = data_.as<double>())
    {
        double total = 0.0;
        for (size_t i = 0; i < data_.size(); ++i)
            total += p[i];
        return total;
    }
    else if (const int* p = data_.as<int>())
    {
        double total = 0.0;
        for (size_t i = 0; i < data_.size(); ++i)
            total += static_cast<double>(p[i]);
        return total;
    }

    return std::numeric_limits<double>::quiet_NaN();
}

double DataSeries::stddev() const
{
    if (data_.size() == 0 || !data_.hasType())
        return std::numeric_limits<double>::quiet_NaN();

    double m = mean();
    double total = 0.0;

    // double
    if(const double* p = data_.as<double>())
    {
        for (size_t i = 0; i < data_.size(); ++i)
        {
            double diff = p[i] - m;
            total += diff * diff;
        }
    }
    // int
    else if(const int* p = data_.as<int>())
    {
        for (size_t i = 0; i < data_.size(); ++i)
        {
            double diff = static_cast<double>(p[i]) - m;
            total += diff * diff;
        }
    }
    else
    {
        // 其他类型，返回 NaN
        return std::numeric_limits<double>::quiet_NaN();
    }
    return std::sqrt(total / static_cast<double>(data_.size()));
}

// ---- 修改 ----

void DataSeries::resize(size_t n)
{
    _aResizeByElementType(data_, n);
}

void DataSeries::sort(bool ascending)
{
    if (data_.size() <= 1 || !data_.hasType()) return;

    if (double* p = data_.as<double>())
    {
        if (ascending)
            std::sort(p, p + data_.size(), std::less<double>());
        else
            std::sort(p, p + data_.size(), std::greater<double>());
    }
    else if(int* p = data_.as<int>())
    {
        if (ascending)
            std::sort(p, p + data_.size(), std::less<int>());
        else
            std::sort(p, p + data_.size(), std::greater<int>());
    }
    else if(TimePoint* p = data_.as<TimePoint>())
    {
        if (ascending)
            std::sort(p, p + data_.size(), std::less<TimePoint>());
        else
            std::sort(p, p + data_.size(), std::greater<TimePoint>());
    }
    else if(std::string* base = data_.as<std::string>())
    {
        if (ascending)
            std::sort(base, base + data_.size(), std::less<std::string>());
        else
            std::sort(base, base + data_.size(), std::greater<std::string>());
    }
}

AST_NAMESPACE_END

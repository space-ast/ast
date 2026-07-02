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
#include "AstCore/Time/TimePoint.hpp"

#include <stdlib.h>     // malloc, free
#include <string.h>     // memcpy
#include <string>
#include <cmath>        // isnan, sqrt
#include <limits>       // numeric_limits

AST_NAMESPACE_BEGIN

using std::string;

// ---- 辅助 ----

/// @brief 获取 EDataType 对应的字节大小
static size_t _aDataTypeSize(EDataType type)
{
    switch (type)
    {
    case EDataType::eDouble:
        return sizeof(double);
    case EDataType::eInt:
        return sizeof(int);
    case EDataType::eString:
        return sizeof(std::string);
    case EDataType::eTimePoint:
        return sizeof(TimePoint);
    default:
        return 0;
    }
}

/// @brief 判断类型是否为 POD（可用 memcpy/memset，无需 placement new/dtor）
///        double / int / TimePoint 都是 POD，只有 std::string 不是
static bool _aIsPod(EDataType type)
{
    return type != EDataType::eString;
}

/// @brief 判断类型是否为数值类型（可用于统计）
static bool _aIsNumeric(EDataType type)
{
    return type == EDataType::eDouble || type == EDataType::eInt;
}

// ======================
// DataSeries 实现
// ======================

// ---- 构造/析构 ----

DataSeries::DataSeries() = default;

DataSeries::DataSeries(const std::string& name, EDataType type, size_t size)
    : name_(name)
    , type_(type)
{
    allocate(size);
}

DataSeries::~DataSeries()
{
    deallocate();
}

// ---- 拷贝 ----

DataSeries::DataSeries(const DataSeries& other)
    : name_(other.name_)
    , type_(other.type_)
{
    allocate(other.size_);

    if (size_ == 0) return;

    if (_aIsPod(type_))
    {
        memcpy(data_, other.data_, size_ * _aDataTypeSize(type_));
    }
    else // EDataType::eString
    {
        std::string* dst = static_cast<std::string*>(data_);
        const std::string* src = static_cast<const std::string*>(other.data_);
        for (size_t i = 0; i < size_; ++i)
            dst[i] = src[i];
    }
}

DataSeries& DataSeries::operator=(const DataSeries& other)
{
    if (this == &other) return *this;

    // 先分配新内存，成功后再释放旧内存（强异常安全）
    void*  newData = nullptr;
    size_t elemSize = _aDataTypeSize(other.type_);

    if (other.size_ > 0)
    {
        newData = malloc(other.size_ * elemSize);
        if (!newData) return *this;  // 分配失败，保持旧状态

        if (_aIsPod(other.type_))
        {
            memcpy(newData, other.data_, other.size_ * elemSize);
        }
        else // EDataType::eString
        {
            std::string* dst = static_cast<std::string*>(newData);
            const std::string* src = static_cast<const std::string*>(other.data_);
            for (size_t i = 0; i < other.size_; ++i)
                new (dst + i) std::string(src[i]);
        }
    }

    // 新数据就绪，释放旧资源
    deallocate();
    name_     = other.name_;
    type_     = other.type_;
    data_ = newData;
    size_ = other.size_;

    return *this;
}

// ---- 移动 ----

DataSeries::DataSeries(DataSeries&& other) noexcept
    : name_(std::move(other.name_))
    , data_(other.data_)
    , type_(other.type_)
    , size_(other.size_)
{
    other.data_ = nullptr;
    other.size_ = 0;
}

DataSeries& DataSeries::operator=(DataSeries&& other) noexcept
{
    if (this != &other)
    {
        deallocate();
        name_ = std::move(other.name_);
        data_ = other.data_;
        type_ = other.type_;
        size_ = other.size_;

        other.data_ = nullptr;
        other.size_ = 0;
    }
    return *this;
}

// ---- 元素访问（无越界检查，调用者确保 i < size()）----

double DataSeries::asDouble(size_t i) const
{
    if (i >= size_) return std::numeric_limits<double>::quiet_NaN();
    switch (type_)
    {
    case EDataType::eDouble:
        return static_cast<const double*>(data_)[i];
    case EDataType::eInt:
        return static_cast<double>(static_cast<const int*>(data_)[i]);
    default:
        return std::numeric_limits<double>::quiet_NaN();
    }
}

int DataSeries::asInt(size_t i) const
{
    if (i >= size_) return 0;
    switch (type_)
    {
    case EDataType::eInt:
        return static_cast<const int*>(data_)[i];
    case EDataType::eDouble:
        return static_cast<int>(static_cast<const double*>(data_)[i]);
    default:
        return 0;
    }
}

const std::string& DataSeries::asString(size_t i) const
{
    static const std::string kEmpty;
    if (i >= size_ || type_ != EDataType::eString)
        return kEmpty;
    return static_cast<const std::string*>(data_)[i];
}

const TimePoint& DataSeries::asTimePoint(size_t i) const
{
    static TimePoint kDefaultTimePoint;
    if (i >= size_ || type_ != EDataType::eTimePoint)
        return kDefaultTimePoint;
    return static_cast<const TimePoint*>(data_)[i];
}

// ---- 设值 ----

void DataSeries::set(size_t i, double v)
{
    if (i >= size_) return;
    switch (type_)
    {
    case EDataType::eDouble:
        static_cast<double*>(data_)[i] = v;
        break;
    case EDataType::eInt:
        static_cast<int*>(data_)[i] = static_cast<int>(v);
        break;
    default:
        break;
    }
}

void DataSeries::set(size_t i, int v)
{
    if (i >= size_) return;
    switch (type_)
    {
    case EDataType::eInt:
        static_cast<int*>(data_)[i] = v;
        break;
    case EDataType::eDouble:
        static_cast<double*>(data_)[i] = static_cast<double>(v);
        break;
    default:
        break;
    }
}

void DataSeries::set(size_t i, const std::string& v)
{
    if (i >= size_ || type_ != EDataType::eString) return;
    static_cast<std::string*>(data_)[i] = v;
}

void DataSeries::set(size_t i, const TimePoint& v)
{
    if (i >= size_ || type_ != EDataType::eTimePoint) return;
    static_cast<TimePoint*>(data_)[i] = v;
}

// ---- 统计方法 ----

double DataSeries::mean() const
{
    double s = sum();
    if (s != s) return s;  // NaN 检查：非数值类型或空序列
    return s / static_cast<double>(size_);
}

double DataSeries::min() const
{
    if (size_ == 0)
        return std::numeric_limits<double>::quiet_NaN();

    if (type_ == EDataType::eDouble)
    {
        double val = static_cast<const double*>(data_)[0];
        const double* p = static_cast<const double*>(data_);
        for (size_t i = 1; i < size_; ++i)
            if (p[i] < val) val = p[i];
        return val;
    }
    else if (type_ == EDataType::eInt)
    {
        int val = static_cast<const int*>(data_)[0];
        const int* p = static_cast<const int*>(data_);
        for (size_t i = 1; i < size_; ++i)
            if (p[i] < val) val = p[i];
        return static_cast<double>(val);
    }
    else if (type_ == EDataType::eTimePoint)
    {
        // TimePoint 无法转为 double，使用 as<TimePoint>().min() 获取类型化结果
        return std::numeric_limits<double>::quiet_NaN();
    }

    return std::numeric_limits<double>::quiet_NaN();
}

double DataSeries::max() const
{
    if (size_ == 0)
        return std::numeric_limits<double>::quiet_NaN();

    if (type_ == EDataType::eDouble)
    {
        double val = static_cast<const double*>(data_)[0];
        const double* p = static_cast<const double*>(data_);
        for (size_t i = 1; i < size_; ++i)
            if (val < p[i]) val = p[i];
        return val;
    }
    else if (type_ == EDataType::eInt)
    {
        int val = static_cast<const int*>(data_)[0];
        const int* p = static_cast<const int*>(data_);
        for (size_t i = 1; i < size_; ++i)
            if (val < p[i]) val = p[i];
        return static_cast<double>(val);
    }
    else if (type_ == EDataType::eTimePoint)
    {
        // TimePoint 无法转为 double，使用 as<TimePoint>().max() 获取类型化结果
        return std::numeric_limits<double>::quiet_NaN();
    }

    return std::numeric_limits<double>::quiet_NaN();
}

double DataSeries::sum() const
{
    if (size_ == 0 || !_aIsNumeric(type_))
        return std::numeric_limits<double>::quiet_NaN();

    double total = 0.0;
    if (type_ == EDataType::eDouble)
    {
        const double* p = static_cast<const double*>(data_);
        for (size_t i = 0; i < size_; ++i) total += p[i];
    }
    else // eInt
    {
        const int* p = static_cast<const int*>(data_);
        for (size_t i = 0; i < size_; ++i) total += static_cast<double>(p[i]);
    }
    return total;
}

double DataSeries::stddev() const
{
    if (size_ == 0 || !_aIsNumeric(type_))
        return std::numeric_limits<double>::quiet_NaN();

    double m = mean();
    double total = 0.0;

    if (type_ == EDataType::eDouble)
    {
        const double* p = static_cast<const double*>(data_);
        for (size_t i = 0; i < size_; ++i)
        {
            double diff = p[i] - m;
            total += diff * diff;
        }
    }
    else
    {
        const int* p = static_cast<const int*>(data_);
        for (size_t i = 0; i < size_; ++i)
        {
            double diff = static_cast<double>(p[i]) - m;
            total += diff * diff;
        }
    }
    return std::sqrt(total / static_cast<double>(size_));
}

// ---- 修改 ----

void DataSeries::resize(size_t n)
{
    if (n == size_) return;

    size_t elemSize = _aDataTypeSize(type_);

    if (n <= size_)
    {
        // 缩小：析构多余元素（仅 std::string 需要）
        if (type_ == EDataType::eString)
        {
            std::string* p = static_cast<std::string*>(data_);
            for (size_t i = n; i < size_; ++i)
                p[i].~string();
        }
        size_ = n;
    }
    else
    {
        // 扩大：重新分配并拷贝旧数据
        void* newData = malloc(n * elemSize);
        if (!newData) return;

        if (data_)
        {
            if (_aIsPod(type_))
            {
                memcpy(newData, data_, size_ * elemSize);
            }
            else // EDataType::eString
            {
                std::string* dst = static_cast<std::string*>(newData);
                std::string* src = static_cast<std::string*>(data_);
                for (size_t i = 0; i < size_; ++i)
                {
                    new (dst + i) std::string(std::move(src[i]));
                    src[i].~string();
                }
            }
            free(data_);
        }
        data_ = newData;

        // 对新增元素做默认初始化
        size_t oldSize = size_;
        size_ = n;

        if (type_ == EDataType::eString)
        {
            std::string* p = static_cast<std::string*>(data_);
            for (size_t i = oldSize; i < n; ++i)
                new (p + i) std::string;
        }
        else
        {
            // POD（double / int / TimePoint）：零填充新增区域
            char* p = static_cast<char*>(data_);
            memset(p + oldSize * elemSize, 0, (n - oldSize) * elemSize);
        }
    }
}

void DataSeries::sort(bool ascending)
{
    if (size_ <= 1) return;

    size_t elemSize = _aDataTypeSize(type_);

    if (type_ == EDataType::eDouble)
    {
        double* p = static_cast<double*>(data_);
        if (ascending)
            std::sort(p, p + size_, std::less<double>());
        else
            std::sort(p, p + size_, std::greater<double>());
    }
    else if (type_ == EDataType::eInt)
    {
        int* p = static_cast<int*>(data_);
        if (ascending)
            std::sort(p, p + size_, std::less<int>());
        else
            std::sort(p, p + size_, std::greater<int>());
    }
    else if (type_ == EDataType::eTimePoint)
    {
        TimePoint* p = static_cast<TimePoint*>(data_);
        if (ascending)
            std::sort(p, p + size_, std::less<TimePoint>());
        else
            std::sort(p, p + size_, std::greater<TimePoint>());
    }
    else
    {
        // std::string：索引排序（O(n log n)，避免冒泡排序的 O(n²) 和 placement-new UB）
        std::string* base = static_cast<std::string*>(data_);

        // 构建索引数组
        std::vector<size_t> idx(size_);
        for (size_t i = 0; i < size_; ++i) idx[i] = i;

        if (ascending)
        {
            std::sort(idx.begin(), idx.end(), [base](size_t a, size_t b) {
                return base[a] < base[b];
            });
        }
        else
        {
            std::sort(idx.begin(), idx.end(), [base](size_t a, size_t b) {
                return base[a] > base[b];
            });
        }

        // 用临时数组重排（避免原地 swap 的复杂性和 placement-new UB）
        std::vector<std::string> tmp(size_);
        for (size_t i = 0; i < size_; ++i)
            tmp[i] = std::move(base[idx[i]]);
        for (size_t i = 0; i < size_; ++i)
            base[i].~string();
        for (size_t i = 0; i < size_; ++i)
            new (base + i) std::string(std::move(tmp[i]));
    }
}

// ---- 内部方法 ----

void DataSeries::allocateRaw(size_t size)
{
    size_ = size;
    if (size == 0) return;

    size_t elemSize = _aDataTypeSize(type_);
    data_ = malloc(size * elemSize);
    if (!data_)
    {
        size_ = 0;
        return;
    }

    if (type_ == EDataType::eString)
    {
        std::string* p = static_cast<std::string*>(data_);
        for (size_t i = 0; i < size; ++i)
            new (p + i) std::string;
    }
}

void DataSeries::allocate(size_t size)
{
    allocateRaw(size);
    if (size > 0 && _aIsPod(type_))
        memset(data_, 0, size * _aDataTypeSize(type_));
}

void DataSeries::deallocate()
{
    if (!data_) return;

    // 仅 std::string 需要逐元素析构，POD 类型直接 free
    if (type_ == EDataType::eString)
    {
        std::string* p = static_cast<std::string*>(data_);
        for (size_t i = 0; i < size_; ++i)
            p[i].~string();
    }

    free(data_);
    data_ = nullptr;
    size_ = 0;
}

AST_NAMESPACE_END

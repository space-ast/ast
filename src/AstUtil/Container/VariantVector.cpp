///
/// @file      VariantVector.cpp
/// @brief     类型擦除向量容器 — 非模板核心方法实现
/// @details
/// @author    axel
/// @date      2026-07-05
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

#include "VariantVector.hpp"

AST_NAMESPACE_BEGIN

// ================================================================
// 类型信息查询
// ================================================================

const std::type_info& VariantVector::elementType() const noexcept
{
    return data_ ? *header()->typeInfo : typeid(void);
}

// ================================================================
// 析构
// ================================================================

VariantVector::~VariantVector()
{
    destroy();
}

void VariantVector::destroy() noexcept
{
    if (data_ == nullptr)
        return;

    const Header* hdr = header();

    // 析构所有元素
    if (size_ > 0)
        hdr->ops->destroy(data_, size_);

    // 析构 Header 并释放内存块
    char* block = static_cast<char*>(data_) - kHeaderSize;
    // Header 是 POD，无需显式析构
    ::operator delete(static_cast<void*>(block));

    data_     = nullptr;
    size_     = 0;
    capacity_ = 0;
}

// ================================================================
// 拷贝
// ================================================================

VariantVector::VariantVector(const VariantVector& other)
{
    copyFrom(other);
}

VariantVector& VariantVector::operator=(const VariantVector& other)
{
    if (this != &other)
    {
        destroy();
        copyFrom(other);
    }
    return *this;
}

void VariantVector::copyFrom(const VariantVector& other)
{
    if (other.data_ == nullptr)
        return;  // other 无类型，保持默认状态

    const Header* otherHeader = other.header();
    size_t elemSize    = otherHeader->elementSize;
    size_t otherSize   = other.size_;
    size_t otherCap    = other.capacity_;

    // 分配新块（检查溢出）
    if (otherCap > (SIZE_MAX - kHeaderSize) / elemSize)
        throw std::length_error("VariantVector::copyFrom: requested size exceeds max_size");

    size_t offset    = kHeaderSize;
    size_t blockSize = offset + otherCap * elemSize;
    char* block = static_cast<char*>(::operator new(blockSize));

    // 拷贝 Header
    ::new (block) Header(*otherHeader);

    // 拷贝元素
    char* newData = block + offset;
    if (otherSize > 0)
    {
        try
        {
            otherHeader->ops->copyConstruct(newData, other.data(), otherSize);
        }
        catch (...)
        {
            ::operator delete(static_cast<void*>(block));
            throw;
        }
    }

    data_     = newData;
    size_     = otherSize;
    capacity_ = otherCap;
}

// ================================================================
// 移动
// ================================================================

VariantVector::VariantVector(VariantVector&& other) noexcept
{
    moveFrom(other);
}

VariantVector& VariantVector::operator=(VariantVector&& other) noexcept
{
    if (this != &other)
    {
        destroy();
        moveFrom(other);
    }
    return *this;
}

void VariantVector::moveFrom(VariantVector& other) noexcept
{
    data_     = other.data_;
    size_     = other.size_;
    capacity_ = other.capacity_;

    other.data_     = nullptr;
    other.size_     = 0;
    other.capacity_ = 0;
}

// ================================================================
// 修改
// ================================================================

void VariantVector::clear()
{
    if (data_ == nullptr)
        return;

    if (size_ > 0)
    {
        header()->ops->destroy(data_, size_);
        size_ = 0;
    }
    // 保留 Header 和 capacity_，类型信息不丢失
}

void VariantVector::shrinkToFit()
{
    if (data_ == nullptr || size_ == capacity_)
        return;

    // 缩容到 size_（size_==0 时同样走此路径：oldHeader->ops->moveConstruct 的 count=0 为无操作）
    const Header* oldHeader = header();
    size_t elemSize    = oldHeader->elementSize;
    size_t offset      = kHeaderSize;
    size_t newBlockSize = offset + size_ * elemSize;
    char* newBlock = static_cast<char*>(::operator new(newBlockSize));

    // 拷贝 Header
    ::new (newBlock) Header(*oldHeader);

    // move 元素到新块
    char* newData = newBlock + offset;
    oldHeader->ops->moveConstruct(newData, data_, size_);

    // 析构旧元素并释放旧块
    char* oldBlock = static_cast<char*>(data_) - offset;
    oldHeader->ops->destroy(data_, size_);
    ::operator delete(static_cast<void*>(oldBlock));

    data_     = newData;
    capacity_ = size_;
}

// ================================================================
// 交换
// ================================================================

void VariantVector::swap(VariantVector& other) noexcept
{
    size_t tmpSize     = size_;
    size_t tmpCapacity = capacity_;
    void*  tmpData     = data_;

    size_     = other.size_;
    capacity_ = other.capacity_;
    data_     = other.data_;

    other.size_     = tmpSize;
    other.capacity_ = tmpCapacity;
    other.data_     = tmpData;
}

AST_NAMESPACE_END

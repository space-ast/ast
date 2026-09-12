///
/// @file      SpiceKernelPool.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-08
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
#include <unordered_map>
#include <vector>
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief  SPICE 数据容器
/// @details 该类用于存储从 SPICE 内核文件加载的不同类型的数据
/// 使得在 SPICE 内核文件中加载不同类型的数据变得更加方便和高效
/// @note
/// std::vector<double>, std::vector<int>, std::vector<char>的内存布局是相同的，都是3个指针
/// 对于这个数据容器类型，统一通过charData_来进行复制、移动、析构
/// 这确保了不同类型的数据容器可以在相同的内存布局下进行存储和操作
/// 通过指针之间的地址差，判断是否可以安全地直接将一个数据容器转换为另一个数据容器
/// 例如将一个char类型的数据容器转换为double类型的数据容器，
/// 只需要判断地址差是否为sizeof(double)的整数倍即可
/// @todo 使用union不太合适，考虑其他的实现方式
union KernelData
{
public:
    KernelData(){
        new (&doubleData_) std::vector<double>();
    }
    KernelData(const std::vector<double>& doubleData)
    {
        new (&doubleData_) std::vector<double>(doubleData);
    }
    KernelData(const std::vector<int>& intData)
    {
        new (&intData_) std::vector<int>(intData);
    }
    KernelData(const std::vector<char>& charData)
    {
        new (&charData_) std::vector<char>(charData);
    }
    KernelData(const KernelData& other)
    {
        new (&charData_) std::vector<char>(other.charData_);
    }
    KernelData(KernelData&& other)
    {
        new (&charData_) std::vector<char>(std::move(other.charData_));
    }
    KernelData& operator=(const KernelData& other)
    {
        charData_ = other.charData_;
        return *this;
    }
    KernelData& operator=(KernelData&& other)
    {
        charData_ = std::move(other.charData_);
        return *this;
    }
    ~KernelData()
    {
        charData_.~vector<char>();
    }
    const std::vector<double>* getDoubleData() const { 
        if(charData_.size() == doubleData_.size() * sizeof(double))
            return &doubleData_; 
        return nullptr;
    }
    const std::vector<int>* getIntData() const {
        if(charData_.size() == intData_.size() * sizeof(int))
            return &intData_; 
        return nullptr;
    }
    const std::vector<char>* getCharData() const { 
        return &charData_; 
    }

    enum DataType
    {
        eDouble = 0,
        eInt = 1,
        eChar = 2,
    };
protected:
    // DataType type_;
    std::vector<double> doubleData_;
    std::vector<int>    intData_;
    std::vector<char>   charData_;
};


/// @brief  SPICE 内核数据池
/// @details 该类用于管理从 SPICE 内核文件加载的数据
class AST_UTIL_API KernelPool
{
public:
    using DataMap = std::unordered_map<std::string, KernelData>;
    KernelPool() = default;
    ~KernelPool() = default;

    /// @brief 设置数据
    /// @param name 数据名称
    /// @param data 数据
    void setData(StringView name, const KernelData& data);

    /// @brief 设置双精度数据
    /// @param name 数据名称
    /// @param data 数据
    void setDoubleData(StringView name, const std::vector<double>& data);

    /// @brief 设置整数数据
    /// @param name 数据名称
    /// @param data 数据
    void setIntData(StringView name, const std::vector<int>& data);
    
    /// @brief 设置字符数据
    /// @param name 数据名称
    /// @param data 数据
    void setCharData(StringView name, const std::vector<char>& data);


    /// @brief 获取数据
    /// @param name 数据名称
    /// @return 指向数据的指针，若数据不存在则返回 nullptr
    const KernelData* getData(StringView name) const;

    /// @brief 获取双精度数据
    /// @param name 数据名称
    /// @return 指向双精度数据的指针，若数据不存在则返回 nullptr
    const std::vector<double>* getDoubleData(StringView name) const;
    
    /// @brief 获取整数数据
    /// @param name 数据名称
    /// @return 指向整数数据的指针，若数据不存在则返回 nullptr
    const std::vector<int>* getIntData(StringView name) const;
    
    /// @brief 获取字符数据
    /// @param name 数据名称
    /// @return 指向字符数据的指针，若数据不存在则返回 nullptr
    const std::vector<char>* getCharData(StringView name) const;

    DataMap::iterator begin() { return dataMap_.begin(); }
    DataMap::iterator end() { return dataMap_.end(); }
    DataMap::const_iterator begin() const { return dataMap_.begin(); }
    DataMap::const_iterator end() const { return dataMap_.end(); }
    size_t size() const { return dataMap_.size(); }
protected:
    DataMap dataMap_{};
};

/*! @} */

AST_NAMESPACE_END

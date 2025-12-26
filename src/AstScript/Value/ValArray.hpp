///
/// @file      ValArray.hpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-26
/// @copyright 版权所有 (C) 2025-present, ast项目.
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
#include "AstScript/Value.hpp"

AST_NAMESPACE_BEGIN


/// @brief 数组值类
/// @tparam T 数组元素类型
/// @tparam NDIM 维度数量
template<typename T, size_t NDIM>
class ValArray: public Value
{
public:
    // AST_EXPR(ValArray)

    /// @brief 获取数组列数
    /// @return size_t 数组列数
    size_t column() const { return dims_[0]; }

    /// @brief 获取数组行数
    /// @return size_t 数组行数
    size_t row() const { return dims_[1]; }

    /// @brief 获取数组页数
    /// @return size_t 数组页数
    size_t page() const { return dims_[2]; }

    /// @brief 获取数组元素数量
    /// @return size_t 数组元素数量
    size_t size() const;

    /// @brief 获取数组元素
    /// @param index 元素索引
    /// @return 数组元素引用
    T at(size_t index) const { return data_[index]; }

    /// @brief 获取数组元素
    /// @param index 元素索引
    /// @return 数组元素引用
    T& at(size_t index) { return data_[index]; }
    
protected:
    size_t dims_[NDIM]; ///< 数组维度
    T*     data_;        ///< 数组数据指针
};


template <typename T, size_t NDIM>
inline size_t ValArray<T, NDIM>::size() const
{
    size_t size = 1;
    for (size_t i = 0; i < NDIM; ++i)
    {
        size *= dims_[i];
    }
    return size;
}


AST_NAMESPACE_END


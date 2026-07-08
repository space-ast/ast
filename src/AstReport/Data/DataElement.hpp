///
/// @file      DataElement.hpp
/// @brief     数据元素 — 从数据向量中提取子数据的可配置单元
/// @details   
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
#include "AstUtil/StringView.hpp"
#include "AstUtil/Dimension.hpp"
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 数据提取函数指针类型
/// @details 函数签名为 void(const VariantVector& data, VariantVector& element)，
///          从输入数据向量 data 中提取子集，写入输出向量 element。
/// @param[in]  data    输入数据向量（源数据）
/// @param[out] element 输出数据向量（提取结果）
typedef errc_t (*ElementExtractFunc)(const VariantVector& data, VariantVector& element);


/// @brief 空提取函数 — 不执行任何操作
AST_REPORT_CAPI errc_t aExtractNoop(const VariantVector& data, VariantVector& element);


/// @brief 数据元素 — 封装数据提取逻辑的可配置单元
/// @details DataElement 持有一个名称和一个提取函数指针，
///          通过 extract() 方法调用注册的提取函数，从 VariantVector 中抽取子数据。
///          典型用法：为报表的每一列或每一行注册对应的提取函数，
///          遍历数据时调用 extract() 逐项填充报表单元格。
class DataElement
{
public:

    /// @brief 默认构造
    DataElement() = default;

    /// @brief 默认析构
    ~DataElement() = default;


    /// @brief 获取数据元素名称
    /// @return 数据元素名称的常量引用
    const char* name() const { return name_; }

    /// @brief 设置数据元素名称
    /// @param name 新的名称字符串
    void setName(const char* name){ name_ = name; }

    /// @brief 获取数据元素的量纲
    /// @return 数据元素的量纲
    Dimension dimension() const { return dimension_; }

    /// @brief 设置数据元素的量纲
    /// @param dimension 新的量纲
    void setDimension(Dimension dimension){ dimension_ = dimension; }

    /// @brief 注册数据提取函数
    /// @param extractFunc 提取函数指针
    void setExtractFunc(ElementExtractFunc extractFunc);

    /// @brief 获取已注册的提取函数
    /// @return 提取函数指针，若未注册则返回 nullptr
    ElementExtractFunc extractFunc() const { return extractFunc_; }


    /// @brief 执行数据提取
    /// @details 调用已注册的提取函数，从输入数据中提取子集到输出向量。
    ///          调用前需确保已通过 setExtractFunc() 注册有效的提取函数。
    /// @param[in]  data    输入数据向量（源数据）
    /// @param[out] element 输出数据向量（提取结果）
    errc_t extract(const VariantVector& data, VariantVector& element) const { return extractFunc_(data, element); }


private:
    const char* name_{""};                              ///< 数据名称
    Dimension dimension_{Dimension::Unit()};            ///< 数据量纲(仅对double类型有效)
    ElementExtractFunc extractFunc_{&aExtractNoop};     ///< 提取函数指针，用于从数据向量中提取子数据
};

/*! @} */

AST_NAMESPACE_END

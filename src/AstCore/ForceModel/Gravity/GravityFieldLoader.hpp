///
/// @file      GravityFieldLoader.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-24
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
#include "AstUtil/StringView.hpp"
#include "AstUtil/BKVParser.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class GravityFieldHead;
class GravityField;

/// @brief 重力场加载器
/// @details 用于加载重力场文件并解析其中的系数或头信息。
/// @warning 注意单个Loader实例不能同时加载多个重力场文件。
class GravityFieldLoader
{
public:
    GravityFieldLoader() = default;
    GravityFieldLoader(int maxLoadDegree, int maxLoadOrder)
        : maxLoadDegree_(maxLoadDegree), maxLoadOrder_(maxLoadOrder)
    {}
    ~GravityFieldLoader() = default;

    AST_CORE_API
    err_t load(StringView filePath, GravityField& gravityField);

    AST_CORE_API
    err_t load(StringView filePath, GravityFieldHead& head);

    /// @brief 获取最大加载阶数
    int getMaxLoadDegree() const { return maxLoadDegree_; }

    /// @brief 获取最大加载次数
    int getMaxLoadOrder() const { return maxLoadOrder_; }

    /// @brief 设置最大加载阶数
    void setMaxLoadDegree(int maxLoadDegree) { maxLoadDegree_ = maxLoadDegree; }

    /// @brief 设置最大加载次数
    void setMaxLoadOrder(int maxLoadOrder) { maxLoadOrder_ = maxLoadOrder; }

public:    
    BKVParser parser_;                      ///< 解析器
    GravityFieldHead* head_{nullptr};       ///< 重力场头
    GravityField* coeff_{nullptr};          ///< 重力场系数
    int maxLoadDegree_ {-1};                ///< 最大加载阶数
    int maxLoadOrder_ {-1};                 ///< 最大加载次数
};



/*! @} */

AST_NAMESPACE_END

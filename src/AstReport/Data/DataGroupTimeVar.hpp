///
/// @file      DataGroupTimeVar.hpp
/// @brief     
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

#pragma once

#include "AstGlobal.h"
#include "DataGroup.hpp"
#include "AstCore/TimeList.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 随时间变化的数据组
class DataGroupTimeVar : public DataGroup
{
public:
    DataGroupTimeVar() = default;
    ~DataGroupTimeVar() = default;

    /// @brief 计算数据组
    /// @param timeList 时间列表
    /// @param result 结果向量
    /// @return errc_t 错误码
    virtual errc_t calculate(const TimeList& timeList, VariantVector& result) const = 0;


};




/*! @} */

AST_NAMESPACE_END

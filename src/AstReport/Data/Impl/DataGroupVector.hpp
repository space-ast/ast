///
/// @file      DataGroupVector.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-07-06
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
#include "AstReport/DataGroupTimeVar.hpp"
#include "AstCore/Vector.hpp"
#include "AstCore/Axes.hpp"
#include "AstUtil/Span.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 向量数据组
class DataGroupVector : public DataGroupTimeVar
{
public:
    struct Data
    {
        Vector3d vector;
        Vector3d velocity;
    };
    static DataElements Elements();
    DataGroupVector() = default;
    ~DataGroupVector() = default;
public:
    errc_t calculate(const TimeList& timeList, VariantVector& result) const override;
    const DataElements& getElements() const override;
public:
    errc_t calculate(const TimeList& timeList, std::vector<Data>& result) const;
    errc_t calculate(const TimeList& timeList, Span<Data> result) const;

private:
    WeakPtr<Axes> axes_;                    ///< 坐标系
};

/*! @} */

AST_NAMESPACE_END

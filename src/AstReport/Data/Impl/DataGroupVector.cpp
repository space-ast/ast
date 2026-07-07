///
/// @file      DataGroupVector.cpp
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

#include "DataGroupVector.hpp"
#include "AstUtil/VariantVector.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

DataElements DataGroupVector::Elements()
{
    return DataElements();
}


errc_t DataGroupVector::calculate(const TimeList &timeList, VariantVector &result) const
{
    size_t size = timeList.size();
    result.reset<Data>(size);
    return calculate(timeList, result.asSpan<Data>());
}

const DataElements &DataGroupVector::getElements() const
{
    static DataElements elements = Elements();
    return elements;
}

errc_t DataGroupVector::calculate(const TimeList &timeList, std::vector<Data> &result) const
{
    size_t size = timeList.size();
    result.resize(size);
    return calculate(timeList, Span<Data>(result));
}

errc_t DataGroupVector::calculate(const TimeList &timeList, Span<Data> result) const
{
    auto vector = this->getVector();
    auto axes = this->getAxes();
    if (vector == nullptr || axes == nullptr)
    {
        aError("vector or axes is null");
        return eErrorNullPtr;
    }
    size_t size = timeList.size();
    if (size != result.size())
    {
        aError("timeList size is not equal to result size");
        return eErrorInvalidParam;
    }

    errc_t rc = 0;
    for (size_t i = 0; i < size; ++i)
    {
        Data& data = result[i];
        errc_t err = vector->getVectorIn(axes, timeList[i], data.vector, data.velocity);
        if(err != eNoError)
            rc = err;
    }

    return rc;
}

AST_NAMESPACE_END


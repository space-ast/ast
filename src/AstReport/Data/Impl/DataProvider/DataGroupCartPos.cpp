///
/// @file      DataGroupCartPos.cpp
/// @brief     笛卡尔位置数据组实现
/// @details   对应 CartPos 服务，输出 ICRF 坐标系下的笛卡尔位置分量
/// @author    axel
/// @date      2026-07-08
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

#include "DataGroupCartPos.hpp"
#include "AstUtil/VariantVector.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

DataElements DataGroupCartPos::Elements()
{
    DataElements elements;
    // 1. Time (独立变量)
    elements.addElement<Data, const TimePoint&, &Data::getTime>("Time");
    // 2. x
    elements.addElement<Data, double, &Data::getX>("x");
    // 3. y
    elements.addElement<Data, double, &Data::getY>("y");
    // 4. z
    elements.addElement<Data, double, &Data::getZ>("z");
    return elements;
}

errc_t DataGroupCartPos::calculate(const TimeList& timeList, VariantVector& result) const
{
    size_t size = timeList.size();
    result.reset<Data>(size);
    return calculate(timeList, result.asSpan<Data>());
}

const DataElements& DataGroupCartPos::getElements() const
{
    static DataElements elements = Elements();
    return elements;
}

errc_t DataGroupCartPos::calculate(const TimeList& timeList, std::vector<Data>& result) const
{
    size_t size = timeList.size();
    result.resize(size);
    return calculate(timeList, Span<Data>(result));
}


errc_t DataGroupCartPos::calculate(const TimeList& timeList, Span<Data> result) const
{
    Frame* frame = this->getFrame();
    Point* point = this->getPoint();
    if(!point || !frame)
    {
        aError("Point or Frame is null");
        return eErrorNullPtr;
    }

    size_t size = result.size();
    if(size != timeList.size())
    {
        aError("result size must be equal to timeList size");
        return eErrorInvalidParam;
    }

    errc_t rc = 0;
    for(size_t i = 0; i < size; i++)
    {
        Data& data = result[i];
        data.time_ = timeList[i];
        errc_t err = point->getPosIn(frame, data.time_, data.pos_);
        if(err != eNoError)
            rc = err;
    }
    return rc;
}


AST_NAMESPACE_END

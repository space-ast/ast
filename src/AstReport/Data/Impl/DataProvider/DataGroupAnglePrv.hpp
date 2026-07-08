///
/// @file      DataGroupAnglePrv.hpp
/// @brief     角度数据组
/// @details   角度及其变化率
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

#pragma once

#include "AstGlobal.h"
#include "AstReport/DataGroupTimeVar.hpp"
#include "AstCore/Angle.hpp"
#include "AstUtil/Span.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/


/// @brief 角度数据组 — 角度及角速率
class DataGroupAnglePrv : public DataGroupTimeVar
{
public:
    struct Data
    {
        TimePoint time_;
        double    angle_;
        double    angleRate_;

        const TimePoint& getTime()      const { return time_; }
        double           getAngle()     const { return angle_; }
        double           getAngleRate() const { return angleRate_; }
    };
    static DataElements Elements();

    DataGroupAnglePrv() = default;
    ~DataGroupAnglePrv() = default;
public:
    errc_t calculate(const TimeList& timeList, VariantVector& result) const override;
    const DataElements& getElements() const override;
public:
    errc_t calculate(const TimeList& timeList, std::vector<Data>& result) const;
    errc_t calculate(const TimeList& timeList, Span<Data> result) const;
public:
    Angle* getAngle() const { return angle_.get(); }
private:
    WeakPtr<Angle> angle_;
};


/*! @} */

AST_NAMESPACE_END

///
/// @file      Quantity.cpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-15
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

#include "Quantity.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/ParseFormat.hpp"

AST_NAMESPACE_BEGIN

void aQuantityReduce(Quantity& q)
{
    double scale;
    aUnitFactorize(q.unit(), scale);
    q.value() *= scale;
}

Quantity aQuantityAdd(const Quantity& q1, const Quantity& q2)
{
    if(q1.unit().dimension() != q2.unit().dimension())
    {
        aError("Quantity dimension mismatch");
    }else{
        double scale1 = q1.unit().getScale();
        double scale2 = q2.unit().getScale();
        if(scale1 < scale2)
        {
            return Quantity(q1.value() + q2.value() * scale2 / scale1, q1.unit());
        }else if(scale1 > scale2){
            return Quantity(q1.value() * scale1 / scale2 + q2.value(), q2.unit());
        }else{
            return Quantity(q1.value() + q2.value(), q1.unit());
        }
    }
    return Quantity::NaN();
}

Quantity aQuantitySub(const Quantity& q1, const Quantity& q2)
{
    return aQuantityAdd(q1, -q2);
}

Quantity aQuantityMul(const Quantity& q1, const Quantity& q2)
{
    Quantity q(q1.value() * q2.value(), q1.unit() * q2.unit());
    aQuantityReduce(q);
    return q;
}

Quantity aQuantityMul(const Quantity& q, const Unit& unit)
{
    Quantity q2(q.value(), q.unit() * unit);
    aQuantityReduce(q2);
    return q2;
}

Quantity aQuantityDiv(const Quantity& q1, const Quantity& q2)
{
    Quantity q(q1.value() / q2.value(), q1.unit() / q2.unit());
    aQuantityReduce(q);
    return q;
}

Quantity aQuantityDiv(const Quantity& q, const Unit& unit)
{
    Quantity q2(q.value(), q.unit() / unit);
    aQuantityReduce(q2);
    return q2;
}



std::string aQuantityToString(const Quantity &q)
{
    return aFormatDouble(q.value()) + " " + q.unit().name();
}

Quantity operator*(double value, const Unit &unit)
{
    return Quantity(value, unit);
}


AST_NAMESPACE_END

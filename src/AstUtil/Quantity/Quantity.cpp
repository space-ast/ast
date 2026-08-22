///
/// @file      Quantity.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
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
    q.magnitude() *= scale;
}

Quantity aQuantityAdd(const Quantity& q1, const Quantity& q2)
{
    if(q1.unit().dimension() != q2.unit().dimension())
    {
        aError("Quantity dimension mismatch");
    }else{
        const Unit& u1 = q1.unit();
        const Unit& u2 = q2.unit();
        // 对数(dB)单位无法线性组合，转 SI 做功率(dBm/dBW/dB)叠加
        if(u1.kind() == EUnitKind::Logarithmic || u2.kind() == EUnitKind::Logarithmic)
        {
            double siSum = u1.toSI(q1.magnitude()) + u2.toSI(q2.magnitude());
            return Quantity(u1.fromSI(siSum), u1);
        }
        double scale1 = u1.getScale();
        double scale2 = u2.getScale();
        // 线性标度(eScale/eAffine)按比例换算后相加；仅取缩放因子、不引入 offset，
        // 避免摄氏/华氏绝对温度相加时偏移量被重复计入
        if(scale1 < scale2)
        {
            return Quantity(q1.magnitude() + q2.magnitude() * scale2 / scale1, u1);
        }else if(scale1 > scale2){
            return Quantity(q1.magnitude() * scale1 / scale2 + q2.magnitude(), u2);
        }else{
            return Quantity(q1.magnitude() + q2.magnitude(), u1);
        }
    }
    return Quantity::NaN();
}

Quantity aQuantitySub(const Quantity& q1, const Quantity& q2)
{
    //@fixme 目前的实现会出现：30dBm - 20dBm ≈ 30.00004dBm(错误)，应为 9.54dBm
    return aQuantityAdd(q1, -q2);
}

Quantity aQuantityMul(const Quantity& q1, const Quantity& q2)
{
    Quantity q(q1.magnitude() * q2.magnitude(), q1.unit() * q2.unit());
    aQuantityReduce(q);
    return q;
}

Quantity aQuantityMul(const Quantity& q, const Unit& unit)
{
    Quantity q2(q.magnitude(), q.unit() * unit);
    aQuantityReduce(q2);
    return q2;
}

Quantity aQuantityDiv(const Quantity& q1, const Quantity& q2)
{
    Quantity q(q1.magnitude() / q2.magnitude(), q1.unit() / q2.unit());
    aQuantityReduce(q);
    return q;
}

Quantity aQuantityDiv(const Quantity& q, const Unit& unit)
{
    Quantity q2(q.magnitude(), q.unit() / unit);
    aQuantityReduce(q2);
    return q2;
}



std::string aQuantityToString(const Quantity &q)
{
    return aFormatDouble(q.magnitude()) + " " + q.unit().name();
}

Quantity operator*(double value, const Unit &unit)
{
    return Quantity(value, unit);
}



AST_NAMESPACE_END

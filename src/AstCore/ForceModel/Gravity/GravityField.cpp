///
/// @file      GravityField.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-05
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

#include "GravityField.hpp"
#include "GravityFieldLoader.hpp"
#include <cmath>



AST_NAMESPACE_BEGIN

void aGravityFieldNormalize(GravityField &gf)
{
    gf.normalize();
}

void aGravityFieldUnnormalize(GravityField &gf)
{
    gf.unnormalize();
}


err_t GravityFieldHead::load(StringView filepath)
{
    GravityFieldLoader loader;
    return loader.load(filepath, *this);
}


/// @brief 计算重力场系数的归一化因子
/// @param n 阶数 degree
/// @param m 次数 order
/// @return 归一化因子
static double gfNormalizeFactor(int n, int m)
{
    /*
    参考：航天器轨道力学理论与方法 附录C 公式C-1-3
    */
    int delta;
    if(m == 0)
        delta = 0;
    else
        delta = 1;

    double factor = 1.0;
    // if(m > 0){
        for(int i = n - m + 1; i <= n + m; i++)
            factor *= i;
    // }
    factor = sqrt(((1 + delta)*(2 * n + 1)) / factor);
    return factor;
}

A_ALWAYS_INLINE void gfNormalize(int n, int m, double& value)
{
    value /= gfNormalizeFactor(n, m);
}

A_ALWAYS_INLINE void gfNormalize(int n, int m, double& value1, double& value2)
{
    double factor = gfNormalizeFactor(n, m);
    value1 /= factor;
    value2 /= factor;
}

A_ALWAYS_INLINE void gfUnnormalize(int n, int m, double& value)
{
    value *= gfNormalizeFactor(n, m);
}

A_ALWAYS_INLINE void gfUnnormalize(int n, int m, double& value1, double& value2)
{
    double factor = gfNormalizeFactor(n, m);
    value1 *= factor;
    value2 *= factor;
}


GravityField::GravityField()
    : GravityFieldHead()
{

}

err_t GravityField::load(StringView model)
{
    GravityFieldLoader loader;
    return loader.load(model, *this);
}


err_t GravityField::load(StringView model, int maxLoadDegree, int maxLoadOrder)
{
    GravityFieldLoader loader(maxLoadDegree, maxLoadOrder);
    return loader.load(model, *this);
}

void GravityField::normalize()
{
    if(normalized_)
        return;
    normalized_ = true;
    for(int n = 2; n <= maxDegree_; n++)
    {
        for(int m = 0; m <= n; m++)
        {
            gfNormalize(n, m, cnm(n, m), snm(n, m));
        }
    }
}

void GravityField::unnormalize() 
{
    if(!normalized_)
        return;
    normalized_ = false;
    for(int n = 0; n <= maxDegree_; n++)
    {
        for(int m = 0; m <= n; m++)
        {
            gfUnnormalize(n, m, cnm(n, m), snm(n, m));
        }
    }
}

GravityField GravityField::normalized() const
{
    GravityField gf_normalized = *this;
    aGravityFieldNormalize(gf_normalized);
    return gf_normalized;
}

GravityField GravityField::unnormalized() const
{
    GravityField gf_unnormalized = *this;
    aGravityFieldUnnormalize(gf_unnormalized);
    return gf_unnormalized;
}


void GravityField::initCoeffMatrices()
{
    int size = std::max(maxDegree_, maxOrder_) + 1;
    sinCoeff_.resize(size);
    cosCoeff_.resize(size);
    sinCoeff_.setZero();
    cosCoeff_.setZero();
}


double GravityField::getSnmNormalized(int n, int m) const
{
    double snm = getSnm(n, m);
    if(!isNormalized())
    {
        gfNormalize(n, m, snm);
    }
    return snm;
}

double GravityField::getCnmNormalized(int n, int m) const
{
    double cnm = getCnm(n, m);
    if(!isNormalized())
    {
        gfNormalize(n, m, cnm);
    }
    return cnm;
}

double GravityField::getSnmUnnormalized(int n, int m) const
{
    double snm = getSnm(n, m);
    if(isNormalized())
    {
        gfUnnormalize(n, m, snm);
    }
    return snm;
}

double GravityField::getCnmUnnormalized(int n, int m) const
{
    double cnm = getCnm(n, m);
    if(isNormalized())
    {
        gfUnnormalize(n, m, cnm);
    }
    return cnm;
}

double GravityField::getJn(int n) const
{
    return -getCnmUnnormalized(n, 0);
}

AST_NAMESPACE_END


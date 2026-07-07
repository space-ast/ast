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
#include "AstUtil/String.hpp"
#include "AstUtil/Logger.hpp"
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


errc_t GravityFieldHead::load(StringView filepath, StringView dirpath)
{
    GravityFieldLoader loader(dirpath);
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

errc_t GravityField::load(StringView model, StringView dirpath)
{
    GravityFieldLoader loader(dirpath);
    return loader.load(model, *this);
}


errc_t GravityField::load(StringView model, int maxLoadDegree, int maxLoadOrder, StringView dirpath)
{
    GravityFieldLoader loader(maxLoadDegree, maxLoadOrder, dirpath);
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

void GravityField::updateVariations(const TimePoint& epoch)
{
    auto& variations = this->secularVariations_;
    double dyr = epoch.daysFrom(variations.referenceEpoch_) / (365.25);     // 时间间隔（年）
    for(auto& variation : variations.variations_)
    {

        int degree = variation.degree_;
        int order = variation.order_;
        bool isValid = this->isValidDegreeOrder(degree, order);
        if(A_UNLIKELY(!isValid)) continue;

        // 根据长期变化率计算当前时刻的系数
        double currentCoefficient = variation.originalCoefficient_ + variation.linearRate_ * dyr;
        if(A_UNLIKELY(variations.normalized_ != this->normalized_))
        {
            if(!this->normalized_)
            {
                gfUnnormalize(degree, order, currentCoefficient);
            }
            else
            {
                gfNormalize(degree, order, currentCoefficient);
            }
        }
        // 更新重力场系数
        if(variation.isSin_)
        {
            snm(degree, order) = currentCoefficient;
        }
        else
        {
            cnm(degree, order) = currentCoefficient;
        }
    }
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

void GravityField::applyPermanentTideC20Correction(double k20)
{
    // 已经是零潮汐模型，无需修正
    if(includesPermTide_)
    {
        aWarning("已经是零潮汐模型，无需修正。");
        return;
    }
    // 阶数至少需要到2
    if(maxDegree_ < 2)
    {
        aWarning("阶数至少需要到2，当前阶数为 %d。", maxDegree_);
        return;
    }

    // 当前实现仅适用于地球 — A₀ 和 H₀ 是基于日月对地球的潮汐势计算得到的。
    // 其他天体（火星、月球等）的永久潮汐修正需要使用该天体对应的潮汐参数，
    // 而非此处硬编码的地球值。
    if(!aEqualsIgnoreCase(centralBody_, "Earth"))
    {
        aWarning("当前仅支持地球, 天体 '%s' 的永久潮汐修正已跳过。",
                 centralBody_.c_str());
        return;
    }

    // 无潮汐 → 零潮汐 C20 修正
    //
    // 参考: IERS Conventions (2010), Technical Note 36 (TN36)
    //       第6章 "Geopotential", 第6.2.2节 "Treatment of the permanent tide" (p.88)
    //       公式(6.13): C̄20zt = C̄20 − C̄20perm
    //       公式(6.14): C̄20perm = A₀·H₀·k20 = (4.4228×10⁻⁸)·(−0.31460)·k20
    //
    // 其中:
    //   A₀ = 4.4228×10⁻⁸ — 2阶带谐归一化因子
    //   H₀ = −0.31460    — 2阶带谐潮汐势均值 (Cartwright-Tayler规范)
    //   k20 = 0.30190    — 名义Love数 (表6.3, p.83, 滞弹性地球)
    //
    // 零潮汐C20更负（永久潮汐隆起使地球扁率增大，J2增大）
    // 以归一化系数计，修正量 ΔC̄20 ≈ −4.201×10⁻⁹ (k20=0.30190)
    // 对EGM2008而言，零潮汐与无潮汐C20之差为 −4.1736×10⁻⁹ (TN36原文)
    constexpr double A0 = 4.4228e-8;
    constexpr double H0 = -0.31460;
    double deltaC20Normalized = A0 * H0 * k20;  // 负值，约 −1.3914×10⁻⁸ × k20

    if(normalized_)
    {
        cosCoeff_(2, 0) += deltaC20Normalized;  // deltaC20Normalized < 0
    }
    else
    {
        // 非归一化状态: C20 = C̄20 × √5
        constexpr double sqrt5 = 2.23606797749979;
        cosCoeff_(2, 0) += deltaC20Normalized * sqrt5;
    }

    includesPermTide_ = true; // 修正后标记为零潮汐模型
}

AST_NAMESPACE_END


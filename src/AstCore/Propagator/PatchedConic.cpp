///
/// @file      PatchedConic.cpp
/// @brief     圆锥曲线拼接(双二体)预报
/// @details   以卫星影响球为拼接面的圆锥曲线拼接预报。
/// @author    axel
/// @date      2026-09-10
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

#include "PatchedConic.hpp"
#include "AstCore/TwoBody.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/Frame.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstCore/OrbitParam.hpp"
#include "AstCore/RunTimeSolarSystem.hpp"
#include "AstCore/RunTimeJplDe.hpp"
#include "AstCore/TimeInterval.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstUtil/Constants.h"
#include "AstUtil/Logger.hpp"
#include "AstUtil/I18n.hpp"
#include "AstMath/BrentSolver.hpp"
#include "AstMath/BisectionSolver.hpp"
#include "AstMath/KinematicTransform.hpp"

#include <cmath>

AST_NAMESPACE_BEGIN

namespace
{

/// @brief 判定为严格抛物线的偏心率容差
constexpr double kParabolicTol = 1e-12;

// 以下时间尺度全部由两个天体的实际几何动态确定，不写死具体天数——原先的
// 30天/30天/0.05天只适用于地月系统，换成木卫或行星际转移就完全失配。
// 统一的时间标尺取卫星绕主天体的轨道周期 T_sec。

/// @brief 扫描步长 = T_sec / 本值
/// @details 地月代入约 0.0546 天，与原硬编码的 0.05 天相当。
///          步长须保证不跳过影响球：每步距离变化 v_rel*dt 应远小于球直径 2*R_SOI。
///          以 v_rel 上界 = sqrt(2*GM_pri/a_sec) + v_sec 估算，地月需 dt < 0.62 天，
///          取 T_sec/500 有约 11 倍余量，与原先 0.05 天的余量相当。
constexpr double kScanStepsPerPeriod = 500.0;

/// @brief 主天体段最大搜索时长 = 本值 * T_sec
/// @details 取 2 个周期：存在"远拱点远超卫星轨道、需先飞到远拱点再折返"的转移
///          （见 testPatchedConic 中的算例），单个主天体段可占约 0.3 个周期。
constexpr double kMaxPrimaryLegPeriods = 2.0;

/// @brief 椭圆俘获时，卫星段时长上界 = 本值 * 该椭圆周期
/// @details 椭圆俘获下 tToPeri 最大为一个周期，取 2 倍留出取"下一个"近拱点的余地。
constexpr double kMaxSecondaryLegPeriods = 2.0;

/// @brief 双曲掠过时，卫星段时长上界 = 本值 * sqrt(R_SOI^3 / GM_sec)
/// @details sqrt(R_SOI^3/GM_sec) 是"从SOI边界到近拱点"的特征时间，地月约 2.81 天。
///          不能用 T_sec 作标尺——双曲掠过与卫星绕主天体的周期无关。
constexpr double kMaxSecondaryLegSOIFactor = 5.0;

} // namespace



double aSOIRadius(double gmPrimary, double gmSatellite, double orbitSemiMajorAxis)
{
    return orbitSemiMajorAxis * std::pow(gmSatellite / gmPrimary, 0.4);
}


errc_t aPatchedConicPropToSecondary
(
    CelestialBody& primaryBody,
    CelestialBody& secondaryBody,
    double           soiRadius,
    const TimePoint& initTime,
    const CartState& initState,
    TimePoint&       outTime,
    CartState&       outState
)
{
    const double primaryGM   = primaryBody.getGM();
    const double secondaryGM = secondaryBody.getGM();

    Frame* primaryFrame   = primaryBody.getFrameInertial();
    Frame* secondaryFrame = secondaryBody.getFrameInertial();
    if (primaryFrame == nullptr || secondaryFrame == nullptr)
    {
        aError(_("天体惯性坐标系不可用"));
        return eErrorInvalidParam;
    }

    // ---- 卫星在初始时刻的位置，用于主天体段远拱点粗筛 ----
    Vector3d secPos0, secVel0;
    {
        errc_t rc = secondaryBody.getPosVelIn(primaryFrame, initTime, secPos0, secVel0);
        if (rc != eNoError)
            return rc;
    }
    const double secondaryOrbitRadius = secPos0.norm();

    // ---- 特征时间尺度：卫星绕主天体的轨道周期 ----
    // 扫描步长与两段时长上界都以它为标尺，从而适配任意天体对。
    const double tSecondary = [&]() -> double
    {
        ModOrbElem eleRel;
        aCartToModOrbElem(secPos0, secVel0, primaryGM, eleRel);
        return eleRel.getPeriod(primaryGM);
    }();

    const double scanStep              = tSecondary / kScanStepsPerPeriod;
    const double maxPrimaryLegDuration = kMaxPrimaryLegPeriods * tSecondary;

    // ---- 主天体段轨道根数与初筛 ----
    {
        ModOrbElem ele0;
        aCartToModOrbElem(initState.pos(), initState.vel(), primaryGM, ele0);
        if (ele0.e_ < 1.0)
        {
            // 远拱点够不到卫星影响球，一定进不去，直接短路，避免无谓的长时间扫描
            const double rApo = ele0.getApoRad();
            if (rApo < secondaryOrbitRadius - soiRadius)
            {
                aWarning(_("主天体段轨道远拱点低于卫星影响球，无法到达卫星"));
                return eErrorNotFound;
            }
        }
        else
        {
            // 双曲/抛物线轨道：远拱点为无穷，一定够得着
        }
    }


    // ---- 主天体段：建立SOI穿越时刻的求根区间 ----
    // 根函数 = 飞行器到卫星的距离 - SOI半径，穿越时由正变负。
    auto g = [&](double dt) -> double
    {
        // aTwoBodyProp 的 r/v 是 in-out 引用，每次必须重新拷贝
        Vector3d r = initState.pos();
        Vector3d v = initState.vel();
        aTwoBodyProp(dt, primaryGM, r, v);

        Vector3d secPos;
        if (secondaryBody.getPosIn(primaryFrame, initTime + dt, secPos) != eNoError)
            return kNaN;

        const Vector3d d = r - secPos;
        return d.norm() - soiRadius;
    };

    

    double tLo = 0.0;
    double tHi = 0.0;
    {
        bool bracketed = false;
        double last = g(0.0);
        if (last <= 0)
        {
            aWarning(_("初始状态已在卫星影响球内，不适用于本前推模型"));
            return eErrorOutOfRange;
        }
        for (double t = 0.0; t < maxPrimaryLegDuration; t += scanStep)
        {
            const double g2 = g(t + scanStep);
            if (last > 0.0 && g2 <= 0.0)
            {
                tLo = t;
                tHi = t + scanStep;
                bracketed = true;
                break;
            }
            last = g2;
        }
        if (!bracketed)
        {
            aWarning(_("在最大搜索时长内未进入卫星影响球"));
            return eErrorNotFound;
        }
    }

    // ---- 5. 求根：Brent 为主，二分兜底 ----
    double dtStar = 0.0;
    {
        BrentqSolver brent;
        brent.setAbsTol(1e-3);      // 自变量容差，单位为秒，对应毫米级位置误差
        brent.setMaxIter(100);
        if (brent.solve(g, tLo, tHi, dtStar) != eNoError)
        {
            // 括号有效时二分法保证收敛，作为安全网
            BisectionSolver bisect;
            bisect.setAbsTol(1e-3);
            bisect.setMaxIter(200);
            if (bisect.solve(g, tLo, tHi, dtStar) != eNoError)
            {
                aError(_("卫星影响球穿越时刻求解不收敛"));
                return eErrorMaxIter;
            }
        }
    }

    // ---- 6. 拼接：主天体惯性系 -> 卫星惯性系 ----
    const TimePoint tStar = initTime + dtStar;
    CartState scSecondary;
    {
        Vector3d rSc = initState.pos();
        Vector3d vSc = initState.vel();
        aTwoBodyProp(dtStar, primaryGM, rSc, vSc);

        // 转换到卫星的天体惯性系
        KinematicTransform transform;
        errc_t rc = aFrameTransform(primaryFrame, secondaryFrame, tStar, transform);
        if (rc != eNoError)
            return rc;
        transform.transformPositionVelocity(rSc, vSc, scSecondary.pos(), scSecondary.vel());
    }

    // ---- 7. 卫星段：解析求到近拱点的时长 ----
    ModOrbElem eleSec;
    aCartToModOrbElem(scSecondary.pos(), scSecondary.vel(), secondaryGM, eleSec);

    // 卫星段时长上界：按该段实际轨道形态分别确定。
    // 不能用单一常量——双曲掠过的特征时间是"从SOI边界到近拱点"的 sqrt(R_SOI^3/GM_sec)
    // （地月约 2.8 天），而大椭圆俘获的一个周期可达数十天，两者相差一个数量级。
    const bool isCaptured = (eleSec.e_ < 1.0);
    const double maxSecondaryLegDuration = isCaptured
        ? kMaxSecondaryLegPeriods * eleSec.getPeriod(secondaryGM)
        : kMaxSecondaryLegSOIFactor * std::sqrt(soiRadius * soiRadius * soiRadius / secondaryGM);

    // 真近点角是周期量，其归一化由 aTrueToTimePastPeri 内部处理
    double tToPeri = -eleSec.getTimePastPeri(secondaryGM);
    if (isCaptured)
    {
        // 椭圆轨道才需要按周期归一，取"下一个"近拱点
        const double period = aSMAToPeriod(eleSec.getSMA(), secondaryGM);
        if (tToPeri < 0.0)
            tToPeri += period;
    }
    if (tToPeri < 0.0 || tToPeri > maxSecondaryLegDuration)
    {
        aWarning(_("卫星段到近拱点的时长超出合理范围，飞行器可能未被卫星俘获"));
        return eErrorOutOfRange;
    }

    // ---- 8. 传播到近拱点并输出 ----
    Vector3d rPeri = scSecondary.pos();
    Vector3d vPeri = scSecondary.vel();
    aTwoBodyProp(tToPeri, secondaryGM, rPeri, vPeri);

    outTime = tStar + tToPeri;
    outState.pos() = rPeri;
    outState.vel() = vPeri;
    return eNoError;
}


errc_t aPatchedConicPropEarthToMoon(
    double           soiRadius,
    const TimePoint& initTime,
    const CartState& initState,
    TimePoint&       outTime,
    CartState&       outState
)
{
    CelestialBody* moon  = aGetMoon();
    CelestialBody* earth = aGetEarth();
    if (moon == nullptr || earth == nullptr)
    {
        aError(_("太阳系数据未加载，请先调用 aInitialize()"));
        return eErrorNotInit;
    }
    return aPatchedConicPropToSecondary(*earth, *moon, soiRadius, initTime, initState, outTime, outState);
}


errc_t aPatchedConicPropToPrimary
(
    CelestialBody& primaryBody,
    CelestialBody& secondaryBody,
    double           soiRadius,
    const TimePoint& initTime,
    const CartState& initState,
    TimePoint&       outTime,
    CartState&       outState
)
{
    const double primaryGM   = primaryBody.getGM();
    const double secondaryGM = secondaryBody.getGM();

    Frame* primaryFrame   = primaryBody.getFrameInertial();
    Frame* secondaryFrame = secondaryBody.getFrameInertial();
    if (primaryFrame == nullptr || secondaryFrame == nullptr)
    {
        aError(_("天体惯性坐标系不可用"));
        return eErrorInvalidParam;
    }

    const double tauSOI = std::sqrt(soiRadius * soiRadius * soiRadius / secondaryGM);

    // ---- 卫星段：正向求穿出影响球的时刻 ----
    // 卫星惯性系下，影响球就是以原点为心、R_SOI 为半径的球面，
    // 故根函数无需查询星历。正向传播时 g 由负变正（球内 -> 球外）。
    auto g = [&](double s) -> double
    {
        // s 为正向传播时长，对应时刻 initTime + s
        Vector3d r = initState.pos();
        Vector3d v = initState.vel();
        aTwoBodyProp(s, secondaryGM, r, v);
        return r.norm() - soiRadius;
    };

    if (g(0.0) > 0.0)
    {
        aWarning(_("初始状态已在卫星影响球外，本模型要求输入位于球内"));
        return eErrorOutOfRange;
    }

    // 卫星段的时间尺度：取该状态在卫星附近轨道的周期（椭圆）或 SOI 特征时间（双曲）
    ModOrbElem eleIn;
    aCartToModOrbElem(initState.pos(), initState.vel(), secondaryGM, eleIn);
    const double smaIn = eleIn.getSMA();
    const bool inIsElliptic = (eleIn.e_ < 1.0) && std::isfinite(smaIn) && smaIn > 0.0;
    const double secScale = inIsElliptic ? aSMAToPeriod(smaIn, secondaryGM) : tauSOI;
    const double maxSecondaryLegForward = inIsElliptic
        ? kMaxSecondaryLegPeriods * secScale
        : kMaxSecondaryLegSOIFactor * secScale;
    const double scanStep = secScale / kScanStepsPerPeriod;

    double sLo = 0.0;
    double sHi = 0.0;
    {
        bool bracketed = false;
        double last = g(0.0);
        for (double s = 0.0; s < maxSecondaryLegForward; s += scanStep)
        {
            const double g2 = g(s + scanStep);
            if (last <= 0.0 && g2 > 0.0)
            {
                sLo = s;
                sHi = s + scanStep;
                bracketed = true;
                break;
            }
            last = g2;
        }
        if (!bracketed)
        {
            aWarning(_("在最大传播时长内未穿出卫星影响球"));
            return eErrorNotFound;
        }
    }

    double sStar = 0.0;
    {
        BrentqSolver brent;
        brent.setAbsTol(1e-3);      // 自变量容差，单位为秒
        brent.setMaxIter(100);
        if (brent.solve(g, sLo, sHi, sStar) != eNoError)
        {
            BisectionSolver bisect;
            bisect.setAbsTol(1e-3);
            bisect.setMaxIter(200);
            if (bisect.solve(g, sLo, sHi, sStar) != eNoError)
            {
                aError(_("卫星影响球穿出时刻求解不收敛"));
                return eErrorMaxIter;
            }
        }
    }

    // ---- 拼接：卫星惯性系 -> 主天体惯性系 ----
    const TimePoint tStar = initTime + sStar;
    CartState scPrimary;
    {
        Vector3d r = initState.pos();
        Vector3d v = initState.vel();
        aTwoBodyProp(sStar, secondaryGM, r, v);

        KinematicTransform transform;
        errc_t rc = aFrameTransform(secondaryFrame, primaryFrame, tStar, transform);
        if (rc != eNoError)
            return rc;
        transform.transformPositionVelocity(r, v, scPrimary.pos(), scPrimary.vel());
    }

    // ---- 主天体段：正向传播至下一个近拱点 ----
    // 穿出影响球时的主天体惯性系状态，是卫星借力**之后**的那条轨道
    // （与来向轨道不是同一条），故这里求的是返回段的近拱点。
    ModOrbElem elePri;
    aCartToModOrbElem(scPrimary.pos(), scPrimary.vel(), primaryGM, elePri);

    // 正向走：近拱点在未来，取"到下一个近拱点的时间"。
    // 真近点角为正表示已过近拱点，椭圆情形需走到下一圈。
    const bool priIsElliptic = (elePri.e_ < 1.0);
    double tToPeri = -elePri.getTimePastPeri(primaryGM);
    if (priIsElliptic && tToPeri < 0.0)
        tToPeri += elePri.getPeriod(primaryGM);
    if (!std::isfinite(tToPeri) || tToPeri < 0.0)
    {
        // 双曲情形下不存在未来的近拱点（航天器已逃逸主天体）
        aWarning(_("主天体段轨道非椭圆，不存在后续近拱点"));
        return eErrorOutOfRange;
    }
    if (priIsElliptic && tToPeri > kMaxSecondaryLegPeriods * elePri.getPeriod(primaryGM))
    {
        aWarning(_("主天体段到下一个近拱点的时长超出合理范围"));
        return eErrorOutOfRange;
    }

    Vector3d rPri = scPrimary.pos();
    Vector3d vPri = scPrimary.vel();
    aTwoBodyProp(tToPeri, primaryGM, rPri, vPri);

    outTime = tStar + tToPeri;
    outState.pos() = rPri;
    outState.vel() = vPri;
    return eNoError;
}


errc_t aPatchedConicPropMoonToEarth(
    double           soiRadius,
    const TimePoint& initTime,
    const CartState& initState,
    TimePoint&       outTime,
    CartState&       outState
)
{
    CelestialBody* moon  = aGetMoon();
    CelestialBody* earth = aGetEarth();
    if (moon == nullptr || earth == nullptr)
    {
        aError(_("太阳系数据未加载，请先调用 aInitialize()"));
        return eErrorNotInit;
    }
    return aPatchedConicPropToPrimary(*earth, *moon, soiRadius, initTime, initState, outTime, outState);
}

AST_NAMESPACE_END

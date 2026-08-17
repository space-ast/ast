///
/// @file      EclipseEventFinder.cpp
/// @brief     日食事件求解器实现
/// @details   双锥阴影模型 + 定步长采样三态分类（阳光/半影/本影）+ 布尔谓词二分精化边界。
/// @author    axel
/// @date      2026-08-15
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

#include "EclipseEventFinder.hpp"
#include "AstCore/Eclipse.hpp"
#include "AstCore/RunTimeSolarSystem.hpp"
#include "AstMath/BrentOptimizer.hpp"
#include "AstUtil/Logger.hpp"

#include <algorithm>
#include <vector>

AST_NAMESPACE_BEGIN

namespace
{
    /// @brief 边界二分精化精度（秒）
    constexpr double kTimeTol = 1e-4;

    /// @brief 单个遮挡体在指定时刻的光照比例：0=全阴影（本影），1=全光照，(0,1)=半影
    double lightingRatio(const TimePoint& t, Point* point, CelestialBody* sun, CelestialBody* body)
    {
        return aLightingRatio_DualConeModel(t, point, sun, body);
    }

    /// @brief 光照比例 → 光照类型
    ELightingType classify(double l)
    {
        if (l >= 1.0) { return ELightingType::eSunlight; }
        if (l <= 0.0) { return ELightingType::eUmbra; }
        return ELightingType::ePenumbra;
    }

    /// @brief 二分精化谓词翻转时刻：pred 在 [lo, hi] 内由 false 变 true，返回首个 true 时刻
    template <typename Pred>
    TimePoint bisectEnter(const TimePoint& lo, const TimePoint& hi, Pred pred)
    {
        TimePoint a = lo;   // pred(a) == false
        TimePoint b = hi;   // pred(b) == true
        while ((b - a) > kTimeTol)
        {
            TimePoint mid = a + (b - a) * 0.5;
            if (pred(mid)) { b = mid; } else { a = mid; }
        }
        return b;
    }

    /// @brief 二分精化谓词翻转时刻：pred 在 [lo, hi] 内由 true 变 false，返回最后一个 true 时刻
    template <typename Pred>
    TimePoint bisectExit(const TimePoint& lo, const TimePoint& hi, Pred pred)
    {
        TimePoint a = lo;   // pred(a) == true
        TimePoint b = hi;   // pred(b) == false
        while ((b - a) > kTimeTol)
        {
            TimePoint mid = a + (b - a) * 0.5;
            if (pred(mid)) { a = mid; } else { b = mid; }
        }
        return a;
    }

    /// @brief 对单个遮挡体求解区间内的日食事件，追加到 events
    void findEventsForBody(const TimeInterval& interval, Point* point, CelestialBody* sun,
                           CelestialBody* body, double stepSize, std::vector<EclipseEvent>& events)
    {
        auto lighting  = [&](const TimePoint& t) { return lightingRatio(t, point, sun, body); };
        auto shadow    = [&](const TimePoint& t) { return 1.0 - lightingRatio(t, point, sun, body); };
        auto isShadow  = [&](const TimePoint& t) { return lighting(t) < 1.0; };
        auto isUmbra   = [&](const TimePoint& t) { return lighting(t) <= 0.0; };

        // 求 [a, b] 内最深遮蔽（最小光照比）时刻与比例
        auto deepestIn = [&](const TimePoint& a, const TimePoint& b, TimePoint& at) {
            double x = 0.0;
            double lmin = 1.0;
            const errc_t rc = aBrentMinimize([&](double dx) { 
                return lighting(a + dx); 
            }, 0.0, b - a, x, &lmin, kTimeTol, 0.0, 100);
            if (rc != eNoError)
            {
                aWarning("lighting minimization did not converge, using best-effort value");
            }
            at = a + x;
            return lmin;
        };

        // 最深遮蔽点/最大遮蔽比例
        auto finalize = [&](EclipseEvent& e) {
            if (e.hasUmbra())
            {
                // 本影事件：最大遮蔽（100%）在本影进入时刻首次达到，整段本影保持不变
                e.setTimeAtMaxShadow(e.getUmbraStart());
                e.setMaxShadowRatio(1.0);
            }
            else
            {
                // 掠影：最深遮蔽点需一维优化求取
                const TimePoint refTime = e.getPenumbraStart();
                const double dur = e.getPenumbraStop() - e.getPenumbraStart();
                if (dur > 0.0)
                {
                    TimePoint tMax;
                    const double lmin = deepestIn(refTime, e.getPenumbraStop(), tMax);
                    e.setTimeAtMaxShadow(tMax);
                    e.setMaxShadowRatio(1.0 - lmin);
                }
                else
                {
                    e.setTimeAtMaxShadow(refTime);
                    e.setMaxShadowRatio(shadow(refTime));
                }
                // 掠影：本影起止均取最深遮蔽时刻
                e.setUmbraStart(e.getTimeAtMaxShadow());
                e.setUmbraStop(e.getTimeAtMaxShadow());
            }
        };

        const TimePoint& start = interval.start();
        const TimePoint& stop  = interval.stop();

        ELightingType prevState = classify(lighting(start));

        EclipseEvent ev;
        bool inEvent = false;
        if (prevState != ELightingType::eSunlight)
        {
            ev.setObstruction(body);
            ev.setPenumbraStart(start);
            if (prevState == ELightingType::eUmbra)
            {
                ev.setUmbraStart(start);
                ev.setHasUmbra(true);
            }
            inEvent = true;
        }

        /*
        @todo 如果步长过大，则可能产生事件遗漏，例如纯半影事件等情况
        */
        TimePoint t = start;
        while (t < stop)
        {
            const TimePoint tNext = (stop - t) > stepSize ? (t + stepSize) : stop;
            const ELightingType curState = classify(lighting(tNext));

            if (prevState != curState)
            {
                if (prevState == ELightingType::eSunlight)
                {
                    if (curState == ELightingType::ePenumbra)
                    {
                        TimePoint tmin;
                        const double lmin = deepestIn(t, tNext, tmin);

                        ev = EclipseEvent{};
                        ev.setObstruction(body);
                        ev.setPenumbraStart(bisectEnter(t, tNext, isShadow));
                        // 直接从光照跳到了第二个半影，需要处理本影起止时刻
                        if (lmin <= 0.0)   // 本影在本步内被完整跨越，此刻处于第二个半影
                        {
                            ev.setUmbraStart(bisectEnter(t, tmin, isUmbra));    // 下降段：false→true
                            ev.setUmbraStop(bisectExit(tmin, tNext, isUmbra));  // 上升段：true→false
                            ev.setHasUmbra(true);
                        }
                        inEvent = true;
                    }
                    else if (curState == ELightingType::eUmbra)
                    {
                        ev = EclipseEvent{};
                        ev.setObstruction(body);
                        ev.setPenumbraStart(bisectEnter(t, tNext, isShadow));
                        ev.setUmbraStart(bisectEnter(t, tNext, isUmbra));
                        ev.setHasUmbra(true);
                        inEvent = true;
                    }
                }
                else if (prevState == ELightingType::ePenumbra)
                {
                    if (curState == ELightingType::eUmbra)
                    {
                        ev.setUmbraStart(bisectEnter(t, tNext, isUmbra));
                        ev.setHasUmbra(true);
                    }
                    else if (curState == ELightingType::eSunlight)
                    {
                        TimePoint tmin;
                        const double lmin = deepestIn(t, tNext, tmin);

                        ev.setPenumbraStop(bisectExit(t, tNext, isShadow));
                        // 直接从第一个半影跳到了光照，需要处理本影起止时刻
                        if (lmin <= 0.0)   // 本影在本步内被完整跨越但端点漏掉：补齐本影起止
                        {
                            ev.setUmbraStart(bisectEnter(t, tmin, isUmbra));
                            ev.setUmbraStop(bisectExit(tmin, tNext, isUmbra));
                            ev.setHasUmbra(true);
                        }
                        finalize(ev);
                        events.push_back(ev);
                        inEvent = false;
                    }
                }
                else // prevState == eUmbra
                {
                    if (curState == ELightingType::ePenumbra)
                    {
                        ev.setUmbraStop(bisectExit(t, tNext, isUmbra));
                    }
                    else if (curState == ELightingType::eSunlight)
                    {
                        ev.setUmbraStop(bisectExit(t, tNext, isUmbra));
                        ev.setPenumbraStop(bisectExit(t, tNext, isShadow));
                        finalize(ev);
                        events.push_back(ev);
                        inEvent = false;
                    }
                }
            }

            prevState = curState;
            t = tNext;
        }

        if (inEvent)
        {
            ev.setPenumbraStop(stop);
            if (prevState == ELightingType::eUmbra)
            {
                ev.setUmbraStop(stop);
            }
            finalize(ev);
            events.push_back(ev);
        }
    }
} // namespace


errc_t EclipseEventFinder::find(const TimeInterval& interval, std::vector<EclipseEvent>& events) const
{
    auto point = point_.get();
    if (!point)
    {
        aError("point is null");
        return eErrorNullPtr;
    }
    auto lightSource = lightSource_.get();
    if(!lightSource)
    {
        lightSource = aGetSun();
        if (!lightSource)
        {
            aError("no light source");
            return eErrorNullPtr;
        }
    }

    std::vector<HCelestialBody> bodies = occultingBodies_;
    if (bodies.empty())
    {
        aWarning("no occulting bodies, using earth and moon as default");
        if (auto earth = aGetEarth()) { bodies.push_back(earth); }
        if (auto moon = aGetMoon())  { bodies.push_back(moon); }
        if (bodies.empty())
        {
            aError("no occulting bodies");
            return eErrorNullPtr;
        }
    }

    if (stepSize_ <= 0.0 || interval.duration() <= 0.0)
    {
        aError("invalid step size or interval");
        return eErrorInvalidParam;
    }

    events.clear();

    int nValidBodies = 0;
    for (auto& b : bodies)
    {
        if (!b || b.get() == lightSource) { continue; }
        ++nValidBodies;
        findEventsForBody(interval, point, lightSource, b.get(), stepSize_, events);
    }

    if (nValidBodies == 0)
    {
        aError("all occulting bodies are null or equal to the light source");
        return eErrorInvalidParam;
    }

    std::sort(events.begin(), events.end(),
              [](const EclipseEvent& a, const EclipseEvent& b) {
                  return a.getPenumbraStart() < b.getPenumbraStart();
              });

    return eNoError;
}

AST_NAMESPACE_END

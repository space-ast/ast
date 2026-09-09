///
/// @file      visualization_simple_demo.cpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-09-06
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

#include "ast/AstVisualization.hpp"
#include "ast/EphemerisTwoBody.hpp"
#include "ast/MotionHPOP.hpp"
#include "ast/HPOPForceModel.hpp"
#include "ast/StateCartesian.hpp"
#include "ast/Ephemeris.hpp"
#include "ast/ScopedPtr.hpp"
#include "ast/Constants.hpp"
#include "ast/OrbitElement.hpp"
#include "ast/RunTime.hpp"
#include "ast/Literals.hpp"

AST_USING_NAMESPACE
using namespace _AST literals;

/// @brief 生成二体（两体）星历
/// @details 地球同步轨道（GEO）的二体传播星历，解析求值，任意时刻可采样。
/// @param epoch 历元
/// @return 二体星历（所有权转移给调用者）；失败返回空
static ScopedPtr<Ephemeris> makeTwoBodyEphemeris(const TimePoint& epoch)
{
    const double radius = kEarthRadius + 35786.0 * 1000.0;   // 地球同步轨道半径 (m)
    const double speed  = std::sqrt(kEarthGrav / radius);    // 圆轨道速度 (m/s)
    CartState initState{Vector3d{radius, 0.0, 0.0}, Vector3d{0.0, speed, 0.0}};

    HFrame icrf = aGetEarth()->getFrameICRF();   // 参考系：地球 ICRF
    return ScopedPtr<Ephemeris>(new EphemerisTwoBody(icrf.get(), kEarthGrav, epoch, initState));
}

/// @brief 生成 HPOP 高精度星历
/// @details 500 km 高度、51.6° 倾角近圆轨道，JGM3(20×20) 地球重力场，30 天预报，
///          能呈现 J2 升交点漂移；内部采样为 EphemerisLagrangeVar 供任意时刻插值。
/// @param epoch 历元
/// @param interval 预报/采样时间区间
/// @return HPOP 星历（所有权转移给调用者）；失败返回空
static ScopedPtr<Ephemeris> makeHPOPEphemeris(const TimePoint& epoch, const TimeInterval& interval)
{
    HPOPForceModel hpForce;
    hpForce.gravity().model_ = "JGM3";
    hpForce.gravity().maxDegree_ = 20;
    hpForce.gravity().maxOrder_ = 20;

    auto hpMotion = MotionHPOP::New();
    hpMotion->setForceModel(hpForce);
    hpMotion->setInterval(interval);

    // 近地轨道初始状态（500 km 高度近圆轨道，带倾角以便显示 J2 升交点漂移）
    const double leoRadius = kEarthRadius + 500.0 * 1000.0;
    const double leoSpeed  = std::sqrt(kEarthGrav / leoRadius);
    const double leoInc    = 51.6_deg;    // 轨道倾角（°），赤道轨道 (i=0) 无升交点漂移
    CartState leoState;
    leoState.pos() = Vector3d{leoRadius, 0.0, 0.0};
    leoState.vel() = Vector3d{0.0, leoSpeed * std::cos(leoInc), leoSpeed * std::sin(leoInc)};

    auto initialState = StateCartesian::New();
    initialState->setFrameByName("Earth ICRF");
    initialState->setGM(kEarthGrav);
    initialState->setStateEpoch(epoch);
    initialState->setState(leoState);

    hpMotion->setInitialState(initialState);
    hpMotion->setPropagationFrame(initialState->getFrame());

    ScopedPtr<Ephemeris> eph;
    if (hpMotion->makeEphemerisSimple(eph) != eNoError) {
        return {};
    }
    return eph;
}

int main()
{
    auto epoch = TimePoint::CurrentTime();

    auto tbEph = makeTwoBodyEphemeris(epoch);
    const TimeInterval hpInterval{epoch, epoch + 30_day};
    auto hpEph = makeHPOPEphemeris(epoch, hpInterval);

    namespace vis = _AST visualization;
    auto earth = aGetEarth();
    auto moon = aGetMoon();

    // 视图参考系：地球 ICRF。天体/轨迹均按此世界系定位（该系原点即地球中心）。
    vis::gcv()->setFrame(aGetEarth()->getFrameICRF());

    vis::trajectory(tbEph.get())->setInterval({epoch, epoch + 3_day});
    vis::trajectory(hpEph.get())->setInterval(hpInterval);

    vis::planet(earth);
    vis::planet(moon);


    // 说明：vis::show(epoch) 会在运行时加载渲染后端，并按 epoch 时刻布局天体；
    // 若存在渲染后端则弹出交互窗口，否则会打印警告。
    vis::show(epoch);
}

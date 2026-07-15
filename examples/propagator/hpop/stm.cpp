///
/// @file      stm.cpp
/// @brief     STM 及参数敏感度逐帧采集示例
/// @details   演示使用 ODEIntegrator 的 stateObserver 机制，
///            在自适应步长积分过程中每步采集状态转移矩阵 Φ、
///            ∂[r,v]/∂B（Drag 敏感度）、∂[r,v]/∂K（SRP 敏感度）。
///            所有帧存入 std::vector，预报结束后统一 CSV 输出。
/// @author    axel
/// @date      2026-07-14
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

#include "ast/HPOP.hpp"
#include "ast/HPOPForceModel.hpp"
#include "ast/RunTime.hpp"
#include "ast/DataContext.hpp"
#include "ast/Vector.hpp"
#include "ast/Matrix.hpp"
#include "ast/Environment.hpp"
#include "ast/StateMapper.hpp"
#include "ast/OrbitElement.hpp"
#include "ast/SpacecraftParam.hpp"
#include "ast/Literals.hpp"
#include "ast/ODEIntegrator.hpp"
#include "ast/ODEVarStepIntegrator.hpp"
#include <vector>
#include <cstdio>

AST_USING_NAMESPACE
using namespace _AST literals;

// ── 帧数据结构 ─────────────────────────────────────────

struct StmFrame
{
    double   time{};        // 相对时间 (s)
    CartState state{};      // 位置 + 速度
    Matrix6d stm{};         // 6×6 状态转移矩阵
    Vector6d dragSens{};    // ∂[r,v]/∂B
    Vector6d srpSens{};     // ∂[r,v]/∂K
};

// ── CSV 输出 ────────────────────────────────────────────

static void printCsv(const std::vector<StmFrame>& frames)
{
    // 表头
    printf("time,px,py,pz,vx,vy,vz");
    for (int i = 0; i < 6; ++i)
        for (int j = 0; j < 6; ++j)
            printf(",stm%d%d", i, j);
    for (int i = 0; i < 6; ++i) printf(",ds%d", i);
    for (int i = 0; i < 6; ++i) printf(",ss%d", i);
    printf("\n");

    // 数据行
    for (const auto& f : frames)
    {
        printf("%.6f", f.time);
        for (int i = 0; i < 3; ++i) printf(",%.15e", f.state.position()[i]);
        for (int i = 0; i < 3; ++i) printf(",%.15e", f.state.velocity()[i]);
        for (int i = 0; i < 6; ++i)
            for (int j = 0; j < 6; ++j)
                printf(",%.15e", f.stm(i, j));
        for (int i = 0; i < 6; ++i) printf(",%.15e", f.dragSens[i]);
        for (int i = 0; i < 6; ++i) printf(",%.15e", f.srpSens[i]);
        printf("\n");
    }
}

// ── main ────────────────────────────────────────────────

int main()
{
    aInitialize();
    aDataContext_GetEOP()->unload();
    setlocale(LC_ALL, ".UTF-8");

    // ── 1. 配置力模型 ──────────────────────────────────

    HPOPForceModel forcemodel;
    forcemodel.gravity().model_ = "JGM3";
    forcemodel.gravity().maxDegree_ = 2;
    forcemodel.gravity().maxOrder_  = 2;
    forcemodel.useSTM(true);

    forcemodel.useDrag(true);
    forcemodel.drag().atmDensityModel_ = EAtmDensityModel::eMSIS1986;

    forcemodel.useSRP(true);
    forcemodel.srp().shadowModel_ = EShadowModel::eDualCone;

    forcemodel.useDragSensitivity(true);
    forcemodel.useSRPSensitivity(true);

    // ── 2. 航天器参数 ──────────────────────────────────

    SpacecraftParam scParam;
    scParam.setDryMass(1000.0);
    scParam.setFuelMass(0.0);
    scParam.setDragArea(20.0);
    scParam.setCd(2.2);
    scParam.setSrpArea(20.0);
    scParam.setCr(1.0);

    // ── 3. 创建预报器 ──────────────────────────────────

    HPOP propagator;
    errc_t err = propagator.setForceModel(forcemodel);
    if (err != eNoError) {
        fprintf(stderr, "setForceModel failed: %d\n", err);
        return -1;
    }
    propagator.setSpacecraftParam(scParam);

    // ── 4. 初始条件 ────────────────────────────────────

    auto start = TimePoint::FromUTC(2026, 1, 20, 4, 0, 0);
    auto end   = TimePoint::FromUTC(2026, 1, 21, 4, 0, 0);

    CartState state;
    state.position() = Vector3d{6678137, 0, 0};
    state.velocity() = Vector3d{0, 6789.53029, 3686.414173};

    Matrix6d stm{};
    for (int i = 0; i < 6; ++i)
        stm(i, i) = 1.0;

    Vector6d stateSensWrtDrag{};
    Vector6d stateSensWrtSRP{};

    // ── 5. 注册 observer：每步采集帧数据 ────────────────

    std::vector<StmFrame> frames;
    auto* mapper = propagator.stateMapper();
    auto* integrator = propagator.getIntegrator();
    // 设置积分器按固定步长60秒积分
    if(auto varintegrator = dynamic_cast<ODEVarStepIntegrator*>(integrator))
    {
        varintegrator->setUseFixedStep(true);
        varintegrator->setStepSize(60);
    }

    integrator->addStateObserver([&frames, mapper](double* y, double& x, ODEIntegrator*) {
        StmFrame f;
        f.time = x;
        mapper->toState(y, f.state);
        mapper->toStateTransitionMatrix(y, f.stm);
        mapper->toStateSensitivityWrtDrag(y, f.dragSens);
        mapper->toStateSensitivityWrtSRP(y, f.srpSens);
        frames.push_back(f);
    });

    // ── 6. 预报 ────────────────────────────────────────

    err = propagator.propagate(start, end, state, stm,
                               stateSensWrtDrag, stateSensWrtSRP);
    if (err != eNoError) {
        fprintf(stderr, "propagate failed: %d\n", err);
        return -1;
    }

    integrator->clearStateObservers();
    // expected:
    // state:    {pos_={x_=6426310.1944292756 y_=-1729560.6268997970 z_=-474610.30727389897 } vel_={x_=2024.3270414323702 ...} }
    // stm:      first row: {-82.465447186568412, 0.00053472762888822495, -0.070726596660925997, -253.93305481600945, ...}
    // dragSens: {2503942.6140104369, 8509256.7297997586, 4763867.5046518976, -11240.949623401957, ...}
    // srpSens:  {-390.37047021941561, -554.73057392318208, -331.79228369067152, 0.60926698269460056, ...}
    // ── 7. 输出 ────────────────────────────────────────

    // printCsv(frames);

    printf("# frames=%zu\n", frames.size());
    printf("# final pos:  %s\n", state.position().toString().c_str());
    printf("# final vel:  %s\n", state.velocity().toString().c_str());
    printf("# final dragSens: [%.6e, %.6e, %.6e, %.6e, %.6e, %.6e]\n",
           stateSensWrtDrag[0], stateSensWrtDrag[1], stateSensWrtDrag[2],
           stateSensWrtDrag[3], stateSensWrtDrag[4], stateSensWrtDrag[5]);
    printf("# final srpSens:  [%.6e, %.6e, %.6e, %.6e, %.6e, %.6e]\n",
           stateSensWrtSRP[0], stateSensWrtSRP[1], stateSensWrtSRP[2],
           stateSensWrtSRP[3], stateSensWrtSRP[4], stateSensWrtSRP[5]);

    aUninitialize();
    return 0;
}

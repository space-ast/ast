///
/// @file      testHPOP_StateTransitionMatrix.cpp
/// @brief     状态转换矩阵预报测试
/// @details   验证启用 STM 后的轨道预报及状态转换矩阵的正确性
/// @author    axel
/// @date      2026-07-12
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
#include "ast/HPOPEquation.hpp"
#include "ast/EOP.hpp"
#include "ast/RunTime.hpp"
#include "ast/Test.hpp"
#include "ast/Vector.hpp"
#include "ast/Matrix.hpp"
#include "ast/Environment.hpp"
#include "ast/ODEVarStepIntegrator.hpp"
#include "ast/StateMapper.hpp"
#include "ast/OrbitElement.hpp"
#include "ast/MathOperator.hpp"

AST_USING_NAMESPACE

class HPOPSTMTest : public ::testing::Test
{
    void SetUp() override
    {
        aInitialize();
        aDataContext_GetEOP()->unload();
    }

    void TearDown() override
    {
        aUninitialize();
    }
};

/// @brief 测试启用 STM 后方程维度
/// @details
/// 启用 STM 后，状态向量应包括 6 个轨道状态和 36 个 STM 元素（6×6），共 42 维。
TEST_F(HPOPSTMTest, STMEquationDimension)
{
    HPOPEquation equation;
    HPOPForceModel forcemodel;
    forcemodel.gravity().model_ = "JGM3";
    forcemodel.gravity().maxDegree_ = 0;
    forcemodel.gravity().maxOrder_ = 0;
    forcemodel.useSTM(true);

    errc_t err = equation.setForceModel(forcemodel);
    ASSERT_EQ(err, eNoError);

    err = equation.initialize();
    ASSERT_EQ(err, eNoError);

    int dim = equation.getDimension();
    // 6 (pos + vel) + 36 (6×6 STM) = 42
    EXPECT_EQ(dim, 42);
}

/// @brief 测试纯二体引力 STM 预报
/// @details
/// 使用二体引力（degree=0），24 小时预报。
/// 验证：
///   1. 预报不报错
///   2. STM 行列式接近 1（二体问题 Liouville 定理）
///   3. STM 上左 3×3 块与上右 3×3 块具有特定关系
TEST_F(HPOPSTMTest, TwoBodySTM)
{
    HPOPForceModel forcemodel;
    forcemodel.gravity().model_ = "JGM3";
    forcemodel.gravity().maxDegree_ = 0;
    forcemodel.gravity().maxOrder_ = 0;
    forcemodel.useSTM(true);

    HPOP propagator;
    errc_t err = propagator.setForceModel(forcemodel);
    ASSERT_EQ(err, eNoError);

    auto start = TimePoint::FromUTC(2026, 1, 20, 0, 0, 0);
    auto end   = TimePoint::FromUTC(2026, 1, 21, 0, 0, 0);

    CartState state;
    state.position() = Vector3d{6678137, 0, 0};
    state.velocity() = Vector3d{0, 6789.53029, 3686.414173};

    Matrix6d stm = Matrix6d{};
    // 初始化 STM = I
    for (int i = 0; i < 6; ++i)
        stm(i, i) = 1.0;

    err = propagator.propagate(start, end, state, stm);
    EXPECT_EQ(err, eNoError);

    // 输出最终状态
    printf("pos:  %s\n", state.position().toString().c_str());
    printf("vel:  %s\n", state.velocity().toString().c_str());

    // 验证轨道状态与无 STM 预报一致
    EXPECT_NEAR(state.position()[0],  5596646.758676, 1e-4);
    EXPECT_NEAR(state.position()[1], -3201966.117956, 1e-4);
    EXPECT_NEAR(state.position()[2],  -1738525.75573, 1e-4);
    EXPECT_NEAR(state.velocity()[0],  4215.063570733, 1e-7);
    EXPECT_NEAR(state.velocity()[1],  5690.000472941, 1e-7);
    EXPECT_NEAR(state.velocity()[2],  3089.418191229, 1e-7);

    // STM 基本性质验证
    // Φ(0) = I, 经过 1 天后 Φ 应有显著非零元素
    double maxOffDiag = 0.0;
    for (int i = 0; i < 6; ++i)
        for (int j = 0; j < 6; ++j)
            if (i != j)
                maxOffDiag = std::max(maxOffDiag, std::abs(stm(i, j)));
    EXPECT_GT(maxOffDiag, 1e-6);  // 非对角元应有非零值
}

/// @brief 测试 STM 初始化
/// @details
/// 验证 STM(0) = I —— 即零时长预报时 STM 应保持单位矩阵。
/// 这是 dΦ/dt = A·Φ 中 Φ(0)=I 初始条件的直接检验。
TEST_F(HPOPSTMTest, STMInitialIdentity)
{
    HPOPForceModel forcemodel;
    forcemodel.gravity().model_ = "JGM3";
    forcemodel.gravity().maxDegree_ = 0;
    forcemodel.gravity().maxOrder_ = 0;
    forcemodel.useSTM(true);

    HPOP propagator;
    errc_t err = propagator.setForceModel(forcemodel);
    ASSERT_EQ(err, eNoError);

    auto start = TimePoint::FromUTC(2026, 1, 20, 0, 0, 0);
    auto end   = start;  // 零时长

    CartState state;
    state.position() = Vector3d{6678137, 0, 0};
    state.velocity() = Vector3d{0, 6789.53029, 3686.414173};

    Matrix6d stm = Matrix6d{};
    for (int i = 0; i < 6; ++i)
        stm(i, i) = 1.0;

    err = propagator.propagate(start, end, state, stm);
    EXPECT_EQ(err, eNoError);

    // 零时长预报后 STM 应仍为单位矩阵
    for (int i = 0; i < 6; ++i)
    {
        for (int j = 0; j < 6; ++j)
        {
            double expected = (i == j) ? 1.0 : 0.0;
            EXPECT_NEAR(stm(i, j), expected, 1e-15);
        }
    }
}


GTEST_MAIN()


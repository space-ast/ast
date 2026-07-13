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
#include "ast/Literals.hpp"

AST_USING_NAMESPACE

using namespace _AST literals;

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

TEST_F(HPOPSTMTest, Gravity_2_2)
{
    HPOPForceModel forcemodel;
    forcemodel.gravity().model_ = "JGM3";
    forcemodel.gravity().maxDegree_ = 2;
    forcemodel.gravity().maxOrder_ = 2;
    forcemodel.useSTM(true);

    HPOP propagator;
    errc_t err = propagator.setForceModel(forcemodel);
    ASSERT_EQ(err, eNoError);

    auto start = TimePoint::FromUTC(2026, 1, 20, 4, 0, 0);
    auto end   = TimePoint::FromUTC(2026, 1, 21, 4, 0, 0);

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

    CartState stateEpected{
        6307.2977033946817755_km, -2085.5098497799403958_km, -674.8981218419083916_km,
        2.4945672065712547_km_s, 6.3651850806138004_km_s, 3.6002197062840202_km_s  
    };

    // 验证轨道状态与无 STM 预报一致
    EXPECT_NEAR(state.position()[0],  stateEpected.position()[0], 1e-4);
    EXPECT_NEAR(state.position()[1],  stateEpected.position()[1], 1e-4);
    EXPECT_NEAR(state.position()[2],  stateEpected.position()[2], 1e-4);
    EXPECT_NEAR(state.velocity()[0],  stateEpected.velocity()[0], 1e-7);
    EXPECT_NEAR(state.velocity()[1],  stateEpected.velocity()[1], 1e-7);
    EXPECT_NEAR(state.velocity()[2],  stateEpected.velocity()[2], 1e-7);

    // STM 基本性质验证
    // Φ(0) = I, 经过 1 天后 Φ 应有显著非零元素
    double maxOffDiag = 0.0;
    for (int i = 0; i < 6; ++i)
        for (int j = 0; j < 6; ++j)
            if (i != j)
                maxOffDiag = std::max(maxOffDiag, std::abs(stm(i, j)));
    EXPECT_GT(maxOffDiag, 1e-6);  // 非对角元应有非零值

    Matrix6d stmExpected = {
          -96.2201791396717567,                                                      
           -0.0829285780355024,                                                      
           -0.1154387649050529,                                                      
         -388.4227809013792694,                                                      
       -73812.4652119806414703,                                                      
       -40141.3323786627879599,                                                      
         -248.4085044793935992,                                                      
            0.9273130108072624,                                                      
           -0.0248484846796194,                                                      
          -16.1436712675956571,                                                      
      -188686.7645276654220652,                                                      
      -102537.8436198902491014,                                                      
         -140.7014024666196406,                                                      
           -0.0374823192318031,                                                      
            0.9551708313835999,                                                      
          -36.2332815815308749,                                                      
      -106810.8890192959952401,                                                      
       -58320.2237313301811810,                                                      
            0.3292395594244089,                                                      
            0.0002309089265001,                                                      
            0.0001118069006597,                                                      
            1.1635257558824761,                                                      
          250.2520164096415556,                                                      
          136.0762980904108019,                                                      
           -0.1089314591801581,                                                      
            0.0003741153148241,                                                      
            0.0000696868310220,                                                      
            0.0008837550503261,                                                      
          -81.8247955569398897,                                                      
          -45.0384714802013590,                                                      
           -0.0354365717070860,                                                      
            0.0000841296436871,                                                      
            0.0002919610171281,                                                      
            0.0829232627833056,                                                      
          -26.9384062867235130,                                                      
          -13.6849919801990172,
    };
    for(int i=0;i<6;i++)
    {
        for(int j=0;j<6;j++)
        {
            EXPECT_NEAR(stm(i,j),stmExpected(i,j),1e-5);
        }
    }
}


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

    auto start = TimePoint::FromUTC(2026, 1, 20, 4, 0, 0);
    auto end   = TimePoint::FromUTC(2026, 1, 21, 4, 0, 0);

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

    Matrix6d stmExpected = {
         -163.2217041709085947,                                                      
           -0.0776476589950642,                                                      
           -0.0421592390624621,                                                      
         -547.9770940091890452,                                                      
      -124976.8673685407411540,                                                      
       -67856.9025375778583111,                                                      
         -222.2080227335862332,                                                      
            0.8583101283405219,                                                      
            0.0109975393688116,                                                      
           19.9226324334798832,                                                      
      -169212.5778521567408461,                                                      
       -91618.8739607787429122,                                                      
         -120.6491125852830351,                                                      
            0.0109975393684747,                                                      
            0.8440263572256796,                                                      
           10.8171068438221987,                                                      
       -91618.8739607931056526,                                                      
       -50216.5932390029265662,                                                      
            0.2911497584310985,                                                      
            0.0004406111523952,                                                      
            0.0002392323367878,                                                      
            1.2714373900993365,                                                      
          221.9750800448428834,                                                      
          120.5226350245791735,                                                      
           -0.1667696609586146,                                                      
            0.0004732877340226,                                                      
           -0.0000857249352145,                                                      
           -0.1552953181096431,                                                      
         -126.1829533140568316,                                                      
          -68.9667806140541302,                                                      
           -0.0905485380468411,                                                      
           -0.0000857249352148,                                                      
            0.0005846285815008,                                                      
           -0.0843184781903268,                                                      
          -68.9667806140654136,                                                      
          -36.6078514820699326,
    };
    for(int i=0;i<6;i++)
    {
        for(int j=0;j<6;j++)
        {
            EXPECT_NEAR(stm(i,j),stmExpected(i,j),3e-2);
        }
    }
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


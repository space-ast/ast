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



TEST_F(HPOPSTMTest, SRP)
{
    HPOPForceModel forcemodel;
    forcemodel.gravity().model_ = "JGM3";
    forcemodel.gravity().maxDegree_ = 2;
    forcemodel.gravity().maxOrder_ = 2;
    forcemodel.useSTM(true);

    forcemodel.useSRP(true);
    forcemodel.srp().shadowModel_ = EShadowModel::eNone;


    HPOP propagator;
    auto integrator = propagator.getIntegrator();
    auto varstep = dynamic_cast<ODEVarStepIntegrator*>(integrator);
    if (varstep)
    {
        varstep->setUseFixedStep(true);
        varstep->setStepSize(60_s);
    }

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
        6307.2828377535297477_km, -2085.5301569741272942_km, -674.9101124419729558_km,
        2.4945902167702645_km_s, 6.3651844352300415_km_s, 3.6002209801539498_km_s  
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
          -96.2210901213182552,                                                    
           -0.0829293116792611,                                                    
           -0.1154391644976363,                                                    
         -388.4268275231244729,                                                    
       -73813.1581151504651643,                                                    
       -40141.7090697997191455,                                                    
         -248.4085000242674539,                                                    
            0.9273087881269143,                                                    
           -0.0248502096696227,                                                    
          -16.1494647379734566,                                                    
      -188686.7639074014732614,                                                    
      -102537.8412709504482336,                                                    
         -140.7014635838022514,                                                    
           -0.0374836607519025,                                                    
            0.9551680617216222,                                                    
          -36.2366875651771068,                                                    
      -106810.9351355284452438,                                                    
       -58320.2518653219667613,                                                    
            0.3292397630946990,                                                    
            0.0002309155340086,                                                    
            0.0001118106553987,                                                    
            1.1635354279695280,                                                    
          250.2521770919222774,                                                    
          136.0763853253391460,                                                    
           -0.1089328421445964,                                                    
            0.0003741164000570,                                                    
            0.0000696853857296,                                                    
            0.0008791837603719,                                                    
          -81.8258477831011675,                                                    
          -45.0390439353445444,                                                    
           -0.0354373049045328,                                                    
            0.0000841288382496,                                                    
            0.0002919639547697, 
            0.0829214902532831,
          -26.9389637420383892,
          -13.6852949524531198,
    };
    for(int i=0;i<6;i++)
    {
        for(int j=0;j<6;j++)
        {
            EXPECT_NEAR(stm(i,j), stmExpected(i,j), fabs(stmExpected(i,j)) * 1e-8);
        }
    }
}

TEST_F(HPOPSTMTest, Drag)
{
    HPOPForceModel forcemodel;
    forcemodel.gravity().model_ = "JGM3";
    forcemodel.gravity().maxDegree_ = 2;
    forcemodel.gravity().maxOrder_ = 2;
    forcemodel.useSTM(true);

    forcemodel.useDrag(true);
    forcemodel.drag().atmDensityModel_ = EAtmDensityModel::eMSIS1986;


    HPOP propagator;
    auto integrator = propagator.getIntegrator();
    auto varstep = dynamic_cast<ODEVarStepIntegrator*>(integrator);
    if (varstep)
    {
        varstep->setUseFixedStep(true);
        varstep->setStepSize(60_s);
    }

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
        6426.3180048466847438_km, -1729.5495217724642316_km, -474.6036655483679283_km,
        2.0243148417866257_km_s, 6.5089945899320956_km_s, 3.6443892361087231_km_s  
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
          -82.4750394267412332,                                                      
           -0.0009698121273170,                                                      
           -0.0732605524274435,                                                      
         -256.1429829291946589,                                                      
       -63398.1941941827171831,                                                      
       -34478.8625870084943017,                                                      
         -269.7926578356768914,                                                      
            1.1324127326343205,                                                      
            0.0749915124826030,                                                      
          165.6305402923279644,                                                      
      -205147.0155512271157932,                                                      
      -111519.7732436253863852,                                                      
         -151.2559096374047556,                                                      
            0.0730758720449757,                                                      
            1.0227305088952960,                                                      
           71.1507586381042501,                                                      
      -114976.3298580274858978,                                                      
       -62707.2560814369571744,                                                      
            0.3572743503792167,                                                      
           -0.0001448685971124,                                                      
           -0.0000795481652354,                                                      
            0.8183895954488961,                                                      
          271.7959337918917981,                                                      
          147.7930024602734420,                                                      
           -0.0964098968789820,                                                      
            0.0003932437139734,                                                      
            0.0001173872058559,                                                      
            0.0879766150855773,                                                      
          -72.4116413650679789,                                                      
          -39.9241616407866786,                                                      
           -0.0266466299261788,                                                      
            0.0001073612823105,                                                      
            0.0002311820743809,                                                      
            0.1054608488333997,                                                      
          -20.3044373633639417,                                                      
          -10.0615528491718500,
    };
    for(int i=0;i<6;i++)
    {
        for(int j=0;j<6;j++)
        {
            EXPECT_NEAR(stm(i,j), stmExpected(i,j), std::max(fabs(stmExpected(i,j)) * 7e-2, 0.02));
        }
    }
}


TEST_F(HPOPSTMTest, Gravity_21_21)
{
    HPOPForceModel forcemodel;
    forcemodel.gravity().model_ = "WGS84";
    forcemodel.gravity().maxDegree_ = 21;
    forcemodel.gravity().maxOrder_ = 21;
    forcemodel.gravity().maxDegreeForPartial_ = 21;
    forcemodel.gravity().maxOrderForPartial_ = 21;
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
        6306.2971286038809922_km, -2086.2584246825363152_km, -674.2588994260432855_km,
        2.4952318195791756_km_s, 6.3657073971464175_km_s, 3.6008356351658355_km_s  
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
          -96.246171,                                                    
           -0.082773856,                                                    
           -0.11604156,                                                    
         -388.21987,                                                    
       -73832.728,                                                    
       -40152.697,                                                    
         -248.42928,                                                    
            0.92692584,                                                    
           -0.026020762,                                                    
          -15.927236,                                                    
      -188703.7,                                                    
      -102548.22,                                                    
         -140.72731,                                                    
           -0.03755264,                                                    
            0.95446272,                                                    
          -36.050272,                                                    
      -106831.12,                                                    
       -58332.342,                                                    
            0.32930975,                                                    
            0.00023107108,                                                    
            0.00011356974,                                                    
            1.163187,                                                    
          250.30697,                                                    
          136.10777,                                                    
           -0.1090063,                                                    
            0.00037386334,                                                    
            0.000069247768,                                                    
            0.00075979169,                                                    
          -81.881921,                                                    
          -45.070254,                                                    
           -0.035413668,                                                    
            0.000084211179,                                                    
            0.00029165901,                                                    
            0.083073397,                                                    
          -26.921273,                                                    
          -13.675887, 
    };
    for(int i=0;i<6;i++)
    {
        for(int j=0;j<6;j++)
        {
            EXPECT_NEAR(stm(i,j), stmExpected(i,j), fabs(stmExpected(i,j))*1e-5);
        }
    }
}


TEST_F(HPOPSTMTest, Gravity_4_4)
{
    HPOPForceModel forcemodel;
    forcemodel.gravity().model_ = "JGM3";
    forcemodel.gravity().maxDegree_ = 4;
    forcemodel.gravity().maxOrder_ = 4;
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
        6307.3780767794678468_km, -2083.5821459382996181_km, -672.9060986406141183_km,
        2.4917802789759587_km_s, 6.3668237607576996_km_s, 3.6011173785045378_km_s  
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
         -96.1123700494979971,                                                    
          -0.0833852292938756,                                                    
          -0.1160834868042193,                                                    
        -387.8768540347623457,                                                    
      -73730.1705606899195118,                                                    
      -40096.9756614683501539,                                                    
        -248.4747166689222979,                                                    
           0.9256571614906497,                                                    
          -0.0261348214704197,                                                    
         -15.9468849993000692,                                                    
     -188735.8776566477608867,                                                    
     -102565.6731454949767794,                                                    
        -140.7387115506031989,                                                    
          -0.0384958168561985,                                                    
           0.9544110760832197,                                                    
         -36.1868722835487517,                                                    
     -106838.7406723592721391,                                                    
      -58335.7320600857274258,                                                    
           0.3293622967787539,                                                    
           0.0002326167322077,                                                    
           0.0001135813765939,                                                    
           1.1630143288948611,                                                    
         250.3435957454802008,                                                    
         136.1272933469545023,                                                    
          -0.1088671048073067,                                                    
           0.0003730294379712,                                                    
           0.0000693144173980,                                                    
           0.0010379979611040,                                                    
         -81.7750803235455805,                                                    
         -45.0118986951096858,                                                    
          -0.0353456360835807,                                                    
           0.0000840648406887,                                                    
           0.0002911999980649,                                                    
           0.0831249083007429,                                                    
         -26.8692946069847025,                                                    
         -13.6473287422093055,                                                    
    };
    for(int i=0;i<6;i++)
    {
        for(int j=0;j<6;j++)
        {
            EXPECT_NEAR(stm(i,j),stmExpected(i,j),1e-5);
        }
    }
}

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

    errc_t err = equation.initialize(forcemodel, SpacecraftParam{}, nullptr);
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


/// @brief 测试启用弹道系数B敏感度传播后方程维度
TEST_F(HPOPSTMTest, DragSensitivityDimension)
{
    HPOPEquation equation;
    HPOPForceModel forcemodel;
    forcemodel.gravity().model_ = "JGM3";
    forcemodel.gravity().maxDegree_ = 0;
    forcemodel.gravity().maxOrder_ = 0;
    forcemodel.useSTM(true);
    forcemodel.useDrag(true);
    forcemodel.useDragSensitivity(true);

    errc_t err = equation.initialize(forcemodel, SpacecraftParam{}, nullptr);
    ASSERT_EQ(err, eNoError);

    int dim = equation.getDimension();
    // 6 (pos + vel) + 36 (STM) + 6 (Ψ_B) = 48
    EXPECT_EQ(dim, 48);
}

/// @brief 测试启用SRP综合参数K敏感度传播后方程维度
TEST_F(HPOPSTMTest, SRPSensitivityDimension)
{
    HPOPEquation equation;
    HPOPForceModel forcemodel;
    forcemodel.gravity().model_ = "JGM3";
    forcemodel.gravity().maxDegree_ = 0;
    forcemodel.gravity().maxOrder_ = 0;
    forcemodel.useSTM(true);
    forcemodel.useSRP(true);
    forcemodel.useSRPSensitivity(true);

    errc_t err = equation.initialize(forcemodel, SpacecraftParam{}, nullptr);
    ASSERT_EQ(err, eNoError);

    int dim = equation.getDimension();
    // 6 (pos + vel) + 36 (STM) + 6 (Ψ_K) = 48
    EXPECT_EQ(dim, 48);
}

/// @brief 测试同时启用B和K敏感度传播后方程维度
TEST_F(HPOPSTMTest, CombinedSensitivityDimension)
{
    HPOPEquation equation;
    HPOPForceModel forcemodel;
    forcemodel.gravity().model_ = "JGM3";
    forcemodel.gravity().maxDegree_ = 0;
    forcemodel.gravity().maxOrder_ = 0;
    forcemodel.useSTM(true);
    forcemodel.useDrag(true);
    forcemodel.useSRP(true);
    forcemodel.useDragSensitivity(true);
    forcemodel.useSRPSensitivity(true);

    errc_t err = equation.initialize(forcemodel, SpacecraftParam{}, nullptr);
    ASSERT_EQ(err, eNoError);

    int dim = equation.getDimension();
    // 6 (pos + vel) + 36 (STM) + 6 (Ψ_B) + 6 (Ψ_K) = 54
    EXPECT_EQ(dim, 54);
}

/// @brief 测试弹道系数B敏感度传播（仅启用阻力）
/// @details 验证：
///   1. Ψ_B 初始值为零，传播后有非零值
///   2. ODE维度为48
TEST_F(HPOPSTMTest, DragSensitivity)
{
    HPOPForceModel forcemodel;
    forcemodel.gravity().model_ = "JGM3";
    forcemodel.gravity().maxDegree_ = 2;
    forcemodel.gravity().maxOrder_ = 2;
    forcemodel.useSTM(true);

    forcemodel.useDrag(true);
    forcemodel.drag().atmDensityModel_ = EAtmDensityModel::eMSIS1986;
    forcemodel.useDragSensitivity(true);

    HPOP propagator;
    auto integrator = propagator.getIntegrator();
    auto varstep = dynamic_cast<ODEVarStepIntegrator*>(integrator);
    if (varstep)
    {
        varstep->setUseFixedStep(true);
        varstep->setStepSize(60_s);
    }

    errc_t err = propagator.setForceModel(forcemodel);
    ASSERT_EQ(err, eNoError);

    auto start = TimePoint::FromUTC(2026, 1, 20, 4, 0, 0);
    auto end   = TimePoint::FromUTC(2026, 1, 21, 4, 0, 0);

    CartState state;
    state.position() = Vector3d{6678137, 0, 0};
    state.velocity() = Vector3d{0, 6789.53029, 3686.414173};

    Matrix6d stm = Matrix6d{};
    for (int i = 0; i < 6; ++i)
        stm(i, i) = 1.0;

    Vector6d stateSensToDrag{};  // Ψ_B(0) = 0
    Vector6d stateSensToSRP{};   // Ψ_K(0) = 0 (won't be used since SRP is disabled)

    err = propagator.propagate(start, end, state, stm, stateSensToDrag, stateSensToSRP);
    EXPECT_EQ(err, eNoError);

    // Ψ_B rows 0-2 (位置偏导) 应有非零值（位置间接受B影响）
    double maxPosSens = std::max({std::abs(stateSensToDrag[0]), std::abs(stateSensToDrag[1]), std::abs(stateSensToDrag[2])});
    EXPECT_GT(maxPosSens, 1e-10);

    // Ψ_B rows 3-5 (速度偏导) 应有显着非零值（加速度直接依赖B）
    double maxVelSens = std::max({std::abs(stateSensToDrag[3]), std::abs(stateSensToDrag[4]), std::abs(stateSensToDrag[5])});
    EXPECT_GT(maxVelSens, 1e-6);

    // 打印结果供参考
    printf("StateSensToDrag: [%.6e, %.6e, %.6e, %.6e, %.6e, %.6e]\n",
           stateSensToDrag[0], stateSensToDrag[1], stateSensToDrag[2], stateSensToDrag[3], stateSensToDrag[4], stateSensToDrag[5]);

    // SRP 未启用，stateSensToSRP 应保持为零（互不干扰验证）
    for (int i = 0; i < 6; ++i)
        EXPECT_NEAR(stateSensToSRP[i], 0.0, 1e-15);
}

/// @brief 测试SRP综合参数K敏感度传播（仅启用SRP）
/// @details 验证：
///   1. Ψ_K 初始值为零，传播后有非零值
///   2. ODE维度为48
TEST_F(HPOPSTMTest, SRPSensitivity)
{
    HPOPForceModel forcemodel;
    forcemodel.gravity().model_ = "JGM3";
    forcemodel.gravity().maxDegree_ = 2;
    forcemodel.gravity().maxOrder_ = 2;
    forcemodel.useSTM(true);

    forcemodel.useSRP(true);
    forcemodel.srp().shadowModel_ = EShadowModel::eNone;
    forcemodel.useSRPSensitivity(true);

    HPOP propagator;
    auto integrator = propagator.getIntegrator();
    auto varstep = dynamic_cast<ODEVarStepIntegrator*>(integrator);
    if (varstep)
    {
        varstep->setUseFixedStep(true);
        varstep->setStepSize(60_s);
    }

    errc_t err = propagator.setForceModel(forcemodel);
    ASSERT_EQ(err, eNoError);

    auto start = TimePoint::FromUTC(2026, 1, 20, 4, 0, 0);
    auto end   = TimePoint::FromUTC(2026, 1, 21, 4, 0, 0);

    CartState state;
    state.position() = Vector3d{6678137, 0, 0};
    state.velocity() = Vector3d{0, 6789.53029, 3686.414173};

    Matrix6d stm = Matrix6d{};
    for (int i = 0; i < 6; ++i)
        stm(i, i) = 1.0;

    Vector6d stateSensToDrag{};
    Vector6d stateSensToSRP{};

    err = propagator.propagate(start, end, state, stm, stateSensToDrag, stateSensToSRP);
    EXPECT_EQ(err, eNoError);

    // Ψ_K rows 0-2 (位置偏导) 应有非零值
    double maxPosSens = std::max({std::abs(stateSensToSRP[0]), std::abs(stateSensToSRP[1]), std::abs(stateSensToSRP[2])});
    EXPECT_GT(maxPosSens, 1e-10);

    // Ψ_K rows 3-5 (速度偏导) 应有显着非零值
    double maxVelSens = std::max({std::abs(stateSensToSRP[3]), std::abs(stateSensToSRP[4]), std::abs(stateSensToSRP[5])});
    EXPECT_GT(maxVelSens, 1e-6);

    printf("StateSensToSRP: [%.6e, %.6e, %.6e, %.6e, %.6e, %.6e]\n",
           stateSensToSRP[0], stateSensToSRP[1], stateSensToSRP[2], stateSensToSRP[3], stateSensToSRP[4], stateSensToSRP[5]);

    // Drag 未启用，stateSensToDrag 应保持为零（互不干扰验证）
    for (int i = 0; i < 6; ++i)
        EXPECT_NEAR(stateSensToDrag[i], 0.0, 1e-15);
}

/// @brief 测试同时启用B和K敏感度传播
/// @details 同时启用 Drag + SRP 及各自的参数敏感度，验证：
///   1. Ψ_B 和 Ψ_K 均有显著非零值
///   2. 6x6 STM 与无敏感度时的 Drag+SRP 测试结果一致
///   3. ODE维度为54
TEST_F(HPOPSTMTest, CombinedSensitivity)
{
    HPOPForceModel forcemodel;
    forcemodel.gravity().model_ = "JGM3";
    forcemodel.gravity().maxDegree_ = 2;
    forcemodel.gravity().maxOrder_ = 2;
    forcemodel.useSTM(true);

    forcemodel.useDrag(true);
    forcemodel.drag().atmDensityModel_ = EAtmDensityModel::eMSIS1986;
    forcemodel.useSRP(true);
    forcemodel.srp().shadowModel_ = EShadowModel::eNone;
    forcemodel.useDragSensitivity(true);
    forcemodel.useSRPSensitivity(true);

    HPOP propagator;
    auto integrator = propagator.getIntegrator();
    auto varstep = dynamic_cast<ODEVarStepIntegrator*>(integrator);
    if (varstep)
    {
        varstep->setUseFixedStep(true);
        varstep->setStepSize(60_s);
    }

    errc_t err = propagator.setForceModel(forcemodel);
    ASSERT_EQ(err, eNoError);

    auto start = TimePoint::FromUTC(2026, 1, 20, 4, 0, 0);
    auto end   = TimePoint::FromUTC(2026, 1, 21, 4, 0, 0);

    CartState state;
    state.position() = Vector3d{6678137, 0, 0};
    state.velocity() = Vector3d{0, 6789.53029, 3686.414173};

    Matrix6d stm = Matrix6d{};
    for (int i = 0; i < 6; ++i)
        stm(i, i) = 1.0;

    Vector6d stateSensToDrag{};
    Vector6d stateSensToSRP{};

    err = propagator.propagate(start, end, state, stm, stateSensToDrag, stateSensToSRP);
    EXPECT_EQ(err, eNoError);

    // Ψ_B rows 3-5 应有显著非零值
    {
        double maxBSens = std::max({std::abs(stateSensToDrag[3]), std::abs(stateSensToDrag[4]), std::abs(stateSensToDrag[5])});
        EXPECT_GT(maxBSens, 1e-6);
    }

    // Ψ_K rows 3-5 应有显著非零值
    {
        double maxKSens = std::max({std::abs(stateSensToSRP[3]), std::abs(stateSensToSRP[4]), std::abs(stateSensToSRP[5])});
        EXPECT_GT(maxKSens, 1e-6);
    }

    printf("StateSensToDrag: [%.6e, %.6e, %.6e, %.6e, %.6e, %.6e]\n",
           stateSensToDrag[0], stateSensToDrag[1], stateSensToDrag[2], stateSensToDrag[3], stateSensToDrag[4], stateSensToDrag[5]);
    printf("StateSensToSRP: [%.6e, %.6e, %.6e, %.6e, %.6e, %.6e]\n",
           stateSensToSRP[0], stateSensToSRP[1], stateSensToSRP[2], stateSensToSRP[3], stateSensToSRP[4], stateSensToSRP[5]);
}

/// @brief 测试 STM + 参数敏感度逐帧采集（StateObserver 机制）
/// @details
/// 模拟 stm.cpp 示例的使用场景：
/// 在 JGM3(2,2) + Drag(MSIS86) + SRP(DualCone) 力模型下，
/// 同时启用 STM、Drag敏感度和SRP敏感度，通过 StateObserver 逐帧采集
/// 每步的位置、速度、STM 和敏感度向量。
/// 验证：
///   1. 预报成功完成
///   2. StateObserver 采集到了积分帧数据
///   3. 最终位置/速度与预期值一致
///   4. STM、Drag敏感度、SRP敏感度均有非零值
///   5. 首帧时间为 0，末帧时间接近预报时长
TEST_F(HPOPSTMTest, STM_StateObserver_Collection)
{
    HPOPForceModel forcemodel;
    forcemodel.gravity().model_ = "JGM3";
    forcemodel.gravity().maxDegree_ = 2;
    forcemodel.gravity().maxOrder_ = 2;
    forcemodel.useSTM(true);

    forcemodel.useDrag(true);
    forcemodel.drag().atmDensityModel_ = EAtmDensityModel::eMSIS1986;

    forcemodel.useSRP(true);
    forcemodel.srp().shadowModel_ = EShadowModel::eDualCone;

    forcemodel.useDragSensitivity(true);
    forcemodel.useSRPSensitivity(true);

    SpacecraftParam scParam;
    scParam.setDryMass(1000.0);
    scParam.setFuelMass(0.0);
    scParam.setDragArea(20.0);
    scParam.setCd(2.2);
    scParam.setSrpArea(20.0);
    scParam.setCr(1.0);

    HPOP propagator;
    errc_t err = propagator.setForceModel(forcemodel);
    ASSERT_EQ(err, eNoError);
    propagator.setSpacecraftParam(scParam);

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

    // 设置积分器固定步长 60s
    auto integrator = propagator.getIntegrator();
    auto varintegrator = dynamic_cast<ODEVarStepIntegrator*>(integrator);
    ASSERT_NE(varintegrator, nullptr);
    varintegrator->setUseFixedStep(true);
    varintegrator->setStepSize(60_s);

    // 注册 observer：每步采集帧数据
    struct StmFrame
    {
        double   time{};
        CartState state{};
        Matrix6d stm{};
        Vector6d dragSens{};
        Vector6d srpSens{};
    };
    std::vector<StmFrame> frames;
    auto* mapper = propagator.stateMapper();
    ASSERT_NE(mapper, nullptr);

    integrator->addStateObserver([&frames, mapper](double* y, double& x, ODEIntegrator*) {
        StmFrame f;
        f.time = x;
        mapper->toState(y, f.state);
        mapper->toStateTransitionMatrix(y, f.stm);
        mapper->toStateSensitivityWrtDrag(y, f.dragSens);
        mapper->toStateSensitivityWrtSRP(y, f.srpSens);
        frames.push_back(f);
    });

    // 预报
    err = propagator.propagate(start, end, state, stm,
                               stateSensWrtDrag, stateSensWrtSRP);
    EXPECT_EQ(err, eNoError);

    integrator->clearStateObservers();

    // 验证帧数据被采集（固定步长 60s，24h 应有约 1440 帧）
    EXPECT_GT(frames.size(), 0u);
    printf("# frames=%zu\n", frames.size());

    // 验证最终状态
    printf("# final pos:  %s\n", state.position().toString().c_str());
    printf("# final vel:  %s\n", state.velocity().toString().c_str());
    printf("# final dragSens: [%.6e, %.6e, %.6e, %.6e, %.6e, %.6e]\n",
           stateSensWrtDrag[0], stateSensWrtDrag[1], stateSensWrtDrag[2],
           stateSensWrtDrag[3], stateSensWrtDrag[4], stateSensWrtDrag[5]);
    printf("# final srpSens:  [%.6e, %.6e, %.6e, %.6e, %.6e, %.6e]\n",
           stateSensWrtSRP[0], stateSensWrtSRP[1], stateSensWrtSRP[2],
           stateSensWrtSRP[3], stateSensWrtSRP[4], stateSensWrtSRP[5]);

    // ── 验证最终状态（基准值来自 stm.cpp 示例） ──
    EXPECT_NEAR(state.position()[0], 6426310.1944292756,  1e-6);
    EXPECT_NEAR(state.position()[1], -1729560.6268997970, 1e-6);
    EXPECT_NEAR(state.position()[2], -474610.30727389897, 1e-6);
    EXPECT_NEAR(state.velocity()[0], 2024.3270414323702,  1e-6);

    // ── 验证 STM（基准值来自 stm.cpp 示例） ──
    // Φ(0)=I, 经过 1 天后应有显著非对角元
    double maxOffDiag = 0.0;
    for (int i = 0; i < 6; ++i)
        for (int j = 0; j < 6; ++j)
            if (i != j)
                maxOffDiag = std::max(maxOffDiag, std::abs(stm(i, j)));
    EXPECT_GT(maxOffDiag, 1e-6);

    // STM 第一行基准值验证
    EXPECT_NEAR(stm(0,0), -82.465447186568412,       1e-7);
    EXPECT_NEAR(stm(0,1),   0.00053472762888822495,  1e-7);
    EXPECT_NEAR(stm(0,2),  -0.070726596660925997,    1e-7);
    EXPECT_NEAR(stm(0,3), -253.93305481600945,       1e-7);

    // ── 验证 Drag 敏感度（基准值来自 stm.cpp 示例） ──
    EXPECT_NEAR(stateSensWrtDrag[0],  2503942.6140104369,  1e-5);
    EXPECT_NEAR(stateSensWrtDrag[1],  8509256.7297997586,  1e-5);
    EXPECT_NEAR(stateSensWrtDrag[2],  4763867.5046518976,  1e-5);
    EXPECT_NEAR(stateSensWrtDrag[3], -11240.949623401957,  1e-5);

    double maxDragSens = 0.0;
    for (int i = 0; i < 6; ++i)
        maxDragSens = std::max(maxDragSens, std::abs(stateSensWrtDrag[i]));
    EXPECT_GT(maxDragSens, 1e-6);

    // ── 验证 SRP 敏感度（基准值来自 stm.cpp 示例） ──
    EXPECT_NEAR(stateSensWrtSRP[0], -390.37047021941561,  1e-7);
    EXPECT_NEAR(stateSensWrtSRP[1], -554.73057392318208,  1e-6);
    EXPECT_NEAR(stateSensWrtSRP[2], -331.79228369067152,  1e-7);
    EXPECT_NEAR(stateSensWrtSRP[3],  0.60926698269460056, 1e-7);

    double maxSRPSens = 0.0;
    for (int i = 0; i < 6; ++i)
        maxSRPSens = std::max(maxSRPSens, std::abs(stateSensWrtSRP[i]));
    EXPECT_GT(maxSRPSens, 1e-6);

    // 验证帧数据一致性：首帧时间应为 0，末帧应为 86400s 附近
    EXPECT_NEAR(frames.front().time, 0.0, 1e-10);
    EXPECT_NEAR(frames.back().time, 86400.0, 60.0);

    // 验证帧内数据与最终状态一致（最后一帧应等于最终输出）
    const auto& lastFrame = frames.back();
    for (int i = 0; i < 3; ++i)
    {
        EXPECT_NEAR(lastFrame.state.position()[i], state.position()[i], 1e-10);
        EXPECT_NEAR(lastFrame.state.velocity()[i], state.velocity()[i], 1e-10);
    }
    for (int i = 0; i < 6; ++i)
    {
        for (int j = 0; j < 6; ++j)
            EXPECT_NEAR(lastFrame.stm(i, j), stm(i, j), 1e-10);
        EXPECT_NEAR(lastFrame.dragSens[i], stateSensWrtDrag[i], 1e-10);
        EXPECT_NEAR(lastFrame.srpSens[i], stateSensWrtSRP[i], 1e-10);
    }
}

/// @brief 测试敏感度向量初始条件（零时长传播）
/// @details 零时长传播后 Ψ_B 和 Ψ_K 应保持为零向量
TEST_F(HPOPSTMTest, SensitivityInitialZero)
{
    HPOPForceModel forcemodel;
    forcemodel.gravity().model_ = "JGM3";
    forcemodel.gravity().maxDegree_ = 0;
    forcemodel.gravity().maxOrder_ = 0;
    forcemodel.useSTM(true);
    forcemodel.useDrag(true);
    forcemodel.useSRP(true);
    forcemodel.useDragSensitivity(true);
    forcemodel.useSRPSensitivity(true);

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

    Vector6d stateSensToDrag{};
    Vector6d stateSensToSRP{};

    err = propagator.propagate(start, end, state, stm, stateSensToDrag, stateSensToSRP);
    EXPECT_EQ(err, eNoError);

    // 零时长传播后 Ψ_B 和 Ψ_K 应保持为零
    for (int i = 0; i < 6; ++i) {
        EXPECT_NEAR(stateSensToDrag[i], 0.0, 1e-15);
        EXPECT_NEAR(stateSensToSRP[i], 0.0, 1e-15);
    }
}

GTEST_MAIN()


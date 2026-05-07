///
/// @file      testState.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-15
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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

#include "AstCore/StateCartesian.hpp"
#include "AstCore/StateKeplerian.hpp"
#include "AstCore/RunTime.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/BuiltinAxes.hpp"
#include "AstCore/SOFA.hpp"
#include "AstCore/EOP.hpp"
#include "AstCore/EarthFrame.hpp"
#include "AstUtil/Literals.hpp"
#include "AstUtil/Math.hpp"
#include "AstTest/Test.hpp"

AST_USING_NAMESPACE


class StateTest : public ::testing::Test
{
protected:
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

/// @brief 测试坐标系转换
TEST_F(StateTest, ChangeFrameBasic)
{
    // 测试从ICRF到TOD的转换
    {
        errc_t rc;
        auto cartState = StateCartesian::MakeShared();
        SharedPtr<CelestialBody> earth = aGetEarth();
        
        auto icrfFrame = earth->makeFrame(aAxesICRF());
        auto todFrame = earth->makeFrame(aAxesTOD());
        rc = cartState->setState(CartState{6678137_m, 0.0, 0.0, 0.0, 6789.5_m/s, 3686.4_m/s});
        EXPECT_EQ(rc, 0);
        cartState->setStateEpoch(TimePoint::FromUTC(2026, 3, 15, 0, 0, 0));
        cartState->setFrame(icrfFrame);
        rc = cartState->changeFrame(todFrame);
        EXPECT_EQ(rc, 0);
        CartState state;
        rc = cartState->getState(state);
        EXPECT_EQ(rc, 0);
        CartState stateExpect {
            6677.9993219814050462_km, 39.3297108095184242_km, 17.0885994905810570_km,
            -0.0494184559469364_km/sec, 6.7891934257351290_km/sec, 3.6866333766957053_km/sec
        };
        for(int i = 0; i < 3; i++)
        {
            EXPECT_NEAR(state[i], stateExpect[i], 1e-3);
            EXPECT_NEAR(state[3+i], stateExpect[3+i], 1e-6);
        }
    }
}

/// @brief 测试从TOE到MOE的转换
TEST_F(StateTest, ChangeFrameWithEpoch)
{
    // 测试从TOE到MOE的转换
    {
        errc_t rc;
        auto cartState = StateCartesian::MakeShared();
        SharedPtr<CelestialBody> earth = aGetEarth();
        auto coordEpoch = TimePoint::FromUTC(1980, 12, 27, 0, 0, 0);
        auto stateEpoch = TimePoint::FromUTC(2026, 3, 15, 0, 0, 0);
        auto toe = earth->makeFrameTOE(coordEpoch);
        auto moe = earth->makeFrameMOE(coordEpoch);
        CartState state0{6678137_m, 0.0, 0.0, 0.0, 6789.5_m/s, 3686.4_m/s};
        rc = cartState->setState(state0);
        EXPECT_EQ(rc, 0);
        cartState->setStateEpoch(stateEpoch);
        cartState->setFrame(toe);
        rc = cartState->changeFrame(moe);
        EXPECT_EQ(rc, 0);
        CartState state1;
        rc = cartState->getState(state1);
        EXPECT_EQ(rc, 0);
        printf("state1: %f %f %f %f %f %f\n", state1[0], state1[1], state1[2], state1[3], state1[4], state1[5]);
        CartState stateExpect {
            6678.1369879813173611_km, 0.3675873390982545_km, 0.1593874602611343_km,
            -0.0004616993065655_km/sec, 6.7893791093392464_km/sec, 3.6866226151454065_km/sec
        };
        Rotation rotation;
        aMODToTODTransform(coordEpoch, rotation);
        rotation = rotation.inverse();
        CartState state2;
        state2.pos() = rotation.transformVector(state0.pos());
        state2.vel() = rotation.transformVector(state0.vel());
        printf("state2: %f %f %f %f %f %f\n", state2[0], state2[1], state2[2], state2[3], state2[4], state2[5]);

        for(int i = 0; i < 3; i++)
        {
            EXPECT_NEAR(state1[i], stateExpect[i], 1e-5);
            EXPECT_NEAR(state2[i], stateExpect[i], 1e-5);
            EXPECT_NEAR(state1[3+i], stateExpect[3+i], 1e-8);
            EXPECT_NEAR(state2[3+i], stateExpect[3+i], 1e-8);
        }
    }
    // 测试从TOE到MOD的转换
    {
        errc_t rc;
        auto cartState = StateCartesian::MakeShared();
        SharedPtr<CelestialBody> earth = aGetEarth();
        auto coordEpoch = TimePoint::FromUTC(1980, 12, 27, 0, 0, 0);
        auto stateEpoch = TimePoint::FromUTC(2026, 3, 15, 0, 0, 0);
        auto toe = earth->makeFrameTOE(coordEpoch);
        auto mod = earth->makeFrameMOD();
        CartState state0{6678137_m, 0.0, 0.0, 0.0, 6789.5_m/s, 3686.4_m/s};
        rc = cartState->setState(state0);
        EXPECT_EQ(rc, 0);
        cartState->setStateEpoch(stateEpoch);
        cartState->setFrame(toe);
        rc = cartState->changeFrame(mod);
        EXPECT_EQ(rc, 0);
        CartState state1;
        rc = cartState->getState(state1);
        EXPECT_EQ(rc, 0);
        printf("state1: %f %f %f %f %f %f\n", state1[0], state1[1], state1[2], state1[3], state1[4], state1[5]);
        CartState stateExpect {
            6677.7267929937161171_km, 67.8861414016167259_km, 29.4981475428268247_km,
            -0.0853012283266688_km/sec, 6.7889455414363677_km/sec, 3.6864342318637973_km/sec
        };

        for(int i = 0; i < 3; i++)
        {
            EXPECT_NEAR(state1[i], stateExpect[i], 1e-5);
            EXPECT_NEAR(state1[3+i], stateExpect[3+i], 1e-8);
        }
    }
}


/// @brief 测试开普勒轨道根数状态参数
TEST_F(StateTest, GetKeplerianStateParams)
{
    auto earth = aGetEarth();
    {
        errc_t rc;
        auto keplerianState = StateKeplerian::MakeShared();
        OrbElem orbElem{
            8678137_m, 0.03, 28.5_deg, 12_deg, 21_deg, 23_deg
        };
        auto tp = TimePoint::FromUTC(2026, 3, 15, 0, 0, 0);
        keplerianState->setState(orbElem);
        keplerianState->setFrame(earth->makeFrameMOD());
        keplerianState->setStateEpoch(tp);
        StateCartesian cartesianState{*keplerianState};
        CartState state;
        rc = cartesianState.getState(state);
        CartState stateExpect {
            4865.7657051977957963_km, 6300.1230393824653220_km, 2796.6565762704854023_km, 
            -5.6042661463648802_km/sec, 3.3614623498040133_km/sec, 2.4178896606906020_km/sec
        };
        for(int i = 0; i < 3; i++)
        {
            EXPECT_NEAR(state[i], stateExpect[i], 1e-6);
            EXPECT_NEAR(state[3+i], stateExpect[3+i], 1e-9);
        }
        OrbElem orbElem1;
        keplerianState->getState(orbElem1);
        EXPECT_EQ(rc, 0);
        printf("state: %f %f %f %f %f %f\n", state[0], state[1], state[2], state[3], state[4], state[5]);
        {
            keplerianState->setSizeType(ESizeType::ePeriRad);
            keplerianState->setShapeType(EShapeType::eApoRad);
            keplerianState->setOrientationType(EOrientationType::eLAN);
            keplerianState->setPositionType(EPositionType::eMeanAnomaly);
            array6d element;
            keplerianState->getInnerRepresentationState(element);
            element[3] = aNormalizeAngle0To2Pi(element[3]);
            printf("element: %f %f %f %f %f %f\n", element[0], element[1], element[2], element[3], element[4], element[5]);
            array6d elementExpect{
                8417.79289_km, 8938.48111_km, 28.5_deg, 
                203.2597961691080002_deg, 21_deg, 21.6841152978667751_deg
            };
            for(int i = 0; i < 6; i++)
            {
                EXPECT_NEAR(element[i], elementExpect[i], 1e-8);
            }
        }
        {
            keplerianState->setSizeType(ESizeType::eMeanMotion);
            keplerianState->setShapeType(EShapeType::ePeriAlt);
            keplerianState->setOrientationType(EOrientationType::eLAN);
            keplerianState->setPositionType(EPositionType::eEccAnomaly);
            array6d element;
            keplerianState->getInnerRepresentationState(element);
            element[3] = aNormalizeAngle0To2Pi(element[3]);
            printf("element: %f %f %f %f %f %f\n", element[0], element[1], element[2], element[3], element[4], element[5]);
            array6d elementExpect{
                10.7389793894957375_revs/day, 2039.65589_km, 28.5_deg, 
                203.2597961691080002_deg, 21_deg, 22.3373900619093533_deg
            };
            for(int i = 0; i < 6; i++)
            {
                EXPECT_NEAR(element[i], elementExpect[i], 1e-9);
            }
        }
        {
            keplerianState->setSizeType(ESizeType::ePeriod);
            keplerianState->setShapeType(EShapeType::eEcc);
            keplerianState->setOrientationType(EOrientationType::eRAAN);
            keplerianState->setPositionType(EPositionType::eTimePastAscNode);
            array6d element;
            keplerianState->getInnerRepresentationState(element);
            element[3] = aNormalizeAngle0To2Pi(element[3]);
            printf("element: %f %f %f %f %f %f\n", element[0], element[1], element[2], element[3], element[4], element[5]);
            array6d elementExpect{
                8045.4572884748704382_s, 0.03, 28.5_deg, 
                12_deg, 21_deg, 926.9610895557920003_s
            };
            for(int i = 0; i < 6; i++)
            {
                EXPECT_NEAR(element[i], elementExpect[i], 1e-9);
            }
        }
        {
            keplerianState->setSizeType(ESizeType::eSMA);
            keplerianState->setShapeType(EShapeType::eApoAlt);
            keplerianState->setOrientationType(EOrientationType::eRAAN);
            keplerianState->setPositionType(EPositionType::eTimePastPeri);
            array6d element;
            keplerianState->getInnerRepresentationState(element);
            element[3] = aNormalizeAngle0To2Pi(element[3]);
            printf("element: %f %f %f %f %f %f\n", element[0], element[1], element[2], element[3], element[4], element[5]);
            array6d elementExpect{
                8678.137_km, 2560344.11_m, 28.5_deg, 
                12_deg, 21_deg, 484.6072874093101746_s
            };
            for(int i = 0; i < 6; i++)
            {
                EXPECT_NEAR(element[i], elementExpect[i], 2e-9);
            }
        }
    }
}

TEST_F(StateTest, SetKeplerianState)
{
    auto earth = aGetEarth();
    {
        OrbElem orbElem{
            8678_km, 0.025, 28.5_deg, 
            12_deg, 34_deg, 56_deg
        };
        auto tp = TimePoint::FromUTC(2026, 3, 17, 0, 0, 0.0);
        auto keplerianState = StateKeplerian::MakeShared();
        keplerianState->setState(orbElem);
        keplerianState->setFrame(earth->makeFrameICRF());
        keplerianState->setStateEpoch(tp);
        // test setLAN
        {
            double lan = keplerianState->getLAN();
            lan = aNormalizeAngle0To2Pi(lan);
            printf("lan: %.15g\n", lan);
            EXPECT_NEAR(lan, 205.7881104269659716_deg, 1e-9_deg);

            lan = 123_deg;
            keplerianState->setLAN(lan);
            lan = keplerianState->getLAN();
            lan = aNormalizeAngle0To2Pi(lan);
            printf("lan: %.15g\n", lan);
            EXPECT_NEAR(lan, 123_deg, 1e-3_deg);
            
            double raan = keplerianState->getRAAN();
            raan = aNormalizeAngle0To2Pi(raan);
            printf("raan: %.15g\n", raan/1_deg);
            EXPECT_NEAR(raan, 289.2119814597829759_deg, 1e-3_deg);

        }
        // test setSMA
        {
            keplerianState->setState(orbElem);
            keplerianState->setSizeType(ESizeType::eApoRad);
            keplerianState->setShapeType(EShapeType::ePeriRad);
            keplerianState->setPositionType(EPositionType::eMeanAnomaly);
            keplerianState->setOrientationType(EOrientationType::eLAN);

            double sma1 = 1000000_km;
            double shapeParam1 = keplerianState->getShapeParam();
            double orientationParam1 = keplerianState->getOrientationParam();
            double positionParam1 = keplerianState->getPositionParam();
            
            keplerianState->setSMA(sma1);

            double sma2 = keplerianState->getSMA();
            double shapeParam2 = keplerianState->getShapeParam();
            double orientationParam2 = keplerianState->getOrientationParam();
            double positionParam2 = keplerianState->getPositionParam();

            printf("sma1: %.15g\n", sma1);
            printf("sma2: %.15g\n", sma2);
            printf("shapeParam1: %.15g\n", shapeParam1);
            printf("shapeParam2: %.15g\n", shapeParam2);
            printf("orientationParam1: %.15g\n", orientationParam1);
            printf("orientationParam2: %.15g\n", orientationParam2);
            printf("positionParam1: %.15g\n", positionParam1);
            printf("positionParam2: %.15g\n", positionParam2);

            EXPECT_NEAR(sma1, sma2, 1e-14 * sma1);
            EXPECT_NEAR(shapeParam1, shapeParam2, 1e-14);
            EXPECT_NEAR(orientationParam1, orientationParam2, 1e-5);
            EXPECT_NEAR(positionParam1, positionParam2, 1e-14);
        }
        // test setMeanMotion
        {
            keplerianState->setState(orbElem);
            keplerianState->setSizeType(ESizeType::eApoRad);
            keplerianState->setShapeType(EShapeType::ePeriAlt);
            keplerianState->setPositionType(EPositionType::eTimePastAscNode);
            keplerianState->setOrientationType(EOrientationType::eLAN);

            double meanMotion1 = 9_revs/day;
            double shapeParam1 = keplerianState->getShapeParam();
            double orientationParam1 = keplerianState->getOrientationParam();
            double positionParam1 = keplerianState->getPositionParam();
            
            keplerianState->setMeanMotion(meanMotion1);

            double meanMotion2 = keplerianState->getMeanMotion();
            double shapeParam2 = keplerianState->getShapeParam();
            double orientationParam2 = keplerianState->getOrientationParam();
            double positionParam2 = keplerianState->getPositionParam();
            
            printf("meanMotion1: %.15g\n", meanMotion1);
            printf("meanMotion2: %.15g\n", meanMotion2);
            printf("shapeParam1: %.15g\n", shapeParam1);
            printf("shapeParam2: %.15g\n", shapeParam2);
            printf("orientationParam1: %.15g\n", orientationParam1);
            printf("orientationParam2: %.15g\n", orientationParam2);
            printf("positionParam1: %.15g\n", positionParam1);
            printf("positionParam2: %.15g\n", positionParam2);
            
            EXPECT_NEAR(meanMotion1, meanMotion2, 1e-14 * meanMotion1);
            EXPECT_NEAR(shapeParam1, shapeParam2, 1e-14);
            EXPECT_NEAR(orientationParam1, orientationParam2, 1e-5);
            EXPECT_NEAR(positionParam1, positionParam2, 1e-12);
        }
        // setPeriod
        {
            keplerianState->setState(orbElem);
            keplerianState->setSizeType(ESizeType::eSMA);
            keplerianState->setShapeType(EShapeType::eApoAlt);
            keplerianState->setPositionType(EPositionType::eTimePastPeri);
            keplerianState->setOrientationType(EOrientationType::eLAN);

            double period0 = keplerianState->getPeriod();
            
            double period1 = period0 * 0.8; // 如果半长轴大于远地点半径，则计算出错
            double shapeParam1 = keplerianState->getShapeParam();
            double orientationParam1 = keplerianState->getOrientationParam();
            double positionParam1 = keplerianState->getPositionParam();
            
            keplerianState->setPeriod(period1);

            double period2 = keplerianState->getPeriod();
            double shapeParam2 = keplerianState->getShapeParam();
            double orientationParam2 = keplerianState->getOrientationParam();
            double positionParam2 = keplerianState->getPositionParam();
            
            printf("period1: %.15g\n", period1);
            printf("period2: %.15g\n", period2);
            printf("orientationParam1: %.15g\n", orientationParam1);
            printf("orientationParam2: %.15g\n", orientationParam2);
            printf("shapeParam1: %.15g\n", shapeParam1);
            printf("shapeParam2: %.15g\n", shapeParam2);
            printf("positionParam1: %.15g\n", positionParam1);
            printf("positionParam2: %.15g\n", positionParam2);
            
            EXPECT_NEAR(period1, period2, 1e-14 * period1);
            EXPECT_NEAR(shapeParam1, shapeParam2, 1e-14);
            EXPECT_NEAR(orientationParam1, orientationParam2, 1e-5);
            EXPECT_NEAR(positionParam1, positionParam2, 1e-12);
        }
        // test setPeriRadForSize
        {
            keplerianState->setState(orbElem);
            keplerianState->setSizeType(ESizeType::eSMA);
            keplerianState->setShapeType(EShapeType::eApoAlt);
            keplerianState->setPositionType(EPositionType::eArgLat);
            keplerianState->setOrientationType(EOrientationType::eLAN);

            double apoRad0 = keplerianState->getApoRad();
            double periRad1 = apoRad0 * 0.8;
            double shapeParam1 = keplerianState->getShapeParam();
            double orientationParam1 = keplerianState->getOrientationParam();
            double positionParam1 = keplerianState->getPositionParam();

            keplerianState->setPeriRadForSize(periRad1);

            double periRad2 = keplerianState->getPeriRad();
            double shapeParam2 = keplerianState->getShapeParam();
            double orientationParam2 = keplerianState->getOrientationParam();
            double positionParam2 = keplerianState->getPositionParam();

            printf("periRad1: %.15g\n", periRad1);
            printf("periRad2: %.15g\n", periRad2);
            printf("orientationParam1: %.15g\n", orientationParam1);
            printf("orientationParam2: %.15g\n", orientationParam2);
            printf("shapeParam1: %.15g\n", shapeParam1);
            printf("shapeParam2: %.15g\n", shapeParam2);
            printf("positionParam1: %.15g\n", positionParam1);
            printf("positionParam2: %.15g\n", positionParam2);
            
            EXPECT_NEAR(periRad1, periRad2, 1e-14 * periRad1);
            EXPECT_NEAR(shapeParam1, shapeParam2, 1e-14 * shapeParam1);
            EXPECT_NEAR(orientationParam1, orientationParam2, 1e-5);
            EXPECT_NEAR(positionParam1, positionParam2, 1e-12); 
        }
        // test setApoRadForSize
        {
            keplerianState->setState(orbElem);
            keplerianState->setSizeType(ESizeType::ePeriAlt);
            keplerianState->setShapeType(EShapeType::eEcc);
            keplerianState->setPositionType(EPositionType::eTimeOfPeriPassage);
            keplerianState->setOrientationType(EOrientationType::eLAN); 

            double apoRad0 = keplerianState->getApoRad();
            double apoRad1 = apoRad0 * 1.2;
            double shapeParam1 = keplerianState->getShapeParam();
            double orientationParam1 = keplerianState->getOrientationParam();
            double positionParam1 = keplerianState->getPositionParam();
            keplerianState->setApoRadForSize(apoRad1);
             
            double apoRad2 = keplerianState->getApoRad();
            double shapeParam2 = keplerianState->getShapeParam();
            double orientationParam2 = keplerianState->getOrientationParam();
            double positionParam2 = keplerianState->getPositionParam();
            
            printf("apoRad1: %.15g\n", apoRad1);
            printf("apoRad2: %.15g\n", apoRad2);
            printf("orientationParam1: %.15g\n", orientationParam1);
            printf("orientationParam2: %.15g\n", orientationParam2);
            printf("shapeParam1: %.15g\n", shapeParam1);
            printf("shapeParam2: %.15g\n", shapeParam2);
            printf("positionParam1: %.15g\n", positionParam1);
            printf("positionParam2: %.15g\n", positionParam2);
            
            EXPECT_NEAR(apoRad1, apoRad2, 1e-14 * apoRad1);
            EXPECT_NEAR(shapeParam1, shapeParam2, 1e-14 * shapeParam1);
            EXPECT_NEAR(orientationParam1, orientationParam2, 1e-5);
            EXPECT_NEAR(positionParam1, positionParam2, 1e-12); 
        }
        // test setEcc
        {
            keplerianState->setState(orbElem);
            keplerianState->setSizeType(ESizeType::eSMA);
            keplerianState->setShapeType(EShapeType::eEcc);
            keplerianState->setPositionType(EPositionType::eTimePastAscNode);
            keplerianState->setOrientationType(EOrientationType::eLAN);

            // double ecc0 = keplerianState->getEcc();
            double ecc1 = 0.01;
            double sizeParam1 = keplerianState->getSizeParam();
            double orientationParam1 = keplerianState->getOrientationParam();
            double positionParam1 = keplerianState->getPositionParam();

            keplerianState->setEcc(ecc1);
            double ecc2 = keplerianState->getEcc();
            double sizeParam2 = keplerianState->getSizeParam();
            double orientationParam2 = keplerianState->getOrientationParam();
            double positionParam2 = keplerianState->getPositionParam();

            printf("ecc1: %.15g\n", ecc1);
            printf("ecc2: %.15g\n", ecc2);
            printf("orientationParam1: %.15g\n", orientationParam1);
            printf("orientationParam2: %.15g\n", orientationParam2);
            printf("sizeParam1: %.15g\n", sizeParam1);
            printf("sizeParam2: %.15g\n", sizeParam2);
            printf("positionParam1: %.15g\n", positionParam1);
            printf("positionParam2: %.15g\n", positionParam2);
            
            EXPECT_NEAR(ecc1, ecc2, 1e-14 * ecc1);
            EXPECT_NEAR(orientationParam1, orientationParam2, 1e-5);
            EXPECT_NEAR(sizeParam1, sizeParam2, 1e-14 * sizeParam1);
            EXPECT_NEAR(positionParam1, positionParam2, 1e-12); 
        }
        // test setPeriRadForShape
        {
            keplerianState->setState(orbElem);
            keplerianState->setSizeType(ESizeType::eSMA);
            keplerianState->setShapeType(EShapeType::eApoAlt);
            keplerianState->setPositionType(EPositionType::eArgLat);
            keplerianState->setOrientationType(EOrientationType::eLAN);

            double apoRad0 = keplerianState->getApoRad();
            double periRad1 = apoRad0 * 0.8;
            double sizeParam1 = keplerianState->getSizeParam();
            double orientationParam1 = keplerianState->getOrientationParam();
            double positionParam1 = keplerianState->getPositionParam();

            keplerianState->setPeriRadForShape(periRad1);

            double periRad2 = keplerianState->getPeriRad();
            double sizeParam2 = keplerianState->getSizeParam();
            double orientationParam2 = keplerianState->getOrientationParam();
            double positionParam2 = keplerianState->getPositionParam();

            printf("periRad1: %.15g\n", periRad1);
            printf("periRad2: %.15g\n", periRad2);
            printf("orientationParam1: %.15g\n", orientationParam1);
            printf("orientationParam2: %.15g\n", orientationParam2);
            printf("sizeParam1: %.15g\n", sizeParam1);
            printf("sizeParam2: %.15g\n", sizeParam2);
            printf("positionParam1: %.15g\n", positionParam1);
            printf("positionParam2: %.15g\n", positionParam2);
            
            EXPECT_NEAR(periRad1, periRad2, 1e-14 * periRad1);
            EXPECT_NEAR(sizeParam1, sizeParam2, 1e-14 * sizeParam1);
            EXPECT_NEAR(orientationParam1, orientationParam2, 1e-5);
            EXPECT_NEAR(positionParam1, positionParam2, 1e-12); 
        }
        // test setApoRadForShape
        {
            keplerianState->setState(orbElem);
            keplerianState->setSizeType(ESizeType::ePeriAlt);
            keplerianState->setShapeType(EShapeType::eEcc);
            keplerianState->setPositionType(EPositionType::eTimeOfPeriPassage);
            keplerianState->setOrientationType(EOrientationType::eLAN); 

            double apoRad0 = keplerianState->getApoRad();
            double apoRad1 = apoRad0 * 1.2;
            double sizeParam1 = keplerianState->getSizeParam();
            double orientationParam1 = keplerianState->getOrientationParam();
            double positionParam1 = keplerianState->getPositionParam();
            keplerianState->setApoRadForShape(apoRad1);
             
            double apoRad2 = keplerianState->getApoRad();
            double sizeParam2 = keplerianState->getSizeParam();
            double orientationParam2 = keplerianState->getOrientationParam();
            double positionParam2 = keplerianState->getPositionParam();
            
            printf("apoRad1: %.15g\n", apoRad1);
            printf("apoRad2: %.15g\n", apoRad2);
            printf("orientationParam1: %.15g\n", orientationParam1);
            printf("orientationParam2: %.15g\n", orientationParam2);
            printf("sizeParam1: %.15g\n", sizeParam1);
            printf("sizeParam2: %.15g\n", sizeParam2);
            printf("positionParam1: %.15g\n", positionParam1);
            printf("positionParam2: %.15g\n", positionParam2);
            
            EXPECT_NEAR(apoRad1, apoRad2, 1e-14 * apoRad1);
            EXPECT_NEAR(sizeParam1, sizeParam2, 1e-14 * sizeParam1);
            EXPECT_NEAR(orientationParam1, orientationParam2, 1e-5);
            EXPECT_NEAR(positionParam1, positionParam2, 1e-12); 
        }
        // test setArgPeri
        {
            keplerianState->setState(orbElem);
            keplerianState->setSizeType(ESizeType::eSMA);
            keplerianState->setShapeType(EShapeType::eApoAlt);
            keplerianState->setPositionType(EPositionType::eTimePastAscNode);
            keplerianState->setOrientationType(EOrientationType::eLAN);

            double argPeri1 = 100_deg;
            double shapeParam1 = keplerianState->getShapeParam();
            double orientationParam1 = keplerianState->getOrientationParam();
            double positionParam1 = keplerianState->getPositionParam();

            keplerianState->setArgPeri(argPeri1);
            double argPeri2 = keplerianState->getArgPeri();
            double shapeParam2 = keplerianState->getShapeParam();
            double orientationParam2 = keplerianState->getOrientationParam();
            double positionParam2 = keplerianState->getPositionParam();

            printf("argPeri1: %.15g\n", argPeri1);
            printf("argPeri2: %.15g\n", argPeri2);
            printf("orientationParam1: %.15g\n", orientationParam1);
            printf("orientationParam2: %.15g\n", orientationParam2);
            printf("shapeParam1: %.15g\n", shapeParam1);
            printf("shapeParam2: %.15g\n", shapeParam2);
            printf("positionParam1: %.15g\n", positionParam1);
            printf("positionParam2: %.15g\n", positionParam2);
            EXPECT_NEAR(argPeri1, argPeri2, 1e-5);
            EXPECT_NEAR(orientationParam1, orientationParam2, 1e-5);
            EXPECT_NEAR(shapeParam1, shapeParam2, 1e-14 * shapeParam1);
            EXPECT_NEAR(positionParam1, positionParam2, 1e-12); 
        }
    }
}


TEST_F(StateTest, Attr)
{
    auto earth = aGetEarth();
    auto stateKeplerian = StateKeplerian::MakeShared();
    stateKeplerian->setFrame(earth->makeFrameICRF());
    auto tp = TimePoint::FromUTC(2025, 12, 9, 0, 0, 0);
    stateKeplerian->setStateEpoch(tp);
    ModOrbElem orbElem{60000_km, 0.1, 2_deg, 3_deg, 4_deg, 5_deg};
    stateKeplerian->setState(orbElem);

    {
        double sma;
        errc_t rc = stateKeplerian->getAttrDouble("SMA", sma);
        EXPECT_EQ(rc, eNoError);
        EXPECT_EQ(sma, stateKeplerian->getSMA());
    }
    {
        double sma = stateKeplerian->getAttrDouble("SMA");
        EXPECT_EQ(sma, stateKeplerian->getSMA());
    }
    {
        double sma = stateKeplerian->attr("SMA");
        EXPECT_EQ(sma, stateKeplerian->getSMA());
    }
    {
        std::vector<std::string> attrs = {
            "SMA",
            "Period",
            "MeanMotion",
            "ApoRadForSize",
            "ApoAltForSize",
            "PeriRadForSize",
            "PeriAltForSize",

            "Ecc",
            "ApoRadForShape",
            "ApoAltForShape",
            "PeriRadForShape",
            "PeriAltForShape",

            "Inc",

            "RAAN",
            "LAN",

            "ArgPeri",

            "TrueAnomaly",
            "MeanAnomaly",
            "EccAnomaly",
            "ArgLat",
            "TimePastPeri",
            "TimePastAscNode"
        };
        for (auto& attr : attrs)
        {
            double val1 = stateKeplerian->attr(attr);
            stateKeplerian->attr(attr) = val1;
            double val2 = stateKeplerian->attr(attr);
            printf("%20s: %.15g -> %.15g\n", attr.c_str(), val1, val2);
            EXPECT_NE(val1, 0.0);
            if(attr != "LAN"){
                EXPECT_NEAR(val1, val2, 1e-14 * val1);
            }
        }
        EXPECT_EQ(stateKeplerian->attr("NotExistProperty").getValueDouble(), 0.0);
    }
    {
        double value = stateKeplerian->attr("SMA") = 1000_km;
        EXPECT_NEAR(value, 1000_km, 1e-14 * value);
    }
    {
        double value = stateKeplerian->attr("SMA") = "2000 km";
        EXPECT_NEAR(value, 2000_km, 1e-14 * value);
    }
    {
        double value = stateKeplerian->attr("ArgPeri") = "30 deg ";
        EXPECT_NEAR(value, 30_deg, 1e-14 * value);
    }
    {
        double value = stateKeplerian->attr("EccAnomaly") = aText("47° ");
        EXPECT_NEAR(value, 47_deg, 1e-14 * value);
    }
    {
        double value = stateKeplerian->attr("MeanAnomaly") = aText("45 arcsec ");
        EXPECT_NEAR(value, 45_arcsec, 1e-14 * value);
    }
    {
        double value = stateKeplerian->attr("TrueAnomaly") = aText(" 30 ″ ");
        EXPECT_NEAR(value, 30_arcsec, 1e-14 * value);
    }
}

GTEST_MAIN();



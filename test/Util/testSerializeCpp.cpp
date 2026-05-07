///
/// @file      testSerializeCpp.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-30
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


#include "AstUtil/SerdeAPI.hpp"
#include "AstUtil/CppSerializer.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/BuilderAPI.hpp"
#include "AstUtil/Literals.hpp"
#include "AstTest/Test.h"
#include "AstTest/TestSerialize.hpp"
#include "AstCore/StateCartesian.hpp"
#include "AstCore/StateKeplerian.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/BodyEphemerisDE.hpp"
#include "AstCore/BodyEphemerisSPK.hpp"

AST_USING_NAMESPACE

class SerializeCppTest: public testing::Test{
public:
    void SetUp() override{
        aInitialize();
    }
    void TearDown() override{
        aUninitialize();
    }
};

TEST_F(SerializeCppTest, StateCartesian) {
    if(aIsCI())
        GTEST_SKIP();

    auto earth = aGetBody("Earth");
    StateCartesian* state = new StateCartesian();
    state->setStateEpoch(TimePoint::FromUTC(2022, 2, 4, 12, 0, 0));
    state->setFrame(earth->makeFrameICRF());
    state->setX(7000);
    state->setY(8000);
    state->setZ(9000);
    state->setVx(1000);
    state->setVy(2000);
    state->setVz(3000);

    errc_t rc = aTestSerializeCpp(state);
    EXPECT_EQ(rc, eNoError);
}


TEST_F(SerializeCppTest, StateKeplerian) {
    GTEST_SKIP();
    if(aIsCI())
        GTEST_SKIP();

    auto earth = aGetBody("Earth");
    StateKeplerian* state = new StateKeplerian();
    state->setStateEpoch(TimePoint::FromUTC(2022, 2, 4, 12, 0, 0));
    state->setFrame(earth->makeFrameICRF());
    state->setSMA(1000000_km);
    state->setEcc(0.1);
    state->setInc(10_deg);
    state->setRAAN(20_deg);
    state->setArgPeri(30_deg);
    state->setTrueAnomaly(30_deg);
    errc_t rc = aTestSerializeCpp(state);
    EXPECT_EQ(rc, eNoError);
}

TEST_F(SerializeCppTest, BodyEphemerisDE) {
    if(aIsCI())
        GTEST_SKIP();

    auto earth = aGetBody("Earth");
    BodyEphemerisDE* ephemeris = new BodyEphemerisDE();
    ephemeris->setJplIndex(JplDe::eJupiter);

    errc_t rc = aTestSerializeCpp(ephemeris);
    EXPECT_EQ(rc, eNoError);
}

TEST_F(SerializeCppTest, BodyEphemerisSPK) {
    if(aIsCI())
        GTEST_SKIP();

    auto earth = aGetBody("Earth");
    BodyEphemerisSPK* ephemeris = new BodyEphemerisSPK();
    ephemeris->setSpiceIndex(3);
    errc_t rc = ephemeris->openSPKFile(aTestGetConfigValue("SPK_FILE"));
    EXPECT_EQ(rc, eNoError);

    rc = aTestSerializeCpp(ephemeris);
    EXPECT_EQ(rc, eNoError);
}

GTEST_MAIN()


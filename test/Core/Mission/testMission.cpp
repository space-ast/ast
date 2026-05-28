///
/// @file      testMission.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-07
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

#include "ast/MissionCommandLoader.hpp"
#include "ast/Segment.hpp"
#include "ast/Sequence.hpp"
#include "ast/OrbitElement.hpp"
#include "ast/RunTime.hpp"
#include "ast/EOP.hpp"
#include "ast/ObjectLinker.hpp"
#include "ast/Literals.hpp"
#include "ast/MathOperator.hpp"
#include "ast/Test.h"

AST_USING_NAMESPACE

// 测试 Hohmann 转移
TEST(MissionTest, HohmannTransfer)
{
    if(aIsCI())
        GTEST_SKIP();
    using namespace math;
    aInitialize();
    aDataContext_GetEOP()->unload();
    errc_t rc;
    std::string filepath = aTestGetConfigValue("STK_HOHMANNT_RANSFER_FILE").toString();
    printf("loading file: %s\n", filepath.c_str());
    SharedPtr<MissionCommand> missionCommand;
    rc = aLoadMissionCommand(filepath, missionCommand);
    EXPECT_EQ(rc, eNoError);
    rc = aResolveAllLinks();
    EXPECT_EQ(rc, eNoError);
    SharedPtr<SpacecraftState> initialState = SpacecraftState::NewDefault();
    initialState->setStateEpoch(TimePoint::FromUTC(2026, 4, 7, 4, 0, 0));
    Segment* segment = aobject_cast<Segment*>(missionCommand.get());
    ASSERT_TRUE(segment != nullptr);
    segment->setInputState(initialState.get());
    rc = missionCommand->execute();
    EXPECT_EQ(rc, eNoError);
    const auto outputState = segment->getOutputState();
    ASSERT_TRUE(outputState != nullptr);
    CartState cartState{};
    TimePoint tp{};
    outputState->getState(cartState);
    outputState->getStateEpoch(tp);
    printf("tp: %s\n", tp.toString().c_str());
    printf("cartState: %s\n", cartState.toString().c_str());
    CartState expectCartState{
        -9999.9956695325836336_km, 23.9548574078268430_km, -22.4634031187308558_km,
        -0.0065201289543048_km/sec, -5.5488192323528542_km/sec, -3.0117026995629019_km/sec 
    };
    TimePoint expectTime = TimePoint::Parse("7 Apr 2026 07:48:46.474  UTC");
    printf("expectTime: %s\n", expectTime.toString().c_str());
    printf("expectCartState: %s\n", expectCartState.toString().c_str());
    double timeError = tp - expectTime;
    printf("timeError: %f\n", timeError);
    EXPECT_NEAR(timeError, 0.0, 1e-2);
    CartState cartError = cartState - expectCartState;
    printf("cartError: %s\n", cartError.toString().c_str());
    EXPECT_NEAR(cartError.pos().norm(), 0.0, 50);
    EXPECT_NEAR(cartError.vel().norm(), 0.0, 0.05);
}



TEST(MissionTest, GeoRendezvous)
{
    if(aIsCI())
        GTEST_SKIP();
    using namespace math;
    aInitialize();
    aDataContext_GetEOP()->unload();
    errc_t rc;
    std::string filepath = aTestGetConfigValue("STK_GEO_RENDEZVOUS_FILE").toString();
    printf("loading file: %s\n", filepath.c_str());
    SharedPtr<MissionCommand> missionCommand;
    rc = aLoadMissionCommand(filepath, missionCommand);
    EXPECT_EQ(rc, eNoError);
    rc = aResolveAllLinks();
    EXPECT_EQ(rc, eNoError);
    return;
    // SharedPtr<SpacecraftState> initialState = SpacecraftState::NewDefault();
    // initialState->setStateEpoch(TimePoint::FromUTC(2026, 4, 7, 4, 0, 0));
    // Segment* segment = aobject_cast<Segment*>(missionCommand.get());
    // ASSERT_TRUE(segment != nullptr);
    // segment->setInputState(initialState.get());
    // rc = missionCommand->execute();
    // EXPECT_EQ(rc, eNoError);
    // const auto outputState = segment->getOutputState();
    // ASSERT_TRUE(outputState != nullptr);
    // CartState cartState{};
    // TimePoint tp{};
    // outputState->getState(cartState);
    // outputState->getStateEpoch(tp);
    // printf("tp: %s\n", tp.toString().c_str());
    // printf("cartState: %s\n", cartState.toString().c_str());
}

GTEST_MAIN()




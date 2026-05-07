///
/// @file      testAIDesign.cpp
/// @brief     测试AI辅助设计
/// @details   测试AI辅助设计框架的基础能力，包括设计轨道、计算状态等
/// @author    axel
/// @date      2026-04-26
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

#include "AstAI/ChatSession.hpp"
#include "AstAI/AgentSession.hpp"
#include "AstAI/AgentUtil.hpp"
#include "AstSim/Satellite.hpp"
#include "AstSim/Facility.hpp"
#include "AstSim/MotionTwoBody.hpp"
#include "AstSim/MotionJ2Analytical.hpp"
#include "AstCore/StateCartesian.hpp"
#include "AstCore/StateKeplerian.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include "AstUtil/Literals.hpp"
#include "AstMath/ODEIntegrator.hpp"
#include "AstMath/RKF78.hpp"
#include "AstTest/Test.h"

AST_USING_NAMESPACE

const bool testAI = aIsCI();

TEST(AIDesignTest, MissionDesign)
{
    if(testAI)
        GTEST_SKIP();
    aRemoveAllObjects();
    const char* message = u8"帮我创建一个霍曼转移任务序列";
    AgentSession session;
    std::string response = session.sendMessage(message);
}

TEST(AIDesignTest, ClassAnalysis)
{
    if(testAI)
        GTEST_SKIP();
    aRemoveAllObjects();
    {
        const char* message = u8"软件支持哪些移动对象？";
        AgentSession session;
        session.sendMessage(message);
    }
    {
        const char* message = u8"软件能支持什么场景的建模分析？";
        AgentSession session;
        session.sendMessage(message);
    }

}



TEST(AIDesignTest, RKF78)
{
    if(testAI)
        GTEST_SKIP();
    aRemoveAllObjects();
    const char* message = u8"帮我新建一个积分器, RKF78吧";
    AgentSession session;
    std::string response = session.sendMessage(message);
    ODEIntegrator* integrator = dynamic_cast<ODEIntegrator*>(aFindObject(ODEIntegrator::StaticType()));
    ASSERT_TRUE(integrator != nullptr);
    RKF78* rkf78 = dynamic_cast<RKF78*>(integrator);
    ASSERT_TRUE(rkf78 != nullptr);
}


TEST(AIDesignTest, ShowEditDialog)
{
    if(testAI)
        GTEST_SKIP();
    aRemoveAllObjects();
    AgentSession session;
    const char* message =
        R"(帮我新建一个开普勒状态，然后显示他的编辑界面:
        - 参考系是地球惯性系
        - 轨道历元是2026-04-24 00:00:00
        - 圆轨道高度是 500km, 倾角91度, 升交点赤经20°, 纬度幅角40°
        )";
    std::string response = session.sendMessage(message);
    
}


TEST(AIDesignTest, WalkerDesign)
{
    if(testAI)
        GTEST_SKIP();
    aRemoveAllObjects();
    clock_t startTime = clock();
    AgentSession session;
    const char* message = u8R"(
    帮我设计一个Walker星座:
    - 轨道高度大约 500km
    - 设计参数(i:T:P:F) = 55°:24:6:1
    )";
    session.sendMessage(message);
    clock_t endTime = clock();
    printf("WalkerDesign time: %f s\n", double(endTime - startTime) / CLOCKS_PER_SEC);
    std::vector<Object*> objects = aFindObjects(Satellite::StaticType());
    size_t numSatellites = objects.size();
    EXPECT_GT(numSatellites, 0);
    printf("numSatellites: %zu\n", numSatellites);
    aPrintAllObjects();
}


TEST(AIDesignTest, Facility)
{
    if(testAI)
        GTEST_SKIP();
    aRemoveAllObjects();
    AgentSession session;
    const char* message = u8"帮我插入一个地面站，位置在海口";
    session.sendMessage(message);
    Facility* facility = dynamic_cast<Facility*>(aFindObject(Facility::StaticType()));
    ASSERT_TRUE(facility != nullptr);
    EXPECT_NEAR(facility->latitude(), 20.0_deg, 0.5_deg);
    EXPECT_NEAR(facility->longitude(), 110.3_deg, 0.5_deg);
    EXPECT_NEAR(facility->altitude(), 0.0_m, 10_m);
    EXPECT_EQ(facility->body(), aGetEarth());
}


TEST(AIDesignTest, SSODesign)
{
    if(testAI)
        GTEST_SKIP();
    aRemoveAllObjects();
    AgentSession session;
    
    const char* message = u8"帮我设计一个太阳同步轨道";
    session.sendMessage(message);
    Satellite* satellite = dynamic_cast<Satellite*>(aFindObject(Satellite::StaticType()));
    ast_printf("%s\n", aObjectToJson(satellite).toJsonString(2).c_str());
    ASSERT_TRUE(satellite != nullptr);
    MotionOrbitDynamics* motionProfile = dynamic_cast<MotionOrbitDynamics*>(satellite->getMotionProfile());
    ASSERT_TRUE(motionProfile != nullptr);
    State* initialState = motionProfile->getInitialState();
    ASSERT_TRUE(initialState != nullptr);
}

TEST(AIDesignTest, Satellite)
{
    if(testAI)
        GTEST_SKIP();
    aRemoveAllObjects();
    AgentSession session;
    const char* message = u8R"(
    帮我新建一个卫星，
    - 名称是 TestSatellite
    - 轨道历元是 2026-04-23 00:00:00
    - 参考坐标系和预报坐标系都是地球惯性系
    - 初始状态是 100km, 200e3, 300km, 3km/s, 2200m/s, 1km/s
    - 预报器是J2解析预报器
    )";
    std::string response = session.sendMessage(message);
    Satellite* satellite = dynamic_cast<Satellite*>(aFindObject(Satellite::StaticType()));
    ASSERT_TRUE(satellite != nullptr);
    EXPECT_EQ(satellite->getName(), "TestSatellite");
    StateCartesian* initialState = dynamic_cast<StateCartesian*>(satellite->getInitialState());
    ASSERT_TRUE(initialState != nullptr);

    EXPECT_EQ(initialState->x(), 100.0_km);
    EXPECT_EQ(initialState->y(), 200e3_m);
    EXPECT_EQ(initialState->z(), 300.0_km);
    EXPECT_EQ(initialState->vx(), 3.0_km_s);
    EXPECT_EQ(initialState->vy(), 2200.0_m_s);
    EXPECT_EQ(initialState->vz(), 1.0_km_s);
    EXPECT_EQ(initialState->getStateEpoch_TimePoint(), TimePoint::Parse("2026-04-23 00:00:00"));
    EXPECT_EQ(initialState->getGM(), aGetEarth()->getGM());

    auto j2profile = dynamic_cast<MotionJ2Analytical*>(satellite->getMotionProfile());
    ASSERT_TRUE(j2profile != nullptr);
}


TEST(AIDesignTest, MotionTwoBody)
{
    if (testAI)
        GTEST_SKIP();
    aRemoveAllObjects();
    AgentSession session;
    const char* message = u8R"(
    帮我新建一个二体运动模型，
    - 预报坐标系是地球惯性系
    - 参考坐标系是地球惯性系
    - 轨道历元是 2026-04-23 00:00:00
    - 初始状态是 100km, 200e3, 300km, 3km/s, 2200m/s, 1km/s
    )";
    std::string response = session.sendMessage(message);
    MotionTwoBody* motion = dynamic_cast<MotionTwoBody*>(aFindObject(MotionTwoBody::StaticType()));
    ASSERT_TRUE(motion != nullptr);
    StateCartesian* state = dynamic_cast<StateCartesian*>(motion->getInitialState());
    ASSERT_TRUE(state != nullptr);
    EXPECT_EQ(state->x(), 100.0_km);
    EXPECT_EQ(state->y(), 200e3_m);
    EXPECT_EQ(state->z(), 300.0_km);
    EXPECT_EQ(state->vx(), 3.0_km_s);
    EXPECT_EQ(state->vy(), 2200.0_m_s);
    EXPECT_EQ(state->vz(), 1.0_km_s);
    EXPECT_EQ(state->getStateEpoch_TimePoint(), TimePoint::Parse("2026-04-23 00:00:00"));
    EXPECT_EQ(state->getGM(), aGetEarth()->getGM());
}

TEST(AIDesignTest, StateKeplerian)
{
    if (testAI)
        GTEST_SKIP();
    aRemoveAllObjects();
    AgentSession session;
    std::vector<std::string> messages({
        R"(帮我新建一个开普勒状态，不用关联到卫星:
        - 参考系是地球惯性系
        - 轨道历元是2026-04-24 00:00:00
        - 圆轨道高度是 500km, 倾角91度, 升交点赤经20°, 纬度幅角40°
        )"
        });
    for (auto& message : messages)
    {
        std::string response = session.sendMessage(message);
    }
    StateKeplerian* state = dynamic_cast<StateKeplerian*>(aFindObject(StateKeplerian::StaticType()));
    ASSERT_TRUE(state != nullptr);
    EXPECT_EQ(state->getPeriAlt(), 500_km);
    EXPECT_EQ(state->getEcc(), 0);
    EXPECT_NEAR(state->getInc(), 91_deg, 0.001_deg);
    EXPECT_NEAR(state->getRAAN(), 20_deg, 0.001_deg);
    EXPECT_NEAR(state->getArgLat(), 40_deg, 0.001_deg);
    EXPECT_EQ(state->getStateEpoch_TimePoint(), TimePoint::Parse("2026-04-24 00:00:00"));
    EXPECT_EQ(state->getGM(), aGetEarth()->getGM());
}



TEST(AIDesignTest, StateCartesian)
{
    if(testAI)
        GTEST_SKIP();
    aRemoveAllObjects();
    AgentSession session;
    std::vector<std::string> messages({
        R"(帮我新建一个笛卡尔状态:
        - 参考系是地球惯性系
        - 轨道历元是2026-04-26 00:00:00
        - 位置速度分别为100km, 200e3, 300km, 3km/s, 2200m/s, 1km/s
        )"
    });
    for(auto& message : messages)
    {
        std::string response = session.sendMessage(message);
    }
    StateCartesian* state = dynamic_cast<StateCartesian*>(aFindObject(StateCartesian::StaticType()));
    ASSERT_TRUE(state != nullptr);
    EXPECT_EQ(state->x(), 100.0_km);
    EXPECT_EQ(state->y(), 200e3_m);
    EXPECT_EQ(state->z(), 300.0_km);
    EXPECT_EQ(state->vx(), 3.0_km_s);
    EXPECT_EQ(state->vy(), 2200.0_m_s);
    EXPECT_EQ(state->vz(), 1.0_km_s);
    EXPECT_EQ(state->getStateEpoch_TimePoint(), TimePoint::Parse("2026-04-26 00:00:00"));
    EXPECT_EQ(state->getGM(), aGetEarth()->getGM());
}





// GTEST_MAIN()

int main(int argc, char **argv) {
    printf("Running main() from %s\n", __FILE__); 
    testing::GTEST_FLAG(catch_exceptions) = false;
    testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS(); 
}
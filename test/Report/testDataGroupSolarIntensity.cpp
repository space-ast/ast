///
/// @file      testDataGroupSolarIntensity.cpp
/// @brief     太阳光照强度数据组测试
/// @details   构建 LEO 圆轨道二体星历，用 DataGroupSolarIntensity 在一天时间网格上
///             计算光照强度/遮蔽比例/光照条件/遮挡体，校验每行不变量
///             （取值范围、shadowRatio = 1 - intensity、条件与遮挡体一致性），
///             以及一天内同时存在光照时段与遮蔽时段。
/// @author    axel
/// @date      2026-08-18
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

#include "ast/Test.hpp"
#include "ast/DataGroupSolarIntensity.hpp"
#include "ast/EphemerisTwoBody.hpp"
#include "ast/OrbitElement.hpp"
#include "ast/RunTime.hpp"
#include "ast/RunTimeSolarSystem.hpp"
#include "ast/EOP.hpp"
#include "ast/TimePoint.hpp"
#include "ast/TimeInterval.hpp"
#include "ast/TimeList.hpp"
#include "ast/Literals.hpp"

#include <string>

AST_USING_NAMESPACE

using namespace ast::literals;

static TimeInterval DefaultInterval()
{
    return {
        TimePoint::FromUTC(2026, 8, 13, 4, 0, 0.0),
        TimePoint::FromUTC(2026, 8, 14, 4, 0, 0.0)
    };
}

/// @brief 构建 LEO 圆轨道二体星历（~300 km 高度，i=28.5°）
static SharedPtr<EphemerisTwoBody> MakeLeoEphemeris()
{
    auto earth = aGetEarth();
    auto eci   = earth->getFrameInertial();
    double gm   = earth->getGM();
    ModOrbElem elem{6678137, 0, 28.5_deg, 0, 0, 0};
    CartState initstate;
    aModOrbElemToCart(elem, gm, initstate.pos(), initstate.vel());
    auto epoch = "13 Aug 2026 04:00:00.000 UTCG"_utc;
    return EphemerisTwoBody::New(eci, epoch, initstate);
}

class DataGroupSolarIntensityTest : public testing::Test
{
public:
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

TEST_F(DataGroupSolarIntensityTest, LeoOrbitOneDay)
{
    DataGroupSolarIntensity dg;
    SharedPtr<EphemerisTwoBody> ephemeris = MakeLeoEphemeris();
    dg.setPoint(ephemeris);
    dg.setLightSource(aGetSun());
    dg.setOccultingBodies({aGetEarth()});

    TimeList timeList = TimeList::FromInterval(DefaultInterval(), 60.0);

    VariantVector result;
    errc_t rc = dg.calculate(timeList, result);
    ASSERT_EQ(rc, eNoError);

    auto data = result.asSpan<DataGroupSolarIntensity::Data>();
    ASSERT_EQ(data.size(), timeList.size());

    bool sawSunlight = false;
    bool sawEclipse  = false;

    for (size_t i = 0; i < data.size(); ++i)
    {
        const auto& row = data[i];

        // 取值范围
        EXPECT_GE(row.getIntensity(), 0.0) << "row " << i;
        EXPECT_LE(row.getIntensity(), 1.0) << "row " << i;
        EXPECT_GE(row.getShadowRatio(), 0.0) << "row " << i;
        EXPECT_LE(row.getShadowRatio(), 1.0) << "row " << i;

        // 遮蔽比例 = 1 - 光照强度
        EXPECT_NEAR(row.getShadowRatio(), 1.0 - row.getIntensity(), 1e-9) << "row " << i;

        // 条件与遮挡体一致性
        if (row.getIntensity() >= 1.0)
        {
            EXPECT_EQ(row.getCurrentCondition(), std::string("Sunlight")) << "row " << i;
            EXPECT_TRUE(row.getObstruction().empty()) << "row " << i;
            sawSunlight = true;
        }
        else if (row.getIntensity() <= 0.0)
        {
            EXPECT_EQ(row.getCurrentCondition(), std::string("Umbra")) << "row " << i;
            EXPECT_FALSE(row.getObstruction().empty()) << "row " << i;
            sawEclipse = true;
        }
        else
        {
            EXPECT_EQ(row.getCurrentCondition(), std::string("Penumbra")) << "row " << i;
            EXPECT_FALSE(row.getObstruction().empty()) << "row " << i;
            sawEclipse = true;
        }
    }

    // 一天内应同时存在光照时段与遮蔽时段
    EXPECT_TRUE(sawSunlight);
    EXPECT_TRUE(sawEclipse);
}

GTEST_MAIN()

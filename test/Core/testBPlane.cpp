///
/// @file      testBPlane.cpp
/// @brief     B平面坐标类型转换测试
/// @details   
///
/// @author    axel
/// @date      2026-09-13
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


#include "ast/AstTestMacro.h"
#include "ast/OrbitElement.hpp"
#include "ast/StateBPlane.hpp"
#include "ast/CelestialBody.hpp"
#include "ast/RunTime.hpp"
#include "ast/MoonFrame.hpp"
#include "ast/Rotation.hpp"
#include "ast/AxesICRF.hpp"
#include "ast/Constants.hpp"
#include "ast/Literals.hpp"
#include <cmath>
#include <cstdio>

AST_USING_NAMESPACE

namespace {

const double kKm = 1000.0;                      ///< km -> m
const double kMuMoon = 4.9028029535968e12;      ///< 月球引力常数 [m^3/s^2]
const double kMuEarth = 3.986004418e14;         ///< 地球引力常数 [m^3/s^2]

/// 参考向量: 月球绕地球的轨道法向
const Vector3d kMoonOrbitNormal{-0.046532, -0.468154, 0.882417};

/// 参考向量: 黄道极, 即地球绕太阳的轨道法向
const Vector3d kEclipticPole{0.0, -0.3977772, 0.9174821};

/// 月球ICRF的直角坐标状态 [m], [m/s]
const Vector3d kMoonPos{kKm * 377983.0, kKm * 82591.2, kKm * 63397.4};
const Vector3d kMoonVel{kKm * -0.21845, kKm * 7.67701, kKm * 4.14573};

/// 地球惯性系的直角坐标状态 [m], [m/s]
const Vector3d kEarthPos{kKm * 6678.14, 0.0, 0.0};
const Vector3d kEarthVel{0.0, kKm * 6.78953, kKm * 3.68641};

double dot3(const Vector3d& a, const Vector3d& b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

Vector3d cross3(const Vector3d& a, const Vector3d& b)
{
    return Vector3d{a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]};
}

/// 单位化矢量
Vector3d unit3(const Vector3d& a)
{
    double m = sqrt(dot3(a, a));
    return Vector3d{a[0] / m, a[1] / m, a[2] / m};
}

/// 两个矢量的夹角 [deg]
double angleDeg(const Vector3d& a, const Vector3d& b)
{
    double c = dot3(unit3(a), unit3(b));
    if (c > 1.0) c = 1.0;
    if (c < -1.0) c = -1.0;
    return acos(c) * kRadToDeg;
}

}   // namespace

/// @brief 双曲线情形: 月球ICRF的直角坐标转换为B平面参数
TEST(BPlane, MoonCartToBPlane)
{
    {
        BPlaneElem bPlane;
        errc_t rc = aCartToBPlane(kMoonPos, kMoonVel, kMuMoon, kMoonOrbitNormal, bPlane);
        EXPECT_EQ(rc, eNoError);
        printf("BPlaneElem: %s\n", bPlane.toString().c_str());

        // 入渐近线方向
        EXPECT_NEAR(bPlane.ra() * kRadToDeg, 91.6163, 1e-3);
        EXPECT_NEAR(bPlane.dec() * kRadToDeg, 28.3609, 1e-3);
        // B矢量分量
        EXPECT_NEAR(bPlane.bDotR(), 1107.5 * kKm, 5e3);
        EXPECT_NEAR(bPlane.bDotT(), 380858.0 * kKm, 1e4);
        // 特征能量与真近点角
        EXPECT_NEAR(bPlane.c3(), 76.1463e6, 1e3);
        EXPECT_NEAR(bPlane.trueA() * kRadToDeg, 13.7697, 1e-3);
        // B矢量模即双曲线半短轴 |a|*sqrt(e^2-1)
        double bmag = sqrt(bPlane.bDotR() * bPlane.bDotR() + bPlane.bDotT() * bPlane.bDotT());
        EXPECT_NEAR(bmag, 380859.6 * kKm, 1e4);
    }
    {
        auto tp = "13 Sep 2026 04:00:00.000 UTCG"_timepoint;
        Vector3d pos{372800_km, 120100_km, 15200_km}, vel{-0.64_km/sec, 8.68_km/sec, 0.62_km/sec};
        Vector3d refVector;
        auto moon = aGetMoon();
        aBodyOrbitNormalIn(*moon->getAxesInertial(), *moon, tp, refVector);
        BPlaneElem bPlane;
        errc_t rc = aCartToBPlane(pos, vel, moon->getGM(), refVector, bPlane);
        EXPECT_EQ(rc, eNoError);
        printf("BPlaneElem: %s\n", bPlane.toString().c_str());
        BPlaneElem bPlaneExpected{
            94.2049179806712402_deg, 4.0748642892009537_deg, 
            1098.2012539112306513_km, 380780.1702146558673121_km,
            76.1113833430455600_km2/sec2, 13.7589655540543809_deg
        };
        EXPECT_NEAR(bPlane.ra(), bPlaneExpected.ra(), 1e-3);
        EXPECT_NEAR(bPlane.dec(), bPlaneExpected.dec(), 1e-3);
        EXPECT_NEAR(bPlane.bDotR(), bPlaneExpected.bDotR(), 1e-2);
        EXPECT_NEAR(bPlane.bDotT(), bPlaneExpected.bDotT(), 1e-1);
        EXPECT_NEAR(bPlane.c3(), bPlaneExpected.c3(), 1e-1);
        EXPECT_NEAR(bPlane.trueA(), bPlaneExpected.trueA(), 1e-3);
    }
}

/// @brief 双曲线情形: 由B平面参数反算直角坐标
TEST(BPlane, MoonBPlaneToCart)
{
    {
        BPlaneElem bPlane;
        bPlane.ra_ = 91.6163_deg;
        bPlane.dec_ = 28.3609_deg;
        bPlane.bDotR_ = 1107.5 * kKm;
        bPlane.bDotT_ = 380858.0 * kKm;
        bPlane.c3_ = 76.1463e6;
        bPlane.trueA_ = 13.7697_deg;

        Vector3d pos, vel;
        errc_t rc = aBPlaneToCart(bPlane, kMuMoon, kMoonOrbitNormal, pos, vel);
        EXPECT_EQ(rc, eNoError);

        for (int i = 0; i < 3; i++)
        {
            EXPECT_NEAR(pos[i], kMoonPos[i], 1e3);    
            EXPECT_NEAR(vel[i], kMoonVel[i], 1e3);    
        }
    }
    {
        BPlaneElem bPlane{
            94.2049179806712402_deg, 4.0748642892009537_deg, 
            1098.2012539112306513_km, 380780.1702146558673121_km,
            76.1113833430455600_km2/sec2, 13.7589655540543809_deg
        };
        auto tp = "13 Sep 2026 04:00:00.000 UTCG"_timepoint;
        Vector3d refVector;
        auto moon = aGetMoon();
        aBodyOrbitNormalIn(*moon->getAxesInertial(), *moon, tp, refVector);
        Vector3d pos, vel;
        errc_t rc = aBPlaneToCart(bPlane, moon->getGM(), refVector, pos, vel);
        EXPECT_EQ(rc, eNoError);
        printf("pos: %f, %f, %f\n", pos[0], pos[1], pos[2]);
        printf("vel: %f, %f, %f\n", vel[0], vel[1], vel[2]);
        Vector3d posExpected{372800_km, 120100_km, 15200_km};
        Vector3d velExpected{-0.64_km/sec, 8.68_km/sec, 0.62_km/sec};
        for (int i = 0; i < 3; i++)
        {
            EXPECT_NEAR(pos[i], posExpected[i], 1e-1);
            EXPECT_NEAR(vel[i], velExpected[i], 1e-2);
        }
    }
}

/// @brief 双曲线情形: 直角坐标与B平面参数之间的往返一致性
TEST(BPlane, MoonRoundTrip)
{
    {
        BPlaneElem bPlane;
        errc_t rc = aCartToBPlane(kMoonPos, kMoonVel, kMuMoon, kMoonOrbitNormal, bPlane);
        EXPECT_EQ(rc, eNoError);

        Vector3d pos, vel;
        rc = aBPlaneToCart(bPlane, kMuMoon, kMoonOrbitNormal, pos, vel);
        EXPECT_EQ(rc, eNoError);

        for (int i = 0; i < 3; i++)
        {
            EXPECT_NEAR(pos[i], kMoonPos[i], 1e-3);
            EXPECT_NEAR(vel[i], kMoonVel[i], 1e-9);
        }
    }
    {
        auto tp = "13 Sep 2026 04:00:00.000 UTCG"_timepoint;
        Vector3d pos{372800_km, 120100_km, 15200_km}, vel{-0.64_km/sec, 8.68_km/sec, 0.62_km/sec};
        Vector3d refVector;
        auto moon = aGetMoon();
        aBodyOrbitNormalIn(*moon->getAxesInertial(), *moon, tp, refVector);
        BPlaneElem bPlane;
        errc_t rc = aCartToBPlane(pos, vel, moon->getGM(), refVector, bPlane);
        EXPECT_EQ(rc, eNoError);
        Vector3d pos2, vel2;
        rc = aBPlaneToCart(bPlane, moon->getGM(), refVector, pos2, vel2);
        EXPECT_EQ(rc, eNoError);
        for (int i = 0; i < 3; i++)
        {
            EXPECT_NEAR(pos2[i], pos[i], 1e-3);
            EXPECT_NEAR(vel2[i], vel[i], 1e-3);
        }
    }

}

/// @brief 椭圆情形: 对偏心率小于1的轨道同样给出B平面参数
/// @details 该例为近圆轨道(偏心率数值上为零), 入渐近线不存在, 此时退化为:
///          入渐近线方向取+x轴, B矢量模取半短轴 a*sqrt(1-e^2)
TEST(BPlane, EarthEllipticCartToBPlane)
{
    {
        BPlaneElem bPlane;
        errc_t rc = aCartToBPlane(kEarthPos, kEarthVel, kMuEarth, kEclipticPole, bPlane);
        EXPECT_EQ(rc, eNoError);
        printf("BPlaneElem: %s\n", bPlane.toString().c_str());

        EXPECT_NEAR(bPlane.ra() * kRadToDeg, 0.0, 1e-3);
        EXPECT_NEAR(bPlane.dec() * kRadToDeg, 0.0, 1e-3);
        EXPECT_NEAR(bPlane.bDotR(), 589.306 * kKm, 1e3);
        EXPECT_NEAR(bPlane.bDotT(), 6652.08 * kKm, 1e3);
        EXPECT_NEAR(bPlane.c3(), -59.6874e6, 1e3);
        EXPECT_NEAR(bPlane.trueA() * kRadToDeg, 0.0, 1e-3);
        // 圆轨道时B矢量模等于半长轴(半短轴)
        double bmag = sqrt(bPlane.bDotR() * bPlane.bDotR() + bPlane.bDotT() * bPlane.bDotT());
        EXPECT_NEAR(bmag, 6678.13 * kKm, 1e3);
    }
    {
        Vector3d pos{2737_km, 0, 0}, vel{0, 1.338_km/sec, 0};
        BPlaneElem bPlane;
        auto tp = "13 Sep 2026 04:00:00.000 UTCG"_timepoint;
        Vector3d refVector;
        auto moon = aGetMoon();
        aBodyOrbitNormalIn(*moon->getAxesInertial(), *moon, tp, refVector);
        errc_t rc = aCartToBPlane(pos, vel, moon->getGM(), refVector, bPlane);
        EXPECT_EQ(rc, eNoError);
        printf("BPlaneElem: %s\n", bPlane.toString().c_str());
        BPlaneElem bPlaneExpected{
            180.0_deg, 0.0_deg, 
            175.6581513230388794_km, 2729.7342099706947920_km,
            -1.7923649189297770_km2/sec2, 180.0000000000000000_deg
        };
        EXPECT_NEAR(bPlane.ra(), bPlaneExpected.ra(), 1e-3);
        EXPECT_NEAR(bPlane.dec(), bPlaneExpected.dec(), 1e-3);
        EXPECT_NEAR(bPlane.bDotR(), bPlaneExpected.bDotR(), 1e-3);
        EXPECT_NEAR(bPlane.bDotT(), bPlaneExpected.bDotT(), 1e-3);
        EXPECT_NEAR(bPlane.c3(), bPlaneExpected.c3(), 1e-3);
        EXPECT_NEAR(bPlane.trueA(), bPlaneExpected.trueA(), 1e-3);
    }
}

/// @brief 异常输入
TEST(BPlane, InvalidInput)
{
    BPlaneElem bPlane;
    // 角动量为零(径向运动), 轨道面不确定
    Vector3d pos{7.0e6, 0.0, 0.0};
    Vector3d vel{1.0e3, 0.0, 0.0};
    EXPECT_NE(aCartToBPlane(pos, vel, kMuEarth, kEclipticPole, bPlane), eNoError);

    // 参考向量与入渐近线平行, R、T轴无定义
    errc_t rc = aCartToBPlane(kMoonPos, kMoonVel, kMuMoon, kMoonOrbitNormal, bPlane);
    EXPECT_EQ(rc, eNoError);
    Vector3d shat{cos(bPlane.dec()) * cos(bPlane.ra()),
                  cos(bPlane.dec()) * sin(bPlane.ra()),
                  sin(bPlane.dec())};
    EXPECT_NE(aCartToBPlane(kMoonPos, kMoonVel, kMuMoon, shat, bPlane), eNoError);

    // B矢量模大于半长轴(椭圆轨道下不合法)
    BPlaneElem bad;
    bad.ra_ = 0.0;
    bad.dec_ = 0.0;
    bad.bDotR_ = 0.0;
    bad.bDotT_ = 2.0e7;
    bad.c3_ = -59.6874e6;
    bad.trueA_ = 0.0;
    Vector3d pos2, vel2;
    EXPECT_NE(aBPlaneToCart(bad, kMuEarth, kEclipticPole, pos2, vel2), eNoError);
}

class BPlaneStateTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        aInitialize();
    }
    void TearDown() override
    {
        aUninitialize();
    }

    /// @brief 构造指定天体中心、ICRF轴系下的B平面状态
    HStateBPlane makeBodyState(Body* body, EBPlaneRefVector refType, const Vector3d& refVector = Vector3d::Zero())
    {
        auto frame = body->makeFrame(aAxesICRF());
        frame->setName("ICRF");
        auto state = StateBPlane::MakeShared();
        state->setFrame(frame.get());
        state->setStateEpoch(TimePoint::FromUTC(2026, 9, 13, 4, 0, 0));
        state->setRefVector(refType, refVector);
        return state;
    }

    /// @brief 构造月球惯性系下的B平面状态
    HStateBPlane makeMoonState(EBPlaneRefVector refType, const Vector3d& refVector = Vector3d::Zero())
    {
        return makeBodyState(aGetMoon(), refType, refVector);
    }
};

/// @brief 自定义参考向量: 状态类的正向/反向转换
TEST_F(BPlaneStateTest, CustomRefVector)
{
    auto state = makeMoonState(EBPlaneRefVector::eCustom, kMoonOrbitNormal);
    // 月球引力常数取自加载的重力场模型, 与常数 kMoonGrav 略有差别
    EXPECT_NEAR(state->getGM() / kMuMoon, 1.0, 1e-5);

    CartState cart;
    cart.pos() = kMoonPos;
    cart.vel() = kMoonVel;
    errc_t rc = state->setState(cart);
    EXPECT_EQ(rc, eNoError);

    BPlaneElem bPlane;
    state->getState(bPlane);
    EXPECT_NEAR(bPlane.ra() * kRadToDeg, 91.6163, 1e-3);
    EXPECT_NEAR(bPlane.dec() * kRadToDeg, 28.3609, 1e-3);
    EXPECT_NEAR(bPlane.bDotR(), 1107.5 * kKm, 1e3);
    EXPECT_NEAR(bPlane.bDotT(), 380858.0 * kKm, 1e3);
    EXPECT_NEAR(bPlane.c3(), 76.1463e6, 1e3);
    EXPECT_NEAR(bPlane.trueA() * kRadToDeg, 13.7697, 1e-3);

    // 反算直角坐标
    CartState cartBack;
    rc = state->getState(cartBack);
    EXPECT_EQ(rc, eNoError);
    for (int i = 0; i < 3; i++)
    {
        EXPECT_NEAR(cartBack.pos()[i], kMoonPos[i], 1e-3);
        EXPECT_NEAR(cartBack.vel()[i], kMoonVel[i], 1e-2);
    }

    // 由状态类型枚举创建状态对象
    auto state2 = State::New(EStateType::eBPlane);
    ASSERT_TRUE(state2 != nullptr);
    EXPECT_EQ(state2->getStateType(), EStateType::eBPlane);
    delete state2;
}


/// @brief 航天器自身轨道法向: 该类型下B矢量与T轴平行, B·R恒为0
TEST_F(BPlaneStateTest, SpacecraftOrbitNormal)
{
    auto state = makeMoonState(EBPlaneRefVector::eSpacecraftOrbitNormal);

    // 由B平面参数反算时无法确定参考向量
    CartState cartBack;
    EXPECT_NE(state->getState(cartBack), eNoError);

    // 正向转换: 参考向量即直角坐标状态的轨道法向
    CartState cart;
    cart.pos() = kMoonPos;
    cart.vel() = kMoonVel;
    errc_t rc = state->setState(cart);
    EXPECT_EQ(rc, eNoError);

    Vector3d refVector;
    rc = state->getRefVector(refVector, &cart);
    EXPECT_EQ(rc, eNoError);
    EXPECT_NEAR(angleDeg(refVector, unit3(cross3(kMoonPos, kMoonVel))), 0.0, 1e-6);

    BPlaneElem bPlane;
    state->getState(bPlane);
    EXPECT_NEAR(bPlane.bDotR(), 0.0, 1e-6);
    EXPECT_NEAR(fabs(bPlane.bDotT()), 380859.6 * kKm, 1e3);
}


GTEST_MAIN()

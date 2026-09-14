///
/// @file      testSpherical.cpp
/// @brief     球坐标根数类型转换测试
/// @details
///
/// @author    axel
/// @date      2026-09-14
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
#include "ast/StateSpherical.hpp"
#include "ast/RTTIAPI.hpp"
#include "ast/CelestialBody.hpp"
#include "ast/RunTime.hpp"
#include "ast/AxesICRF.hpp"
#include "ast/TimePoint.hpp"
#include "ast/Constants.hpp"
#include "ast/Literals.hpp"
#include <cmath>
#include <cstdio>
#include <string>

AST_USING_NAMESPACE

namespace {

const double kKm = 1000.0;                      ///< km -> m

double dot3(const Vector3d& a, const Vector3d& b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

Vector3d sub3(const Vector3d& a, const Vector3d& b)
{
    return Vector3d{a[0] - b[0], a[1] - b[1], a[2] - b[2]};
}

double norm3(const Vector3d& a)
{
    return sqrt(dot3(a, a));
}

Vector3d unit3(const Vector3d& a)
{
    double m = norm3(a);
    return Vector3d{a[0] / m, a[1] / m, a[2] / m};
}

/// @brief 相对误差 |a-b| / |b|
double relDiff(const Vector3d& a, const Vector3d& b)
{
    return norm3(sub3(a, b)) / norm3(b);
}

}   // namespace

TEST(Spherical, CartToSpherical)
{
    {
        Vector3d pos{6600_km, 0, 0}, vel{0_km/sec, 6.79_km/sec, 3.69_km/sec};
        SphericalElem sph{};
        aCartToSpherical(pos, vel, sph);
        printf("%s\n", sph.toString().c_str());
        SphericalElem sphExpected{0, 0, 6600_km, 0_deg, 61.4782975379204331_deg, 7.7278845747073630_km/sec};
        printf("%s\n", sphExpected.toString().c_str());
        for(int i = 0; i < 6; ++i)
        {
            EXPECT_NEAR(sph[i], sphExpected[i], 1e-12);
        }
    }
    {
        Vector3d pos{1234_km, 5678_km, 9101_km}, vel{1_km/sec, 2_km/sec, 5_km/sec};
        SphericalElem sph{};
        aCartToSpherical(pos, vel, sph);
        printf("%s\n", sph.toString().c_str());
        SphericalElem sphExpected{
            77.7385777844615973_deg, 57.4437495923115620_deg, 10797.7146193071803282_km,
            79.2053794608183068_deg, 327.4159429986316923_deg - 360_deg, 5.4772255750516612_km/sec
        };
        printf("%s\n", sphExpected.toString().c_str());
        for(int i = 0; i < 6; ++i)
        {
            EXPECT_NEAR(sph[i], sphExpected[i], 1e-12);
        }
    }
}

/// @brief 解析已知值: 位置在+x轴、速度在当地水平面内
TEST(Spherical, KnownValues)
{
    const Vector3d pos{kKm * 6678.14, 0.0, 0.0};
    const Vector3d vel{0.0, kKm * 6.78953, kKm * 3.68641};

    SphericalElem sph{};
    errc_t rc = aCartToSpherical(pos, vel, sph);
    EXPECT_EQ(rc, eNoError);

    // 赤经、赤纬与地心距
    EXPECT_NEAR(sph.ra(), 0.0, 1e-12);
    EXPECT_NEAR(sph.dec(), 0.0, 1e-12);
    EXPECT_NEAR(sph.r(), norm3(pos), 1e-9);

    // 速度大小
    EXPECT_NEAR(sph.v(), norm3(vel), 1e-9);

    // 位置与速度正交, 航迹角为零
    EXPECT_NEAR(sph.fpa(), 0.0, 1e-12);

    // 航迹方位角: 自当地北(+z)向东(+y)量
    EXPECT_NEAR(sph.azi(), atan2(vel[1], vel[2]), 1e-12);

    // 几何不变量: 速度径向分量 = v*sin(航迹角)
    EXPECT_NEAR(dot3(pos, vel) / norm3(pos), sph.v() * sin(sph.fpa()), 1e-9);

    // 回合: 反算回直角坐标
    Vector3d posBack{}, velBack{};
    rc = aSphericalToCart(sph, posBack, velBack);
    EXPECT_EQ(rc, eNoError);
    EXPECT_NEAR(relDiff(posBack, pos), 0.0, 1e-12);
    EXPECT_NEAR(relDiff(velBack, vel), 0.0, 1e-12);
}

/// @brief 当地北/东/径向四个方向的航迹角与航迹方位角
TEST(Spherical, CardinalDirections)
{
    const double r = 7000e3;
    const double v = 7500.0;
    const Vector3d pos{r, 0.0, 0.0};

    SphericalElem sph{};

    // 当地北: 自当地北向东量的方位角为零
    EXPECT_EQ(aCartToSpherical(pos, Vector3d{0.0, 0.0, v}, sph), eNoError);
    EXPECT_NEAR(sph.fpa(), 0.0, 1e-12);
    EXPECT_NEAR(sph.azi(), 0.0, 1e-12);

    // 当地东
    EXPECT_EQ(aCartToSpherical(pos, Vector3d{0.0, v, 0.0}, sph), eNoError);
    EXPECT_NEAR(sph.fpa(), 0.0, 1e-12);
    EXPECT_NEAR(sph.azi(), kPI / 2, 1e-12);

    // 当地西
    EXPECT_EQ(aCartToSpherical(pos, Vector3d{0.0, -v, 0.0}, sph), eNoError);
    EXPECT_NEAR(sph.azi(), -kPI / 2, 1e-12);

    // 当地南
    EXPECT_EQ(aCartToSpherical(pos, Vector3d{0.0, 0.0, -v}, sph), eNoError);
    EXPECT_NEAR(fabs(sph.azi()), kPI, 1e-12);

    // 径向外向: 航迹角为正
    EXPECT_EQ(aCartToSpherical(pos, Vector3d{v, 0.0, 0.0}, sph), eNoError);
    EXPECT_NEAR(sph.fpa(), kPI / 2, 1e-12);

    // 径向内向: 航迹角为负
    EXPECT_EQ(aCartToSpherical(pos, Vector3d{-v, 0.0, 0.0}, sph), eNoError);
    EXPECT_NEAR(sph.fpa(), -kPI / 2, 1e-12);
}

/// @brief 各类轨道(含双曲能量、近极点、精确极点)的往返精度
TEST(Spherical, RoundTrip)
{
    struct Case
    {
        const char* name;
        Vector3d    pos;
        Vector3d    vel;
    };

    const Case cases[] = {
        {"LEO圆轨",     {kKm * 6678.14, 0.0, 0.0},                  {0.0, kKm * 6.78953, kKm * 3.68641}},
        {"太阳同步98度", {kKm * 500.0, kKm * 6800.0, kKm * 900.0},    {kKm * -6.1, kKm * 1.2, kKm * 2.8}},
        {"极轨",        {0.0, kKm * 7000.0, kKm * 100.0},            {kKm * 1.5, 0.0, kKm * 7.2}},
        {"大偏心率",     {kKm * 20000.0, kKm * -3000.0, kKm * 8000.0}, {kKm * -1.2, kKm * 4.8, kKm * 0.6}},
        {"双曲能量",     {kKm * -50000.0, kKm * 10000.0, kKm * 3000.0}, {kKm * 2.0, kKm * -9.0, kKm * 1.0}},
        {"近极点",       {kKm * 3.0, kKm * 4.0, kKm * 7000.0},       {kKm * 5.0, kKm * -1.0, kKm * 2.0}},
        {"精确极点",     {0.0, 0.0, kKm * 7000.0},                   {kKm * 3.0, kKm * 4.0, kKm * 1.0}},
    };

    for (const auto& c : cases)
    {
        SphericalElem sph{};
        errc_t rc = aCartToSpherical(c.pos, c.vel, sph);
        EXPECT_EQ(rc, eNoError) << c.name;

        Vector3d posBack{}, velBack{};
        rc = aSphericalToCart(sph, posBack, velBack);
        EXPECT_EQ(rc, eNoError) << c.name;

        EXPECT_NEAR(relDiff(posBack, c.pos), 0.0, 1e-12) << c.name;
        EXPECT_NEAR(relDiff(velBack, c.vel), 0.0, 1e-12) << c.name;
    }
}

/// @brief 退化与非法输入
TEST(Spherical, DegenerateAndInvalid)
{
    const Vector3d pos{kKm * 7000.0, 0.0, 0.0};
    const Vector3d vel{0.0, kKm * 7.5, 0.0};
    const Vector3d zero{0.0, 0.0, 0.0};

    SphericalElem sph{};
    Vector3d posBack{}, velBack{};

    // 位置为零矢量
    EXPECT_NE(aCartToSpherical(zero, vel, sph), eNoError);

    // 速度为零矢量: 航迹角与航迹方位角无定义
    EXPECT_NE(aCartToSpherical(pos, zero, sph), eNoError);

    // 反算: 地心距不大于零
    const SphericalElem badRadius{0.0, 0.0, 0.0, 0.0, 0.0, 100.0};
    EXPECT_NE(aSphericalToCart(badRadius, posBack, velBack), eNoError);

    // 反算: 速度小于零
    const SphericalElem badVel{0.0, 0.0, kKm * 7000.0, 0.0, 0.0, -1.0};
    EXPECT_NE(aSphericalToCart(badVel, posBack, velBack), eNoError);

    // 反算: 速度为零允许
    const SphericalElem zeroVel{0.0, 0.0, kKm * 7000.0, 0.0, 0.0, 0.0};
    EXPECT_EQ(aSphericalToCart(zeroVel, posBack, velBack), eNoError);
    EXPECT_NEAR(norm3(velBack), 0.0, 1e-12);
    EXPECT_NEAR(norm3(posBack), kKm * 7000.0, 1e-6);

    // 精确极点: 不失败, 赤经取约定值0, 赤纬为±90度, 且往返精确
    const Vector3d polePos{0.0, 0.0, kKm * 7000.0};
    const Vector3d poleVel{kKm * 3.0, kKm * 4.0, kKm * 1.0};
    errc_t rc = aCartToSpherical(polePos, poleVel, sph);
    EXPECT_EQ(rc, eNoError);
    EXPECT_NEAR(sph.ra(), 0.0, 1e-12);
    EXPECT_NEAR(fabs(sph.dec()), kPI / 2, 1e-12);

    rc = aSphericalToCart(sph, posBack, velBack);
    EXPECT_EQ(rc, eNoError);
    EXPECT_NEAR(relDiff(posBack, polePos), 0.0, 1e-12);
    EXPECT_NEAR(relDiff(velBack, poleVel), 0.0, 1e-12);
}

/// @brief 字符串输出
TEST(Spherical, ToString)
{
    const SphericalElem sph{kPI / 2, kPI / 6, 7000e3, 0.1, -0.2, 7500.0};
    const std::string str = sph.toString();
    printf("SphericalElem: %s\n", str.c_str());
    EXPECT_TRUE(str.find("SphericalElem") != std::string::npos);
}

/// @brief 球坐标状态类: 与直角坐标/修正轨道根数互换、工厂创建、RTTI属性
class SphericalStateTest : public ::testing::Test
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

    /// @brief 构造地球ICRF轴系下的球坐标状态
    HStateSpherical makeEarthState()
    {
        auto frame = aGetEarth()->makeFrame(aAxesICRF());
        frame->setName("ICRF");
        auto state = StateSpherical::MakeShared();
        state->setFrame(frame.get());
        state->setStateEpoch(TimePoint::FromUTC(2026, 9, 14, 4, 0, 0));
        return state;
    }
};

TEST_F(SphericalStateTest, StateIntegration)
{
    const Vector3d pos{kKm * 6678.14, 0.0, 0.0};
    const Vector3d vel{0.0, kKm * 6.78953, kKm * 3.68641};

    auto state = makeEarthState();

    CartState cart;
    cart.pos() = pos;
    cart.vel() = vel;
    errc_t rc = state->setState(cart);
    EXPECT_EQ(rc, eNoError);

    // 状态内的分量与直接转换一致
    SphericalElem sphState{};
    SphericalElem sphDirect{};
    state->getState(sphState);
    (void)aCartToSpherical(pos, vel, sphDirect);
    EXPECT_NEAR(sphState.ra(), sphDirect.ra(), 1e-12);
    EXPECT_NEAR(sphState.dec(), sphDirect.dec(), 1e-12);
    EXPECT_NEAR(sphState.r(), sphDirect.r(), 1e-6);
    EXPECT_NEAR(sphState.fpa(), sphDirect.fpa(), 1e-12);
    EXPECT_NEAR(sphState.azi(), sphDirect.azi(), 1e-12);
    EXPECT_NEAR(sphState.v(), sphDirect.v(), 1e-9);

    // 回代直角坐标
    CartState cartBack;
    rc = state->getState(cartBack);
    EXPECT_EQ(rc, eNoError);
    EXPECT_NEAR(relDiff(cartBack.pos(), pos), 0.0, 1e-12);
    EXPECT_NEAR(relDiff(cartBack.vel(), vel), 0.0, 1e-12);

    // 与修正轨道根数互换
    ModOrbElem modOrb{};
    rc = state->getState(modOrb);
    EXPECT_EQ(rc, eNoError);
    const ModOrbElem modOrbDirect = aCartToModOrbElem(pos, vel, state->getGM());
    EXPECT_NEAR(modOrb.getSMA(), modOrbDirect.getSMA(), 1e-3);
    EXPECT_NEAR(modOrb.getEcc(), modOrbDirect.getEcc(), 1e-12);
    EXPECT_NEAR(modOrb.getInc(), modOrbDirect.getInc(), 1e-12);

    rc = state->setState(modOrb);
    EXPECT_EQ(rc, eNoError);
    CartState cartFromModOrb;
    rc = state->getState(cartFromModOrb);
    EXPECT_EQ(rc, eNoError);
    EXPECT_NEAR(relDiff(cartFromModOrb.pos(), pos), 0.0, 1e-9);
    EXPECT_NEAR(relDiff(cartFromModOrb.vel(), vel), 0.0, 1e-9);

    // 由状态类型枚举创建(工厂)
    PState state2 = State::New(EStateType::eSpherical);
    ASSERT_TRUE(state2 != nullptr);
    EXPECT_EQ(state2->getStateType(), EStateType::eSpherical);
    delete state2;

    // 由已有状态拷贝构造
    PState state3 = State::New(*state, EStateType::eSpherical);
    ASSERT_TRUE(state3 != nullptr);
    EXPECT_EQ(state3->getStateType(), EStateType::eSpherical);
    CartState cart3;
    EXPECT_EQ(state3->getState(cart3), eNoError);
    EXPECT_NEAR(relDiff(cart3.pos(), pos), 0.0, 1e-12);
    EXPECT_NEAR(relDiff(cart3.vel(), vel), 0.0, 1e-12);
    delete state3;

    // RTTI: 按类名创建并读写属性
    Object* obj = aNewObject(StringView("StateSpherical"));
    ASSERT_TRUE(obj != nullptr);
    auto* sphObj = aobject_cast<StateSpherical*>(obj);
    ASSERT_TRUE(sphObj != nullptr);
    sphObj->attr("Radius") = "7000 km";
    EXPECT_NEAR(sphObj->getRadius(), 7000e3, 1e-6);
    sphObj->setFltPathAng(10.0_deg);
    EXPECT_NEAR(sphObj->getAttrDouble("FltPathAng"), 10.0_deg, 1e-12);
    sphObj->attr("Vel") = 7500.0;
    EXPECT_NEAR(sphObj->getVel(), 7500.0, 1e-12);
    delete sphObj;
}


GTEST_MAIN()

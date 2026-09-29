///
/// @file      testMixedSpherical.cpp
/// @brief     混合球坐标根数类型转换测试
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
#include "ast/StateMixedSpherical.hpp"
#include "ast/RTTIAPI.hpp"
#include "ast/CelestialBody.hpp"
#include "ast/BodyShape.hpp"
#include "ast/SpheroidShape.hpp"
#include "ast/SphereShape.hpp"
#include "ast/GeodeticPoint.hpp"
#include "ast/Rotation.hpp"
#include "ast/Transform.hpp"
#include "ast/RunTime.hpp"
#include "ast/AxesICRF.hpp"
#include "ast/Frame.hpp"
#include "ast/TimePoint.hpp"
#include "ast/Constants.hpp"
#include "ast/Literals.hpp"
#include "ast/EOP.hpp"
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

/// @brief 相对误差 |a-b| / |b|
double relDiff(const Vector3d& a, const Vector3d& b)
{
    return norm3(sub3(a, b)) / norm3(b);
}

Vector3d cross3(const Vector3d& a, const Vector3d& b)
{
    return Vector3d{a[1] * b[2] - a[2] * b[1],
                    a[2] * b[0] - a[0] * b[2],
                    a[0] * b[1] - a[1] * b[0]};
}

Vector3d unit3(const Vector3d& a)
{
    return a / norm3(a);
}

/// @brief 大地坐标 -> 固连系直角坐标(椭球)
Vector3d lla2fixed(const BodyShape& shape, double lat, double lon, double alt)
{
    Vector3d pos{};
    shape.transform(GeodeticPoint(lat, lon, alt), pos);
    return pos;
}

}   // namespace


// ---------------------------------------------------------------------------
// 纯数学转换(恒等旋转 + WGS84 椭球)
// ---------------------------------------------------------------------------

TEST(MixedSpherical, CartToMixedSpherical)
{
    aInitialize();
    aDataContext_GetEOP()->unload();
    {
        auto shape = aWGS84Spheroid();
        ASSERT_TRUE(shape != nullptr);
        Vector3d pos{6678137_m, 0, 0};
        Vector3d vel{0, 6789.53_m/sec, 3686.41_m/sec};
        MixedSphericalElem mixedSph{};
        auto tp = "14 Sep 2026 04:00:00.000 UTCG"_timepoint;
        auto earth = aGetEarth();
        Rotation rot;
        earth->getAxesInertial()->getTransformTo(earth->getAxesFixed(), tp, rot);
        errc_t rc = aCartToMixedSpherical(pos, vel, rot, *shape, mixedSph);
        EXPECT_EQ(rc, eNoError);
        printf("%s\n", mixedSph.toString().c_str());

        MixedSphericalElem mixedSphExpected{
            -52.8087312642573110_deg, 0.1505936983982324_deg, 300.0001465399117819_km, 
            0_deg, 61.5000261440186478_deg, 7.7257579763412210_km/sec
        };
        printf("%s\n", mixedSphExpected.toString().c_str());
        for(int i=0; i<6; ++i)
        {
            EXPECT_NEAR(mixedSph[i], mixedSphExpected[i], 1e-9) << i;
        }
    }
    /// 赤道解析算例: 位置在+x轴, 速度沿当地东向
    {
        auto shape = aWGS84Spheroid();
        ASSERT_TRUE(shape != nullptr);
        const double a = shape->majorAxis();
        EXPECT_NEAR(a, kEarthRadius_WGS84, 1e-9);

        const Vector3d pos{a, 0.0, 0.0};
        const Vector3d vel{0.0, 7800.0, 0.0};

        MixedSphericalElem mixedSph{};
        errc_t rc = aCartToMixedSpherical(pos, vel, Rotation::Identity(), *shape, mixedSph);
        EXPECT_EQ(rc, eNoError);
        printf("%s\n", mixedSph.toString().c_str());

        EXPECT_NEAR(mixedSph.lon(), 0.0, 1e-12);
        EXPECT_NEAR(mixedSph.lat(), 0.0, 1e-12);
        EXPECT_NEAR(mixedSph.alt(), 0.0, 1e-6);
        EXPECT_NEAR(mixedSph.fpa(), 0.0, 1e-12);
        EXPECT_NEAR(mixedSph.azi(), kHalfPI, 1e-12);
        EXPECT_NEAR(mixedSph.v(), 7800.0, 1e-12);
    }

}

/// @brief 往返: 大地坐标 -> 直角坐标 -> 大地坐标
TEST(MixedSpherical, RoundTrip)
{
    auto shape = aWGS84Spheroid();
    ASSERT_TRUE(shape != nullptr);

    const double lats[]  = {0.0, 30.0_deg, 45.0_deg, -60.0_deg, 85.0_deg, 28.5_deg};
    const double lons[]  = {0.0, 120.0_deg, -170.0_deg, 30.0_deg, 179.0_deg, -80.6_deg};
    const double alts[]  = {0.0, 400.0_km, 250.0_km, 1000.0_km, 0.0, 200.0_km};
    const double fpas[]  = {-30.0_deg, 0.0, 45.0_deg, 10.0_deg, -5.0_deg, 60.0_deg};
    const double azis[]  = {0.0, 90.0_deg, 135.0_deg, 180.0_deg, -90.0_deg, -170.0_deg};

    for(int i = 0; i < 6; ++i)
    {
        MixedSphericalElem elem{};
        elem.lon_ = lons[i];
        elem.lat_ = lats[i];
        elem.alt_ = alts[i];
        elem.fpa_ = fpas[i];
        elem.azi_ = azis[i];
        elem.v_   = 7500.0;

        Vector3d pos{}, vel{};
        errc_t rc = aMixedSphericalToCart(elem, Rotation::Identity(), *shape, pos, vel);
        EXPECT_EQ(rc, eNoError);

        MixedSphericalElem back{};
        rc = aCartToMixedSpherical(pos, vel, Rotation::Identity(), *shape, back);
        EXPECT_EQ(rc, eNoError);

        // 经度规范化到[0, 2π), 比较时取同一区间
        double lonExpected = lons[i];
        EXPECT_NEAR(back.lon(), lonExpected, 1e-12);
        EXPECT_NEAR(back.lat(), lats[i], 1e-12);
        EXPECT_NEAR(back.alt(), alts[i], 1e-6);
        EXPECT_NEAR(back.fpa(), fpas[i], 1e-12);
        EXPECT_NEAR(back.azi(), azis[i], 1e-12);
        EXPECT_NEAR(back.v(), 7500.0, 1e-9);

        // 反向: 直角坐标 -> 大地坐标 -> 直角坐标
        Vector3d posBack{}, velBack{};
        rc = aMixedSphericalToCart(back, Rotation::Identity(), *shape, posBack, velBack);
        EXPECT_EQ(rc, eNoError);
        EXPECT_NEAR(relDiff(posBack, pos), 0.0, 1e-12);
        EXPECT_NEAR(relDiff(velBack, vel), 0.0, 1e-12);
    }
}

/// @brief 当地水平基准: 航迹角以地心半径方向(而非椭球法线)为准
/// @details 在45度大地纬度处, 速度沿椭球切向朝正北, 此时
///          地心纬度 phi' 与大地纬度 phi 之差即为航迹角, 约 -0.1924 度;
///          若以椭球法线为基准则航迹角恰为0, 故本测试锁定了基准的选取。
TEST(MixedSpherical, GeocentricHorizontalConvention)
{
    auto shape = aWGS84Spheroid();
    ASSERT_TRUE(shape != nullptr);
    const SpheroidShape* spheroid = dynamic_cast<const SpheroidShape*>(shape);
    ASSERT_TRUE(spheroid != nullptr);
    const double ee = spheroid->flatFactor() * (2.0 - spheroid->flatFactor());

    const double lat = 45.0_deg;
    const double v = 7800.0;

    // 位置: 45度大地纬度、经度0、高度0; 速度: 沿椭球切向朝正北
    const Vector3d pos = lla2fixed(*shape, lat, 0.0, 0.0);
    const Vector3d north{ -sin(lat), 0.0, cos(lat) };
    const Vector3d vel = north * v;

    MixedSphericalElem elem{};
    errc_t rc = aCartToMixedSpherical(pos, vel, Rotation::Identity(), *shape, elem);
    EXPECT_EQ(rc, eNoError);

    // 地心纬度, 由位置矢量直接给出
    const double latCentric = atan2(pos[2], hypot(pos[0], pos[1]));
    // 地心纬度与大地纬度之差即航迹角(负值, 速度相对当地水平面下倾)
    EXPECT_NEAR(elem.fpa(), latCentric - lat, 1e-12);
    EXPECT_NEAR(elem.fpa(), -0.1924_deg, 1e-4_deg);
    EXPECT_LT(elem.fpa(), -0.19_deg);           // 若改用椭球法线基准, 此处将为0
    EXPECT_NEAR(elem.azi(), 0.0, 1e-12);

    // 椭球扁率确实进入了计算
    EXPECT_GT(ee, 0.0);
    EXPECT_NEAR(elem.lat(), lat, 1e-12);
    EXPECT_NEAR(elem.alt(), 0.0, 1e-6);

    // 速度沿当地东向时, 两种基准一致: 航迹角为零、方位角为90度
    const Vector3d east{ 0.0, 1.0, 0.0 };
    MixedSphericalElem elemEast{};
    rc = aCartToMixedSpherical(pos, east * v, Rotation::Identity(), *shape, elemEast);
    EXPECT_EQ(rc, eNoError);
    EXPECT_NEAR(elemEast.fpa(), 0.0, 1e-12);
    EXPECT_NEAR(elemEast.azi(), kHalfPI, 1e-12);
}

/// @brief 航迹方位角: 北/东/西/南与径向朝外/朝内
TEST(MixedSpherical, CardinalDirections)
{
    auto shape = aWGS84Spheroid();
    ASSERT_TRUE(shape != nullptr);

    const Vector3d pos = lla2fixed(*shape, 0.0, 0.0, 500.0_km);
    const double r = norm3(pos);
    const Vector3d rhat = pos / r;
    const Vector3d ehat{ 0.0, 1.0, 0.0 };
    const Vector3d nhat = cross3(rhat, ehat);
    const double v = 7000.0;

    struct Case
    {
        Vector3d dir;
        double   fpa;
        double   azi;
    };
    const Case cases[] = {
        {nhat,          0.0,     0.0},
        {ehat,          0.0,     kHalfPI},
        {-ehat,         0.0,    -kHalfPI},
        {-nhat,         0.0,     kPI},
        {rhat,          kHalfPI, 0.0},
        {-rhat,        -kHalfPI, 0.0},
    };

    for(const auto& c : cases)
    {
        MixedSphericalElem elem{};
        errc_t rc = aCartToMixedSpherical(pos, c.dir * v, Rotation::Identity(), *shape, elem);
        EXPECT_EQ(rc, eNoError);
        EXPECT_NEAR(elem.fpa(), c.fpa, 1e-12);
        // 沿径向时方位角由无定义的 atan2(0, 0) 约定为0
        EXPECT_NEAR(elem.azi(), c.azi, 1e-12);
    }
}

/// @brief 球体退化: 与球坐标根数的转换结果完全一致
/// @details 零扁率时大地坐标退化为地心坐标, 混合球坐标应与球坐标逐项相同,
///          由此一次性验证当地东/北轴系的复用与各角度的符号约定。
TEST(MixedSpherical, SphereReduction)
{
    const double radius = 6800.0_km;
    SphereShape shape(radius);

    const Vector3d pos{1234.0_km, 5678.0_km, 9101.0_km};
    const Vector3d vel{1.0_km, 2.0_km, 5.0_km};

    MixedSphericalElem mixedSph{};
    errc_t rc = aCartToMixedSpherical(pos, vel, Rotation::Identity(), shape, mixedSph);
    EXPECT_EQ(rc, eNoError);

    SphericalElem sph{};
    rc = aCartToSpherical(pos, vel, sph);
    EXPECT_EQ(rc, eNoError);

    EXPECT_NEAR(mixedSph.lon(), sph.ra(), 1e-12);
    EXPECT_NEAR(mixedSph.lat(), sph.dec(), 1e-12);
    EXPECT_NEAR(mixedSph.alt(), sph.r() - radius, 1e-6);
    EXPECT_NEAR(mixedSph.fpa(), sph.fpa(), 1e-12);
    EXPECT_NEAR(mixedSph.azi(), sph.azi(), 1e-12);
    EXPECT_NEAR(mixedSph.v(), sph.v(), 1e-9);

    // 反向亦应一致
    Vector3d posMixed{}, velMixed{}, posSph{}, velSph{};
    EXPECT_EQ(aMixedSphericalToCart(mixedSph, Rotation::Identity(), shape, posMixed, velMixed), eNoError);
    EXPECT_EQ(aSphericalToCart(sph, posSph, velSph), eNoError);
    EXPECT_NEAR(relDiff(posMixed, posSph), 0.0, 1e-12);
    EXPECT_NEAR(relDiff(velMixed, velSph), 0.0, 1e-12);
}

/// @brief 非恒等旋转: 固连系到惯性系差一个绕自转轴的旋转, 往返一致
TEST(MixedSpherical, NonIdentityRotation)
{
    auto shape = aWGS84Spheroid();
    ASSERT_TRUE(shape != nullptr);

    const double lat = 28.5_deg;
    const double lon = 100.0_deg;
    const Vector3d posFixed = lla2fixed(*shape, lat, lon, 200.0_km);

    MixedSphericalElem elemFixed{};
    elemFixed.lon_ = lon;      elemFixed.lat_ = lat;       elemFixed.alt_ = 200.0_km;
    elemFixed.fpa_ = 15.0_deg; elemFixed.azi_ = 45.0_deg;  elemFixed.v_ = 7600.0;

    Vector3d posFromElem{}, velFixed{};
    ASSERT_EQ(aMixedSphericalToCart(elemFixed, Rotation::Identity(), *shape, posFromElem, velFixed), eNoError);
    EXPECT_NEAR(relDiff(posFromElem, posFixed), 0.0, 1e-12);

    // 固连系到惯性系的旋转: 绕自转轴转过30度
    const Rotation fixedToInertial(30.0_deg, Vector3d{0.0, 0.0, 1.0});
    const Vector3d posInertial = fixedToInertial.transformVector(posFixed);
    const Vector3d velInertial = fixedToInertial.transformVector(velFixed);

    // 两个系下的位置只差一个绕自转轴的旋转: 纬度、高度不变, 经度相差30度
    const double lonInertial = atan2(posInertial[1], posInertial[0]);
    EXPECT_NEAR(fabs(lonInertial - lon), 30.0_deg, 1e-12);
    EXPECT_NEAR(norm3(posInertial), norm3(posFixed), 1e-9);

    // 换算回固连系, 应还原出原大地坐标(各分量均不受轴系旋转影响)
    MixedSphericalElem elemRot{};
    errc_t rc = aCartToMixedSpherical(posInertial, velInertial, fixedToInertial.inverse(), *shape, elemRot);
    EXPECT_EQ(rc, eNoError);
    EXPECT_NEAR(elemRot.lon(), lon, 1e-12);
    EXPECT_NEAR(elemRot.lat(), lat, 1e-12);
    EXPECT_NEAR(elemRot.alt(), 200.0_km, 1e-6);
    EXPECT_NEAR(elemRot.fpa(), 15.0_deg, 1e-12);
    EXPECT_NEAR(elemRot.azi(), 45.0_deg, 1e-12);
    EXPECT_NEAR(elemRot.v(), 7600.0, 1e-9);

    // 往返
    Vector3d posBack{}, velBack{};
    rc = aMixedSphericalToCart(elemRot, fixedToInertial, *shape, posBack, velBack);
    EXPECT_EQ(rc, eNoError);
    EXPECT_NEAR(relDiff(posBack, posInertial), 0.0, 1e-12);
    EXPECT_NEAR(relDiff(velBack, velInertial), 0.0, 1e-12);
}

/// @brief 极点邻域: 避开精确极点, 仍可精确往返
/// @note  精确极点处的大地坐标反解存在已知缺陷(见 aBodyFixedToGeodetic), 故此处不覆盖。
TEST(MixedSpherical, NearPole)
{
    auto shape = aWGS84Spheroid();
    ASSERT_TRUE(shape != nullptr);

    const double lat = 89.9_deg;
    MixedSphericalElem elem{};
    elem.lon_ = 120.0_deg;  elem.lat_ = lat;  elem.alt_ = 100.0_km;
    elem.fpa_ = -20.0_deg;  elem.azi_ = 30.0_deg;  elem.v_ = 7500.0;

    Vector3d pos{}, vel{};
    errc_t rc = aMixedSphericalToCart(elem, Rotation::Identity(), *shape, pos, vel);
    EXPECT_EQ(rc, eNoError);

    MixedSphericalElem back{};
    rc = aCartToMixedSpherical(pos, vel, Rotation::Identity(), *shape, back);
    EXPECT_EQ(rc, eNoError);
    EXPECT_NEAR(back.lat(), lat, 1e-12);
    EXPECT_NEAR(back.alt(), 100.0_km, 1e-6);
    EXPECT_NEAR(back.fpa(), -20.0_deg, 1e-12);
    EXPECT_NEAR(back.v(), 7500.0, 1e-9);
}

/// @brief 非法输入
TEST(MixedSpherical, InvalidInput)
{
    auto shape = aWGS84Spheroid();
    ASSERT_TRUE(shape != nullptr);

    const Vector3d pos{kKm * 7000.0, 0.0, 0.0};
    const Vector3d zero{0.0, 0.0, 0.0};
    MixedSphericalElem elem{};

    // 零位置、零速度
    EXPECT_EQ(aCartToMixedSpherical(zero, pos, Rotation::Identity(), *shape, elem), eErrorInvalidParam);
    EXPECT_EQ(aCartToMixedSpherical(pos, zero, Rotation::Identity(), *shape, elem), eErrorInvalidParam);

    // 反向: 速度为负、位置退化到椭球中心
    MixedSphericalElem bad{};
    bad.v_ = -1.0;
    Vector3d p{}, v{};
    EXPECT_EQ(aMixedSphericalToCart(bad, Rotation::Identity(), *shape, p, v), eErrorInvalidParam);
    EXPECT_NEAR(norm3(p), 0.0, 1e-12);

    bad = MixedSphericalElem{};
    bad.alt_ = -shape->majorAxis();         // 位置退化到中心
    EXPECT_EQ(aMixedSphericalToCart(bad, Rotation::Identity(), *shape, p, v), eErrorInvalidParam);

    // 速度为零是合法的
    MixedSphericalElem still{};
    still.lat_ = 10.0_deg;
    still.v_ = 0.0;
    EXPECT_EQ(aMixedSphericalToCart(still, Rotation::Identity(), *shape, p, v), eNoError);
    EXPECT_NEAR(norm3(v), 0.0, 1e-12);
}

/// @brief 字符串输出
TEST(MixedSpherical, ToString)
{
    MixedSphericalElem elem{};
    elem.lon_ = 30.0_deg;
    elem.lat_ = 20.0_deg;
    elem.alt_ = 500.0_km;
    elem.fpa_ = 5.0_deg;
    elem.azi_ = 90.0_deg;
    elem.v_ = 7500.0;

    const std::string str = elem.toString();
    printf("MixedSphericalElem: %s\n", str.c_str());
    EXPECT_TRUE(str.find("MixedSphericalElem") != std::string::npos);
}


// ---------------------------------------------------------------------------
// 状态类: 与直角坐标/修正轨道根数互换、工厂创建、RTTI属性
// ---------------------------------------------------------------------------

class MixedSphericalStateTest : public ::testing::Test
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

    /// @brief 构造地球ICRF轴系下的混合球坐标状态
    HStateMixedSpherical makeEarthState(const TimePoint& epoch)
    {
        auto frame = aGetEarth()->makeFrame(aAxesICRF());
        frame->setName("ICRF");
        auto state = StateMixedSpherical::MakeShared();
        state->setFrame(frame.get());
        state->setStateEpoch(epoch);
        return state;
    }
};

TEST_F(MixedSphericalStateTest, StateIntegration)
{
    auto earth = aGetEarth();
    ASSERT_TRUE(earth != nullptr);
    auto shape = earth->getShape();
    ASSERT_TRUE(shape != nullptr);
    ASSERT_GT(shape->majorAxis(), 0.0);

    const TimePoint epoch = TimePoint::FromUTC(2026, 9, 14, 4, 0, 0);
    auto frame = earth->makeFrame(aAxesICRF());
    frame->setName("ICRF");

    // 以地球固连系下的位置构造惯性系直角坐标状态
    const double lat = 28.5_deg;
    const double lon = -80.6_deg;
    const Vector3d posFixed = lla2fixed(*shape, lat, lon, 300.0_km);

    Transform transform;
    ASSERT_EQ(aFrameTransform(earth->getFrameFixed(), frame.get(), epoch, transform), eNoError);
    const Rotation fixedToInertial = transform.getRotation();

    const Vector3d posInertial = fixedToInertial.transformVector(posFixed);
    // 惯性系速度: 固连系下的“东向”速度叠加地球自转的牵连速度
    const Vector3d velFixed = Vector3d{-sin(lon), cos(lon), 0.0} * 7000.0;
    const Vector3d velInertial = fixedToInertial.transformVector(velFixed);

    auto state = makeEarthState(epoch);

    CartState cart{};
    cart.pos() = posInertial;
    cart.vel() = velInertial;
    errc_t rc = state->setState(cart);
    EXPECT_EQ(rc, eNoError);

    // 状态内的分量与直接转换一致
    MixedSphericalElem fromState{};
    MixedSphericalElem direct{};
    state->getState(fromState);
    (void)aCartToMixedSpherical(posInertial, velInertial,
                                fixedToInertial.inverse(), *shape, direct);
    EXPECT_NEAR(fromState.lon(), direct.lon(), 1e-12);
    EXPECT_NEAR(fromState.lat(), direct.lat(), 1e-12);
    EXPECT_NEAR(fromState.alt(), direct.alt(), 1e-6);
    EXPECT_NEAR(fromState.fpa(), direct.fpa(), 1e-12);
    EXPECT_NEAR(fromState.azi(), direct.azi(), 1e-12);
    EXPECT_NEAR(fromState.v(), direct.v(), 1e-9);

    // 大地坐标确实是固连系下的位置
    EXPECT_NEAR(fromState.lat(), lat, 1e-9);
    double lonExpected = lon;
    EXPECT_NEAR(fromState.lon(), lonExpected, 1e-9);
    EXPECT_NEAR(fromState.alt(), 300.0_km, 1e-4);

    // 回代直角坐标
    CartState cartBack{};
    rc = state->getState(cartBack);
    EXPECT_EQ(rc, eNoError);
    EXPECT_NEAR(relDiff(cartBack.pos(), posInertial), 0.0, 1e-9);
    EXPECT_NEAR(relDiff(cartBack.vel(), velInertial), 0.0, 1e-9);

    // 与大地坐标接口一致
    GeodeticPoint geodetic;
    rc = state->getStateIn(earth, geodetic);
    EXPECT_EQ(rc, eNoError);
    double lonGeo = geodetic.longitude();
    EXPECT_NEAR(lonGeo, fromState.lon(), 1e-9);
    EXPECT_NEAR(geodetic.latitude(), fromState.lat(), 1e-9);
    EXPECT_NEAR(geodetic.altitude(), fromState.alt(), 1e-4);

    // 与修正轨道根数互换
    ModOrbElem modOrb{};
    rc = state->getState(modOrb);
    EXPECT_EQ(rc, eNoError);
    const ModOrbElem modOrbDirect = aCartToModOrbElem(posInertial, velInertial, state->getGM());
    EXPECT_NEAR(modOrb.getSMA(), modOrbDirect.getSMA(), 1e-3);
    EXPECT_NEAR(modOrb.getEcc(), modOrbDirect.getEcc(), 1e-12);
    EXPECT_NEAR(modOrb.getInc(), modOrbDirect.getInc(), 1e-12);

    rc = state->setState(modOrb);
    EXPECT_EQ(rc, eNoError);
    CartState cartFromModOrb{};
    rc = state->getState(cartFromModOrb);
    EXPECT_EQ(rc, eNoError);
    EXPECT_NEAR(relDiff(cartFromModOrb.pos(), posInertial), 0.0, 1e-8);
    EXPECT_NEAR(relDiff(cartFromModOrb.vel(), velInertial), 0.0, 1e-8);
}

/// @brief 共转发射点: 速度取固连系的牵连速度
/// @details 该速度在固连系下与当地东向重合, 但航迹方位角按惯性系下的当地东、北向度量:
///          惯性极与固连极相差岁差、章动与极移, 故方位角与地理正东(90度)并不相等。
TEST_F(MixedSphericalStateTest, CoRotatingLaunchSite)
{
    auto earth = aGetEarth();
    ASSERT_TRUE(earth != nullptr);
    auto shape = earth->getShape();
    ASSERT_TRUE(shape != nullptr);

    const TimePoint epoch = TimePoint::FromUTC(2026, 9, 14, 4, 0, 0);
    auto frame = earth->makeFrame(aAxesICRF());
    frame->setName("ICRF");

    const GeodeticPoint site(28.5_deg, -80.6_deg, 0.0);
    const Vector3d posFixed = shape->transform(site);

    // 固连系下的牵连速度 omega x r, omega 沿固连系 z 轴
    const Vector3d omega{0.0, 0.0, kEarthAngVel};
    const Vector3d velFixed = cross3(omega, posFixed);

    Transform transform;
    ASSERT_EQ(aFrameTransform(earth->getFrameFixed(), frame.get(), epoch, transform), eNoError);
    const Rotation fixedToInertial = transform.getRotation();

    const Vector3d posInertial = fixedToInertial.transformVector(posFixed);
    const Vector3d velInertial = fixedToInertial.transformVector(velFixed);

    auto state = makeEarthState(epoch);
    CartState cart{};
    cart.pos() = posInertial;
    cart.vel() = velInertial;
    errc_t rc = state->setState(cart);
    EXPECT_EQ(rc, eNoError);

    // 牵连速度与位置矢量正交, 航迹角恒为零(与轴系无关)
    EXPECT_NEAR(state->getFltPathAng(), 0.0, 1e-9);
    EXPECT_NEAR(state->getFltPathAng(), state->getVertFltPathAng() - kHalfPI, 1e-12);
    EXPECT_NEAR(state->getVel(), kEarthAngVel * hypot(posFixed[0], posFixed[1]), 1e-6);

    // 航迹方位角在惯性系下度量: 当地东向取惯性极叉乘位置矢量
    const Vector3d rhat = unit3(posInertial);
    const Vector3d ehat = unit3(cross3(Vector3d{0.0, 0.0, 1.0}, rhat));
    const Vector3d nhat = cross3(rhat, ehat);
    const double aziExpected = atan2(dot3(velInertial, ehat), dot3(velInertial, nhat));
    EXPECT_NEAR(state->getFltPathAzi(), aziExpected, 1e-12);

    // 与固连系下的方位角(恒为90度)不相等, 相差惯性极与固连极夹角在本地的投影
    EXPECT_GT(fabs(state->getFltPathAzi() - kHalfPI), 0.01_deg);

    // 位置分量仍为固连系下的大地坐标
    EXPECT_NEAR(state->getLat(), site.latitude(), 1e-9);
    EXPECT_NEAR(state->getAlt(), 0.0, 1e-3);
}

TEST_F(MixedSphericalStateTest, Factory)
{
    const TimePoint epoch = TimePoint::FromUTC(2026, 9, 14, 4, 0, 0);
    auto state = makeEarthState(epoch);

    auto earth = aGetEarth();
    auto shape = earth->getShape();
    ASSERT_TRUE(shape != nullptr);

    const Vector3d posFixed = lla2fixed(*shape, 10.0_deg, 20.0_deg, 400.0_km);
    auto frame = earth->makeFrame(aAxesICRF());
    frame->setName("ICRF");
    Transform transform;
    ASSERT_EQ(aFrameTransform(earth->getFrameFixed(), frame.get(), epoch, transform), eNoError);

    CartState cart{};
    cart.pos() = transform.transformPosition(posFixed);
    cart.vel() = Vector3d{0.0, 1.0, 0.0} * 7000.0;
    ASSERT_EQ(state->setState(cart), eNoError);

    // 由状态类型枚举创建(工厂)
    PState state2 = State::New(EStateType::eMixedSpherical);
    ASSERT_TRUE(state2 != nullptr);
    EXPECT_EQ(state2->getStateType(), EStateType::eMixedSpherical);
    delete state2;

    // 由已有状态拷贝构造
    PState state3 = State::New(*state, EStateType::eMixedSpherical);
    ASSERT_TRUE(state3 != nullptr);
    EXPECT_EQ(state3->getStateType(), EStateType::eMixedSpherical);
    CartState cart3{};
    EXPECT_EQ(state3->getState(cart3), eNoError);
    EXPECT_NEAR(relDiff(cart3.pos(), cart.pos()), 0.0, 1e-9);
    EXPECT_NEAR(relDiff(cart3.vel(), cart.vel()), 0.0, 1e-9);
    delete state3;

    // RTTI: 按类名创建并读写属性
    Object* obj = aNewObject(StringView("StateMixedSpherical"));
    ASSERT_TRUE(obj != nullptr);
    auto* mixedObj = aobject_cast<StateMixedSpherical*>(obj);
    ASSERT_TRUE(mixedObj != nullptr);
    mixedObj->attr("Alt") = "500 km";
    EXPECT_NEAR(mixedObj->getAlt(), 500e3, 1e-6);
    mixedObj->setFltPathAng(10.0_deg);
    EXPECT_NEAR(mixedObj->getAttrDouble("FltPathAng"), 10.0_deg, 1e-12);
    mixedObj->setVertFltPathAng(60.0_deg);
    EXPECT_NEAR(mixedObj->getFltPathAng(), 30.0_deg, 1e-12);
    mixedObj->attr("Vel") = 7500.0;
    EXPECT_NEAR(mixedObj->getVel(), 7500.0, 1e-12);
    delete mixedObj;
}

/// @brief 缺少参考坐标系或参考椭球时应报错而非崩溃
TEST_F(MixedSphericalStateTest, MissingContext)
{
    CartState cart{};
    cart.pos() = Vector3d{kKm * 7000.0, 0.0, 0.0};
    cart.vel() = Vector3d{0.0, kKm * 7.0, 0.0};

    PState state = State::New(EStateType::eMixedSpherical);
    ASSERT_TRUE(state != nullptr);

    // 未设置参考坐标系
    EXPECT_NE(state->setState(cart), eNoError);
    CartState out{};
    EXPECT_NE(state->getState(out), eNoError);

    delete state;
}


GTEST_MAIN()

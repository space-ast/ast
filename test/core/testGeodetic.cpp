///
/// @file      testGeodetic.cpp
/// @brief     大地坐标根数类型转换测试
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
/// 您使用本软件所产生的风险，需由您自行承担。


#include "ast/AstTestMacro.h"
#include "ast/OrbitElement.hpp"
#include "ast/StateGeodetic.hpp"
#include "ast/RTTIAPI.hpp"
#include "ast/CelestialBody.hpp"
#include "ast/BodyShape.hpp"
#include "ast/SpheroidShape.hpp"
#include "ast/SphereShape.hpp"
#include "ast/EllipsoidShape.hpp"
#include "ast/GeodeticPoint.hpp"
#include "ast/Rotation.hpp"
#include "ast/Transform.hpp"
#include "ast/KinematicTransform.hpp"
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

/// @brief 相对误差, 分母加1以兼容零矢量(零速度是合法工况)
double relDiffSafe(const Vector3d& a, const Vector3d& b)
{
    return norm3(sub3(a, b)) / (1.0 + norm3(b));
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

/// @brief 第一偏心率平方
double ecc2(const BodyShape& shape)
{
    const double f = static_cast<const SpheroidShape&>(shape).flatFactor();
    return f * (2.0 - f);
}

/// @brief 卯酉圈曲率半径
double primeVerticalRadius(const BodyShape& shape, double lat)
{
    const double e2 = ecc2(shape);
    const double s = sin(lat);
    return shape.majorAxis() / sqrt(1.0 - e2 * s * s);
}

/// @brief 子午圈曲率半径
double meridionalRadius(const BodyShape& shape, double lat)
{
    const double e2 = ecc2(shape);
    const double s = sin(lat);
    const double denom = 1.0 - e2 * s * s;
    return shape.majorAxis() * (1.0 - e2) / (denom * sqrt(denom));
}

/// @brief 大地坐标 -> 固连系直角坐标
Vector3d lla2fixed(const BodyShape& shape, double lat, double lon, double alt)
{
    Vector3d pos{};
    shape.transform(GeodeticPoint(lat, lon, alt), pos);
    return pos;
}

/// @brief 构造大地坐标根数(变化率依次为经度率、纬度率、高度率)
GeodeticElem makeElem(double lat, double lon, double alt,
                      double lonRate, double latRate, double altRate)
{
    GeodeticElem elem{};
    elem.lon_ = lon;
    elem.lat_ = lat;
    elem.alt_ = alt;
    elem.lonRate_ = lonRate;
    elem.latRate_ = latRate;
    elem.altRate_ = altRate;
    return elem;
}

}   // namespace


TEST(Geodetic, CartToGeodetic)
{
    auto earth = aGetEarth();
    auto shape = earth->getShape();
    {
        Vector3d pos{4036800_m, -5319900_m, 17440_m};
        Vector3d vel{5015_m/sec, 3817_m/sec, 3686_m/sec};
        GeodeticElem geoticElem{};
        ASSERT_EQ(aCartToGeodetic(pos, vel, *shape, geoticElem), eNoError);
        GeodeticElem geoticElemExpected{
            0.1505915129124376_deg, -52.8083639372511300_deg, 299.9912078714071413_km,
            0.0318280608444481_deg/sec, 0.0540719414328207_deg/sec, 0.0004778574962943_km/sec
        };
        for(int i=0;i<6;i++)
        {
            EXPECT_NEAR(geoticElem[i], geoticElemExpected[i], 2e-9) << i;
        }
    }
}


TEST(Geodetic, GeodeticToCart)
{
    auto earth = aGetEarth();
    auto shape = earth->getShape();
    {
        GeodeticElem GeodeticElem
        {
            0.1499999999999999_deg, -52.7999999999999829_deg, 299.9999999999996021_km,
            0.0320000000000000_deg/sec, 0.0540000000000000_deg/sec, 0.0010000000000008_km/sec
        };
        Vector3d pos{}, vel{};
        ASSERT_EQ(aGeodeticToCart(GeodeticElem, *shape, pos, vel), eNoError);
        printf("pos: %s\n", pos.toString().c_str());
        printf("vel: %s\n", vel.toString().c_str());
        Vector3d posExpected{4037581.9709812141954899_m, -5319317.8046476449817419_m, 17371.5200733465462690_m};
        Vector3d velExpected{5008.0776446801501152_m/sec, 3812.2628332697418045_m/sec, 3705.9185959816118157_m/sec};
        for(int i=0;i<3;i++)
        {
            EXPECT_NEAR(pos[i], posExpected[i], 1e-9) << i;
        }
        for(int i=0;i<3;i++)
        {
            EXPECT_NEAR(vel[i], velExpected[i], 1e-11) << i;
        }
        
    }
}

// ---------------------------------------------------------------------------
// 纯数学转换(天体固连系下的位置与速度)
// ---------------------------------------------------------------------------

/// @brief 赤道上的解析解: 位置、东向速度与经度率
TEST(Geodetic, EquatorAnalytic)
{
    auto shape = aWGS84Spheroid();
    ASSERT_TRUE(shape != nullptr);
    const double a = shape->majorAxis();
    EXPECT_NEAR(a, kEarthRadius_WGS84, 1e-6);

    // 赤道上经度0处, 高度0, 固连系下正东方向的速度
    const Vector3d pos{a, 0.0, 0.0};
    const Vector3d vel{0.0, 7800.0, 0.0};

    GeodeticElem elem{};
    ASSERT_EQ(aCartToGeodetic(pos, vel, *shape, elem), eNoError);
    EXPECT_NEAR(elem.lat(), 0.0, 1e-15);
    EXPECT_NEAR(elem.lon(), 0.0, 1e-15);
    EXPECT_NEAR(elem.alt(), 0.0, 1e-6);
    // 赤道上当地东向恰为 +y, 纬度率与高度率为零
    EXPECT_NEAR(elem.lonRate(), 7800.0 / a, 1e-15);
    EXPECT_NEAR(elem.lonRate() * kRadToDeg, 0.0700686, 1e-6);
    EXPECT_NEAR(elem.latRate(), 0.0, 1e-15);
    EXPECT_NEAR(elem.altRate(), 0.0, 1e-12);

    // 反向: 经度率取地球自转角速度时, 速度应为赤道上的自转线速度 omega*a
    const GeodeticElem spin = makeElem(0.0, 0.0, 0.0, kEarthAngVel, 0.0, 0.0);
    Vector3d posOut{}, velOut{};
    ASSERT_EQ(aGeodeticToCart(spin, *shape, posOut, velOut), eNoError);
    EXPECT_NEAR(relDiff(posOut, pos), 0.0, 1e-12);
    EXPECT_NEAR(velOut[0], 0.0, 1e-9);
    EXPECT_NEAR(velOut[1], kEarthAngVel * a, 1e-12);
    EXPECT_NEAR(velOut[2], 0.0, 1e-9);
    EXPECT_NEAR(velOut[1], 465.1011, 1e-3);
}

/// @brief 纬度的度量因子是子午圈曲率半径 M, 而不是卯酉圈曲率半径 N
/// @details 若误用 N, 纬度率会有约 0.34% 的偏差, 本用例专门钉住这一点。
TEST(Geodetic, MeridianCurvatureNotPrimeVertical)
{
    auto shape = aWGS84Spheroid();
    ASSERT_TRUE(shape != nullptr);

    const double lat = 45.0_deg;
    const double lon = 0.0;
    const double alt = 0.0;
    const double V = 1000.0;

    const GeodeticPoint detic(lat, lon, alt);
    const Vector3d north = detic.getNorth();

    GeodeticElem elem{};
    ASSERT_EQ(aCartToGeodetic(lla2fixed(*shape, lat, lon, alt), north * V, *shape, elem), eNoError);

    const double N = primeVerticalRadius(*shape, lat);
    const double M = meridionalRadius(*shape, lat);
    EXPECT_GT(N, M);
    // WGS84 下 45 度处 N 与 M 相差约 0.34%
    EXPECT_GT(fabs(N - M) / M, 0.003);

    EXPECT_NEAR(elem.lat(), lat, 1e-12);
    EXPECT_NEAR(elem.lonRate(), 0.0, 1e-15);
    EXPECT_NEAR(elem.altRate(), 0.0, 1e-9);
    EXPECT_NEAR(elem.latRate(), V / M, 1e-15);
    // 用 N 代替 M 会明显偏离
    EXPECT_GT(fabs(elem.latRate() - V / N), 0.003 * (V / M));
    // 与公认数值量级一致
    EXPECT_NEAR(M, 6.367382e6, 1e-3 * 6.367382e6);

    // 数值验证几何关系: dpos/dlat = (M+alt)*north
    const double delta = 1.0e-6;
    const Vector3d dp =
        (shape->transform(GeodeticPoint(lat + delta, lon, alt)) -
         shape->transform(GeodeticPoint(lat - delta, lon, alt))) / (2.0 * delta);
    EXPECT_NEAR(norm3(dp), M + alt, 1e-6 * (M + alt));
    EXPECT_NEAR(relDiff(unit3(dp), north), 0.0, 1e-8);
}

/// @brief 四个基本方向的速度只改变对应的变化率
TEST(Geodetic, CardinalDirections)
{
    auto shape = aWGS84Spheroid();
    ASSERT_TRUE(shape != nullptr);

    const double lat = 30.0_deg;
    const double lon = 60.0_deg;
    const double alt = 500.0_km;
    const double V = 100.0;

    const GeodeticPoint detic(lat, lon, alt);
    const Vector3d east = detic.getEast();
    const Vector3d north = detic.getNorth();
    const Vector3d up = detic.getZenith();
    const Vector3d pos = lla2fixed(*shape, lat, lon, alt);

    const double N = primeVerticalRadius(*shape, lat);
    const double M = meridionalRadius(*shape, lat);
    const double rho = (N + alt) * cos(lat);

    GeodeticElem elem{};

    ASSERT_EQ(aCartToGeodetic(pos, east * V, *shape, elem), eNoError);
    EXPECT_NEAR(elem.lonRate(), V / rho, 1e-15);
    EXPECT_NEAR(elem.latRate(), 0.0, 1e-15);
    EXPECT_NEAR(elem.altRate(), 0.0, 1e-12);

    ASSERT_EQ(aCartToGeodetic(pos, east * (-V), *shape, elem), eNoError);
    EXPECT_NEAR(elem.lonRate(), -V / rho, 1e-15);

    ASSERT_EQ(aCartToGeodetic(pos, north * V, *shape, elem), eNoError);
    EXPECT_NEAR(elem.lonRate(), 0.0, 1e-15);
    EXPECT_NEAR(elem.latRate(), V / (M + alt), 1e-15);
    EXPECT_NEAR(elem.altRate(), 0.0, 1e-12);

    ASSERT_EQ(aCartToGeodetic(pos, up * V, *shape, elem), eNoError);
    EXPECT_NEAR(elem.lonRate(), 0.0, 1e-15);
    EXPECT_NEAR(elem.latRate(), 0.0, 1e-15);
    EXPECT_NEAR(elem.altRate(), V, 1e-12);

    ASSERT_EQ(aCartToGeodetic(pos, up * (-V), *shape, elem), eNoError);
    EXPECT_NEAR(elem.altRate(), -V, 1e-12);

    // 东、北、天三方向正交且完备
    EXPECT_NEAR(dot3(east, north), 0.0, 1e-15);
    EXPECT_NEAR(dot3(east, up), 0.0, 1e-15);
    EXPECT_NEAR(dot3(north, up), 0.0, 1e-15);

    // 经度率与经度无关(绕自转轴的旋转对称性)
    const Vector3d pos2 = lla2fixed(*shape, lat, lon + 137.0_deg, alt);
    const GeodeticPoint detic2(lat, lon + 137.0_deg, alt);
    ASSERT_EQ(aCartToGeodetic(pos2, detic2.getEast() * V, *shape, elem), eNoError);
    EXPECT_NEAR(elem.lonRate(), V / rho, 1e-15);
}

/// @brief 往返一致性
TEST(Geodetic, RoundTrip)
{
    auto shape = aWGS84Spheroid();
    ASSERT_TRUE(shape != nullptr);

    struct Case
    {
        double lat, lon, alt, lonRate, latRate, altRate;
    };
    const Case cases[] = {
        {0.0,          0.0,          0.0,        0.0,        0.0,       0.0},
        {30.0_deg,     120.0_deg,    400.0_km,   1.0e-3,     2.0e-3,    10.0},
        {45.0_deg,     -170.0_deg,   250.0_km,  -3.0e-3,     1.0e-3,   -25.0},
        {-60.0_deg,    30.0_deg,     1000.0_km,  2.5e-3,    -2.0e-3,    50.0},
        {85.0_deg,     179.0_deg,    0.0,        1.0e-4,    1.0e-4,     1.0},
        {28.5_deg,    -80.6_deg,     200.0_km,  -1.5e-3,     5.0e-4,   -12.5},
    };

    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)
    {
        const Case& c = cases[i];
        const GeodeticElem elem = makeElem(c.lat, c.lon, c.alt, c.lonRate, c.latRate, c.altRate);

        Vector3d pos{}, vel{};
        errc_t rc = aGeodeticToCart(elem, *shape, pos, vel);
        ASSERT_EQ(rc, eNoError) << i;

        GeodeticElem back{};
        rc = aCartToGeodetic(pos, vel, *shape, back);
        ASSERT_EQ(rc, eNoError) << i;

        EXPECT_NEAR(back.lon(), c.lon, 1e-12) << i;
        EXPECT_NEAR(back.lat(), c.lat, 1e-12) << i;
        EXPECT_NEAR(back.alt(), c.alt, 1e-6) << i;
        EXPECT_NEAR(back.lonRate(), c.lonRate, 1e-12) << i;
        EXPECT_NEAR(back.latRate(), c.latRate, 1e-12) << i;
        EXPECT_NEAR(back.altRate(), c.altRate, 1e-9) << i;

        // 再正向一次, 位置与速度应与首次一致
        Vector3d pos2{}, vel2{};
        rc = aGeodeticToCart(back, *shape, pos2, vel2);
        ASSERT_EQ(rc, eNoError) << i;
        EXPECT_NEAR(relDiff(pos2, pos), 0.0, 1e-12) << i;
        // 零速度是合法工况, 故用分母加1的相对误差, 避免 0/0
        EXPECT_NEAR(relDiffSafe(vel2, vel), 0.0, 1e-12) << i;
    }
}

/// @brief 圆球是零扁率椭球: 退化为地心球坐标, 且与球坐标根数一致
TEST(Geodetic, SphereReduction)
{
    const double R = 6800.0_km;
    SharedPtr<BodyShape> sphere = new SphereShape(R);
    ASSERT_TRUE(sphere != nullptr);

    const Vector3d pos{-2.0e6, 5.5e6, 3.1e6};
    const Vector3d vel{-1.2e3, 6.0e3, -4.4e3};

    GeodeticElem elem{};
    ASSERT_EQ(aCartToGeodetic(pos, vel, *sphere, elem), eNoError);

    SphericalElem sph{};
    ASSERT_EQ(aCartToSpherical(pos, vel, sph), eNoError);

    // 零扁率下大地纬度退化为地心纬度、经度退化为赤经, 高度退化为地心距减半径
    EXPECT_NEAR(elem.lon(), sph.ra(), 1e-15);
    EXPECT_NEAR(elem.lat(), sph.dec(), 1e-15);
    EXPECT_NEAR(elem.alt(), sph.r() - R, 1e-6);

    // 速度分解与球坐标根数的航迹角、航迹方位角一致
    const double rho = R + elem.alt();
    const double vE = rho * cos(elem.lat()) * elem.lonRate();
    const double vN = rho * elem.latRate();
    const double vU = elem.altRate();
    EXPECT_NEAR(sqrt(vE * vE + vN * vN + vU * vU), sph.v(), 1e-9);
    EXPECT_NEAR(atan2(vU, hypot(vE, vN)), sph.fpa(), 1e-12);
    EXPECT_NEAR(atan2(vE, vN), sph.azi(), 1e-12);

    // 圆球路径与"扁率为0的扁球体"一致
    SharedPtr<BodyShape> spheroid = SpheroidShape::NewFromMajorAxisFlatFactor(R, 0.0);
    ASSERT_TRUE(spheroid != nullptr);
    GeodeticElem elem2{};
    ASSERT_EQ(aCartToGeodetic(pos, vel, *spheroid, elem2), eNoError);
    EXPECT_NEAR(elem2.lon(), elem.lon(), 1e-15);
    EXPECT_NEAR(elem2.lat(), elem.lat(), 1e-15);
    EXPECT_NEAR(elem2.alt(), elem.alt(), 1e-6);
    EXPECT_NEAR(elem2.lonRate(), elem.lonRate(), 1e-15);
    EXPECT_NEAR(elem2.latRate(), elem.latRate(), 1e-15);
    EXPECT_NEAR(elem2.altRate(), elem.altRate(), 1e-9);
}

/// @brief 接近极点(但不取极点)处的往返
/// @details lat=±90° 处经度率奇异, 且 aBodyFixedToGeodetic 的高度反解在极点失效,
///          故本用例只取 89.9°, 极点工况不在本类型的能力范围内。
TEST(Geodetic, NearPole)
{
    auto shape = aWGS84Spheroid();
    ASSERT_TRUE(shape != nullptr);

    const double lat = 89.9_deg;
    const double lon = 120.0_deg;
    const double alt = 100.0_km;
    const GeodeticElem elem = makeElem(lat, lon, alt, 1.0e-4, 1.0e-4, 1.0);

    Vector3d pos{}, vel{};
    ASSERT_EQ(aGeodeticToCart(elem, *shape, pos, vel), eNoError);

    GeodeticElem back{};
    ASSERT_EQ(aCartToGeodetic(pos, vel, *shape, back), eNoError);
    EXPECT_NEAR(back.lat(), lat, 1e-12);
    EXPECT_NEAR(back.alt(), alt, 1e-5);
    EXPECT_NEAR(back.lonRate(), 1.0e-4, 1e-12);
    EXPECT_NEAR(back.latRate(), 1.0e-4, 1e-12);
    EXPECT_NEAR(back.altRate(), 1.0, 1e-9);
}

/// @brief 非法输入与退化工况
TEST(Geodetic, InvalidInput)
{
    auto shape = aWGS84Spheroid();
    ASSERT_TRUE(shape != nullptr);

    const Vector3d pos{kKm * 7000.0, 0.0, 0.0};
    const Vector3d zero{0.0, 0.0, 0.0};
    GeodeticElem elem{};

    // 零位置: 无法确定经度与纬度
    EXPECT_EQ(aCartToGeodetic(zero, pos, *shape, elem), eErrorInvalidParam);

    // 零速度是合法的(同步轨道等): 三个变化率均为零
    // 注意与 aCartToSpherical/aCartToMixedSpherical 不同, 那里速度为零会报错
    ASSERT_EQ(aCartToGeodetic(pos, zero, *shape, elem), eNoError);
    EXPECT_NEAR(elem.lat(), 0.0, 1e-15);
    EXPECT_NEAR(elem.lon(), 0.0, 1e-15);
    EXPECT_EQ(elem.lonRate(), 0.0);
    EXPECT_EQ(elem.latRate(), 0.0);
    EXPECT_EQ(elem.altRate(), 0.0);

    // 大地纬度超出范围
    Vector3d p{}, v{};
    const GeodeticElem badLat = makeElem(100.0_deg, 0.0, 0.0, 0.0, 0.0, 0.0);
    EXPECT_EQ(aGeodeticToCart(badLat, *shape, p, v), eErrorInvalidParam);
    EXPECT_NEAR(norm3(p), 0.0, 1e-12);
    EXPECT_NEAR(norm3(v), 0.0, 1e-12);
}

/// @brief 三轴椭球不支持该转换, 应明确报错而不是给出错误结果
TEST(Geodetic, UnsupportedShape)
{
    SharedPtr<BodyShape> ellipsoid = new EllipsoidShape(6378137.0, 6378137.0, 6356752.3);
    ASSERT_TRUE(ellipsoid != nullptr);

    const Vector3d pos{6378137.0, 0.0, 0.0};
    const Vector3d vel{0.0, 7000.0, 0.0};

    GeodeticElem elem{};
    EXPECT_EQ(aCartToGeodetic(pos, vel, *ellipsoid, elem), eErrorNotImplemented);

    const GeodeticElem good = makeElem(0.0, 0.0, 0.0, 1e-3, 1e-3, 1.0);
    Vector3d p{}, v{};
    EXPECT_EQ(aGeodeticToCart(good, *ellipsoid, p, v), eErrorNotImplemented);
    EXPECT_NEAR(norm3(p), 0.0, 1e-12);
    EXPECT_NEAR(norm3(v), 0.0, 1e-12);
}

/// @brief 字符串输出
TEST(Geodetic, ToString)
{
    const GeodeticElem elem = makeElem(20.0_deg, 30.0_deg, 500.0_km, 1.0e-3, -2.0e-3, 15.0);
    const std::string str = elem.toString();
    printf("GeodeticElem: %s\n", str.c_str());
    EXPECT_TRUE(str.find("GeodeticElem") != std::string::npos);
}


// ---------------------------------------------------------------------------
// 状态类: 与直角坐标/修正轨道根数互换、工厂创建、RTTI属性
// ---------------------------------------------------------------------------

class GeodeticStateTest : public ::testing::Test
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

    /// @brief 构造地球ICRF轴系下的大地坐标状态
    HStateGeodetic makeEarthState(const TimePoint& epoch)
    {
        auto frame = aGetEarth()->makeFrame(aAxesICRF());
        frame->setName("ICRF");
        auto state = StateGeodetic::MakeShared();
        state->setFrame(frame.get());
        state->setStateEpoch(epoch);
        return state;
    }
};

TEST_F(GeodeticStateTest, StateIntegration)
{
    auto earth = aGetEarth();
    ASSERT_TRUE(earth != nullptr);
    auto shape = earth->getShape();
    ASSERT_TRUE(shape != nullptr);
    ASSERT_GT(shape->majorAxis(), 0.0);

    const TimePoint epoch = TimePoint::FromUTC(2026, 9, 14, 4, 0, 0);
    auto frame = earth->makeFrame(aAxesICRF());
    frame->setName("ICRF");

    const double lat = 28.5_deg;
    const double lon = -80.6_deg;
    const double alt = 300.0_km;

    // 固连系下的位置与速度
    const Vector3d posFixed = lla2fixed(*shape, lat, lon, alt);
    const Vector3d velFixed = Vector3d{-sin(lon), cos(lon), 0.0} * 7000.0;

    // 用运动学变换换算到惯性系(含固连系的牵连速度和角速度)
    KinematicTransform fixedToInertial;
    ASSERT_EQ(aFrameTransform(earth->getFrameFixed(), frame.get(), epoch, fixedToInertial), eNoError);
    CartState cart{};
    fixedToInertial.transformPositionVelocity(posFixed, velFixed, cart.pos(), cart.vel());
    EXPECT_GT(norm3(cart.vel()), 7000.0);

    auto state = makeEarthState(epoch);
    ASSERT_EQ(state->setState(cart), eNoError);

    // 状态分量应还原为固连系下的大地坐标及其变化率
    EXPECT_NEAR(state->getLat(), lat, 1e-9);
    EXPECT_NEAR(state->getLon(), lon, 1e-9);
    EXPECT_NEAR(state->getAlt(), alt, 1e-4);

    GeodeticElem direct{};
    ASSERT_EQ(aCartToGeodetic(posFixed, velFixed, *shape, direct), eNoError);
    EXPECT_NEAR(state->getLonRate(), direct.lonRate(), 1e-12);
    EXPECT_NEAR(state->getLatRate(), direct.latRate(), 1e-12);
    EXPECT_NEAR(state->getAltRate(), direct.altRate(), 1e-9);

    // 回代直角坐标
    CartState cartBack{};
    ASSERT_EQ(state->getState(cartBack), eNoError);
    EXPECT_NEAR(relDiff(cartBack.pos(), cart.pos()), 0.0, 1e-9);
    EXPECT_NEAR(relDiff(cartBack.vel(), cart.vel()), 0.0, 1e-9);

    // 与大地坐标接口一致
    GeodeticPoint geodetic;
    ASSERT_EQ(state->getStateIn(earth, geodetic), eNoError);
    EXPECT_NEAR(geodetic.latitude(), state->getLat(), 1e-9);
    EXPECT_NEAR(geodetic.longitude(), state->getLon(), 1e-9);
    EXPECT_NEAR(geodetic.altitude(), state->getAlt(), 1e-4);

    // 与修正轨道根数互换
    ModOrbElem modOrb{};
    ASSERT_EQ(state->getState(modOrb), eNoError);
    const ModOrbElem modOrbDirect = aCartToModOrbElem(cart.pos(), cart.vel(), state->getGM());
    EXPECT_NEAR(modOrb.getSMA(), modOrbDirect.getSMA(), 1e-3);
    EXPECT_NEAR(modOrb.getEcc(), modOrbDirect.getEcc(), 1e-12);
    EXPECT_NEAR(modOrb.getInc(), modOrbDirect.getInc(), 1e-12);

    ASSERT_EQ(state->setState(modOrb), eNoError);
    CartState cartFromModOrb{};
    ASSERT_EQ(state->getState(cartFromModOrb), eNoError);
    EXPECT_NEAR(relDiff(cartFromModOrb.pos(), cart.pos()), 0.0, 1e-8);
    EXPECT_NEAR(relDiff(cartFromModOrb.vel(), cart.vel()), 0.0, 1e-8);

    // 参考坐标系取固连系本身时, 变换退化为恒等
    auto fixedState = makeEarthState(epoch);
    fixedState->setFrame(earth->getFrameFixed());
    ASSERT_EQ(fixedState->setState(CartState{posFixed, velFixed}), eNoError);
    EXPECT_NEAR(fixedState->getLat(), lat, 1e-9);
    EXPECT_NEAR(fixedState->getLon(), lon, 1e-9);
    CartState fixedBack{};
    ASSERT_EQ(fixedState->getState(fixedBack), eNoError);
    EXPECT_NEAR(relDiff(fixedBack.pos(), posFixed), 0.0, 1e-12);
    EXPECT_NEAR(relDiff(fixedBack.vel(), velFixed), 0.0, 1e-12);
}

/// @brief 速度是固连系速度而非惯性速度
/// @details 同步轨道在固连系下静止, 三个变化率应全为零, 而惯性速度约为 3 km/s;
///          反之, 变化率为零的状态还原出的惯性速度应为固连系的牵连速度。
///          若把变化率误当作惯性速度的分量, 这两条都会失败。
TEST_F(GeodeticStateTest, GeostationaryIsNotInertial)
{
    auto earth = aGetEarth();
    ASSERT_TRUE(earth != nullptr);
    auto shape = earth->getShape();
    ASSERT_TRUE(shape != nullptr);

    const TimePoint epoch = TimePoint::FromUTC(2026, 9, 14, 4, 0, 0);
    auto frame = earth->makeFrame(aAxesICRF());
    frame->setName("ICRF");

    const double lon = 110.0_deg;
    const double alt = 35786000.0;
    const double rho = shape->majorAxis() + alt;
    const double spinSpeed = kEarthAngVel * rho;      // 约 3074.7 m/s
    EXPECT_GT(spinSpeed, 3000.0);

    KinematicTransform fixedToInertial;
    ASSERT_EQ(aFrameTransform(earth->getFrameFixed(), frame.get(), epoch, fixedToInertial), eNoError);

    // --- 固连系下静止的点: 惯性速度为牵连速度, 变化率为零 ---
    const Vector3d posFixed = lla2fixed(*shape, 0.0, lon, alt);
    CartState cart{};
    fixedToInertial.transformPositionVelocity(posFixed, Vector3d::Zero(), cart.pos(), cart.vel());
    EXPECT_NEAR(norm3(cart.vel()), spinSpeed, 1e-4 * spinSpeed);

    auto state = makeEarthState(epoch);
    ASSERT_EQ(state->setState(cart), eNoError);
    EXPECT_NEAR(state->getLonRate(), 0.0, 1e-15);
    EXPECT_NEAR(state->getLatRate(), 0.0, 1e-15);
    EXPECT_NEAR(state->getAltRate(), 0.0, 1e-6);
    EXPECT_NEAR(state->getLat(), 0.0, 1e-9);
    EXPECT_NEAR(state->getLon(), lon, 1e-9);
    EXPECT_NEAR(state->getAlt(), alt, 1e-3);

    // --- 变化率为零的状态: 惯性速度必须是牵连速度而不是零 ---
    auto still = makeEarthState(epoch);
    still->setLon(lon);
    still->setLat(0.0);
    still->setAlt(alt);
    still->setLonRate(0.0);
    still->setLatRate(0.0);
    still->setAltRate(0.0);

    CartState cartStill{};
    ASSERT_EQ(still->getState(cartStill), eNoError);
    EXPECT_NEAR(norm3(cartStill.vel()), spinSpeed, 1e-4 * spinSpeed);
    EXPECT_NEAR(relDiff(cartStill.pos(), cart.pos()), 0.0, 1e-9);

    // 再回代, 三个变化率仍为零(自洽)
    auto still2 = makeEarthState(epoch);
    ASSERT_EQ(still2->setState(cartStill), eNoError);
    EXPECT_NEAR(still2->getLonRate(), 0.0, 1e-15);
    EXPECT_NEAR(still2->getLatRate(), 0.0, 1e-15);
    EXPECT_NEAR(still2->getAltRate(), 0.0, 1e-6);

    // --- 经度率取地球自转角速度: 固连系速度叠加牵连速度, 惯性速度约为两倍 ---
    auto doubled = makeEarthState(epoch);
    doubled->setLon(lon);
    doubled->setLat(0.0);
    doubled->setAlt(alt);
    doubled->setLonRate(kEarthAngVel);
    doubled->setLatRate(0.0);
    doubled->setAltRate(0.0);

    CartState cartDoubled{};
    ASSERT_EQ(doubled->getState(cartDoubled), eNoError);
    EXPECT_NEAR(norm3(cartDoubled.vel()), 2.0 * spinSpeed, 1e-4 * spinSpeed);
}

TEST_F(GeodeticStateTest, Factory)
{
    const TimePoint epoch = TimePoint::FromUTC(2026, 9, 14, 4, 0, 0);
    auto state = makeEarthState(epoch);

    auto earth = aGetEarth();
    auto shape = earth->getShape();
    ASSERT_TRUE(shape != nullptr);

    const Vector3d posFixed = lla2fixed(*shape, 10.0_deg, 20.0_deg, 400.0_km);
    auto frame = earth->makeFrame(aAxesICRF());
    frame->setName("ICRF");
    KinematicTransform fixedToInertial;
    ASSERT_EQ(aFrameTransform(earth->getFrameFixed(), frame.get(), epoch, fixedToInertial), eNoError);

    CartState cart{};
    fixedToInertial.transformPositionVelocity(posFixed, Vector3d{0.0, 7000.0, 0.0},
                                              cart.pos(), cart.vel());
    ASSERT_EQ(state->setState(cart), eNoError);

    // 由状态类型枚举创建(工厂)
    PState state2 = State::New(EStateType::eGeodetic);
    ASSERT_TRUE(state2 != nullptr);
    EXPECT_EQ(state2->getStateType(), EStateType::eGeodetic);
    delete state2;

    // 由已有状态拷贝构造
    PState state3 = State::New(*state, EStateType::eGeodetic);
    ASSERT_TRUE(state3 != nullptr);
    EXPECT_EQ(state3->getStateType(), EStateType::eGeodetic);
    CartState cart3{};
    ASSERT_EQ(state3->getState(cart3), eNoError);
    EXPECT_NEAR(relDiff(cart3.pos(), cart.pos()), 0.0, 1e-9);
    EXPECT_NEAR(relDiff(cart3.vel(), cart.vel()), 0.0, 1e-9);
    delete state3;

    // RTTI: 按类名创建并读写属性
    Object* obj = aNewObject(StringView("StateGeodetic"));
    ASSERT_TRUE(obj != nullptr);
    auto* geodeticObj = aobject_cast<StateGeodetic*>(obj);
    ASSERT_TRUE(geodeticObj != nullptr);
    geodeticObj->attr("Alt") = "500 km";
    EXPECT_NEAR(geodeticObj->getAlt(), 500e3, 1e-6);
    geodeticObj->setLonRate(1.0e-3);
    EXPECT_NEAR(geodeticObj->getAttrDouble("LonRate"), 1.0e-3, 1e-15);
    geodeticObj->attr("LatRate") = 2.0e-3;
    EXPECT_NEAR(geodeticObj->getLatRate(), 2.0e-3, 1e-15);
    geodeticObj->setAltRate(-30.0);
    EXPECT_NEAR(geodeticObj->getAltRate(), -30.0, 1e-12);
    delete geodeticObj;
}

/// @brief 缺少参考坐标系、参考椭球或状态历元时应报错而非崩溃
TEST_F(GeodeticStateTest, MissingContext)
{
    CartState cart{};
    cart.pos() = Vector3d{kKm * 7000.0, 0.0, 0.0};
    cart.vel() = Vector3d{0.0, kKm * 7.0, 0.0};

    // 未设置参考坐标系
    PState state = State::New(EStateType::eGeodetic);
    ASSERT_TRUE(state != nullptr);
    EXPECT_NE(state->setState(cart), eNoError);
    CartState out{};
    EXPECT_NE(state->getState(out), eNoError);
    delete state;

    // 有参考坐标系但没有状态历元
    const TimePoint epoch = TimePoint::FromUTC(2026, 9, 14, 4, 0, 0);
    auto state2 = makeEarthState(epoch);
    ASSERT_EQ(state2->setState(cart), eNoError);
    state2->setStateEpoch(static_cast<EventTime*>(nullptr));
    CartState out2{};
    EXPECT_NE(state2->getState(out2), eNoError);
}


GTEST_MAIN()

///
/// @file      testAttitudeProfile.cpp
/// @brief     姿态剖面模块测试
/// @details   
/// @author    axel
/// @date      2026-09-28
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
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

#include "ast/OrbitElement.hpp"
#include "ast/Literals.hpp"
#include "ast/RunTime.hpp"
#include "ast/CelestialBody.hpp"
#include "ast/AttitudeProfile.hpp"
#include "ast/AttitudeAlignConstrain.hpp"
#include "ast/AttitudeECIVVLH.hpp"
#include "ast/AttitudeECFVVLH.hpp"
#include "ast/AttitudeRelSunLH.hpp"
#include "ast/AttitudeECFVelRadial.hpp"
#include "ast/AttitudeNadirNormal.hpp"
#include "ast/AttitudeAircraftZDown.hpp"
#include "ast/AttitudeMissile.hpp"
#include "ast/AttitudeCbiVelSun.hpp"
#include "ast/AttitudeSunPointing.hpp"
#include "ast/AttitudeSunPointingEclpNormal.hpp"
#include "ast/AttitudeSunPointingCbiZ.hpp"
#include "ast/AttitudeFixed.hpp"
#include "ast/AttitudeYPRFixedECI.hpp"
#include "ast/AttitudeSpinning.hpp"
#include "ast/AttitudeConvertProto.hpp"
#include "ast/AxesFrozen.hpp"
#include "ast/BuiltinAxes.hpp"
#include "ast/BuiltinFrame.hpp"
#include "ast/Constants.h"
#include "ast/EphemerisTwoBody.hpp"
#include "ast/KinematicRotation.hpp"
#include "ast/LocalOrbitFrame.hpp"
#include "ast/Mover.hpp"
#include "ast/Rotation.hpp"
#include "ast/TimePoint.hpp"
#include "ast/RunTime.hpp"
#include "ast/RTTIAPI.hpp"
#include "ast/AstTestMacro.h"
#include "ast/EOP.hpp"
#include <cmath>

AST_USING_NAMESPACE
using namespace _AST literals;


/// @brief 测试用点：直接返回给定的位置与速度，便于构造退化几何
class TestPoint : public Point
{
public:
    Frame* getFrame() const override { return frame_; }
    errc_t getPos(const TimePoint& tp, Vector3d& pos) const override
    {
        pos = pos_;
        return eNoError;
    }
    errc_t getPosVel(const TimePoint& tp, Vector3d& pos, Vector3d& vel) const override
    {
        pos = pos_;
        vel = vel_;
        return eNoError;
    }

    Frame* frame_{nullptr};
    Vector3d pos_{};
    Vector3d vel_{};
};

/// @brief 校验旋转矩阵正交且行列式为 +1
void ExpectOrthonormal(const Matrix3d& m, double eps = 1e-12)
{
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
        {
            double value = 0.0;
            for (int k = 0; k < 3; k++)
                value += m(k, i) * m(k, j);
            EXPECT_NEAR(value, (i == j) ? 1.0 : 0.0, eps);
        }
    const double det = m(0, 0) * (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1))
                     - m(0, 1) * (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0))
                     + m(0, 2) * (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0));
    EXPECT_NEAR(det, 1.0, eps);
}

/// @brief 校验两个旋转矩阵逐元素相等
void ExpectSameMatrix(const Matrix3d& actual, const Matrix3d& expected, double eps)
{
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            EXPECT_NEAR(actual(r, c), expected(r, c), eps);
}


class AttitudeProfileTest : public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        aInitialize();
        aDataContext_GetEOP()->unload();
    }

    /// @brief 测试历元
    /// @details 必须让"查询时刻 - 剖面历元"保持为一个不大的数：
    ///          若历元取默认值，角速度乘以相隔数十年的大时间会得到一个巨大的角度，
    ///          光是这个乘积自身的舍入误差就会淹没数值差分的精度。
    static TimePoint TestEpoch()
    {
        return TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    }

    static TimePoint TestTime()
    {
        return TimePoint::FromUTC(2026, 1, 1, 0, 0, 10);
    }

    /// @brief 构造一条二体星历作为姿态的载体
    static SharedPtr<EphemerisTwoBody> MakeOrbit(const Vector3d& pos, const Vector3d& vel)
    {
        CartState state;
        state.pos_ = pos;
        state.vel_ = vel;
        return SharedPtr<EphemerisTwoBody>(
            EphemerisTwoBody::New(aFrameECI(), kEarthGrav, TestEpoch(), state));
    }

    /// @brief 一般倾斜偏心轨道
    static SharedPtr<EphemerisTwoBody> MakeGeneralOrbit()
    {
        return MakeOrbit(Vector3d{7000e3, 1200e3, 800e3},
                         Vector3d{-1.2e3, 6.8e3, 2.4e3});
    }

    /// @brief 半径为 radius 的赤道圆轨道，在历元时刻位于 +X 轴上
    static SharedPtr<EphemerisTwoBody> MakeCircularEquatorialOrbit(double radius = 7000e3)
    {
        const double speed = std::sqrt(kEarthGrav / radius);
        return MakeOrbit(Vector3d{radius, 0.0, 0.0}, Vector3d{0.0, speed, 0.0});
    }
};


TEST_F(AttitudeProfileTest, AttitudeECIVVLH)
{
    {
        OrbElem orbElem{6678137, 0, 28.5_deg, 0, 0, 0};
        CartState state;
        auto earth = aGetEarth();
        double gm = earth->getGM();
        auto epoch = "28 Sep 2026 04:00:00.000"_utc;
        aOrbElemToCart(orbElem, gm, state.pos_, state.vel_);
        SharedPtr<EphemerisTwoBody> eph  = EphemerisTwoBody::New(earth->getFrameInertial(), gm, epoch, state);
        AttitudeECIVVLH attitude(eph, earth);

        auto startTime = "28 Sep 2026 04:00:00.000"_utc;
        auto stopTime  = "29 Sep 2026 04:00:00.000"_utc;
        {
            Quaternion q;
            attitude.getAttitudeIn(*earth->getAxesInertial(), startTime, q);
            printf("q: %s\n", q.toString().c_str());
            Quaternion expected{0.6076921013678738, -0.3615388083388807, -0.6076921013678738, 0.3615388083388807};
            for(int i = 0; i < 4; i++)
            {
                EXPECT_NEAR(q[i], expected[i], 1e-14);
            }
        }
        {
            Quaternion q;
            attitude.getAttitudeIn(*earth->getAxesInertial(), stopTime, q);
            printf("q: %s\n", q.toString().c_str());
            Quaternion expected{0.7554924957257300, -0.4494708027285990, -0.4096467612857596, 0.2437142124799141};
            for(int i = 0; i < 4; i++)
            {
                EXPECT_NEAR(q[i], expected[i], 1e-14);
            }
        }
    }
    {
        OrbElem orbElem{6678137, 0.02, 28.5_deg, 0, 0, 0};
        CartState state;
        auto earth = aGetEarth();
        double gm = earth->getGM();
        auto epoch = "28 Sep 2026 04:00:00.000"_utc;
        aOrbElemToCart(orbElem, gm, state.pos_, state.vel_);
        SharedPtr<EphemerisTwoBody> eph  = EphemerisTwoBody::New(earth->getFrameInertial(), gm, epoch, state);
        AttitudeECIVVLH attitude(eph, earth);

        {
            auto time = "28 Sep 2026 05:51:00.000"_utc;
            Quaternion q;
            Vector3d w;
            attitude.getAttitudeIn(*earth->getAxesInertial(), time, q, w);
            Rotation rot(q);
            w = rot.transformVector(w);
            printf("q: %s\n", q.toString().c_str());
            printf("w: %s\n", w.toString().c_str());

            Quaternion expected_q{-0.0470336889994441, 0.0279821044808127, 0.8581184138735795, -0.5105268080120973 };
            Vector3d expected_w{0.0000000000000000, -0.0011626341030019, 0.0000000000000000};
            for(int i = 0; i < 4; i++)
            {
                EXPECT_NEAR(q[i], expected_q[i], 1e-14);
            }
            for(int i = 0; i < 3; i++)
            {
                EXPECT_NEAR(w[i], expected_w[i], 1e-14);
            }
        }
        {
            auto time  = "29 Sep 2026 04:00:00.000"_utc;
            Quaternion q;
            Vector3d w;
            attitude.getAttitudeIn(*earth->getAxesInertial(), time, q, w);
            Rotation rot(q);
            w = rot.transformVector(w);
            printf("q: %s\n", q.toString().c_str());
            printf("w: %s\n", w.toString().c_str());
            
            Quaternion expected_q{0.7600106309069503, -0.4521588106945666, -0.4012022196327228, 0.2386902381361061};
            Vector3d expected_w{0.0000000000000000, -0.0011961153080119, 0.0000000000000000};
            for(int i = 0; i < 4; i++)
            {
                EXPECT_NEAR(q[i], expected_q[i], 1e-13) << "q[" << i << "]";
            }
            for(int i = 0; i < 3; i++)
            {
                EXPECT_NEAR(w[i], expected_w[i], 1e-14) << "w[" << i << "]";
            }
        }
    }
}


TEST_F(AttitudeProfileTest, AttitudeECFVVLH)
{
    {
        OrbElem orbElem{6678137, 0.02, 28.5_deg, 0, 0, 0};
        CartState state;
        auto earth = aGetEarth();
        double gm = earth->getGM();
        auto epoch = "28 Sep 2026 04:00:00.000"_utc;
        aOrbElemToCart(orbElem, gm, state.pos_, state.vel_);
        SharedPtr<EphemerisTwoBody> eph  = EphemerisTwoBody::New(earth->getFrameInertial(), gm, epoch, state);
        AttitudeECFVVLH attitude(eph, earth);

        {
            auto time = "28 Sep 2026 11:44:00.000"_utc;
            Quaternion q;
            Vector3d w, w2;
            attitude.getAttitudeIn(*earth->getAxesInertial(), time, q, w);
            Rotation rot;
            attitude.getTransformFrom(*earth->getAxesInertial(), time, rot);
            w = rot.transformVector(w);
            aAxesRotationRateByDifference(attitude, *earth->getAxesInertial(), time, w2);
            w2 = rot.transformVector(w2);
            printf("q : %s\n", q.toString().c_str());
            printf("w : %s\n", w.toString().c_str());
            printf("w2: %s\n", w2.toString().c_str());
            Quaternion expected_q{-0.3199066399242105, 0.1789782544788932, 0.8015404017298567, -0.4723976191206197};
            Vector3d expected_w{0.0000249277151808, -0.0011890975123486, 0.0000262655902357};
            for(int i = 0; i < 4; i++)
            {
                EXPECT_NEAR(q[i], expected_q[i], 1e-13) << "q[" << i << "]";
            }
            for(int i = 0; i < 3; i++)
            {
                EXPECT_NEAR(w[i], expected_w[i], 1e-12) << "w[" << i << "]";
                EXPECT_NEAR(w2[i], expected_w[i], 1e-12) << "w2[" << i << "]";
            }
        }
    }
}


TEST_F(AttitudeProfileTest, AttitudeNadirNormal)
{
    {
        OrbElem orbElem{6678137, 0.02, 28.5_deg, 0, 0, 0};
        CartState state;
        auto earth = aGetEarth();
        double gm = earth->getGM();
        auto epoch = "3 Oct 2026 04:00:00.000"_utc;
        aOrbElemToCart(orbElem, gm, state.pos_, state.vel_);
        SharedPtr<EphemerisTwoBody> eph  = EphemerisTwoBody::New(earth->getFrameInertial(), gm, epoch, state);
        AttitudeNadirNormal attitude(eph, earth->getFrameInertial());

        {
            auto time = "3 Oct 2026 18:05:00.000"_utc;
            Quaternion q1, q2;
            Vector3d w;
            attitude.getAttitudeIn(*earth->getAxesInertial(), time, q1, w);
            attitude.getAttitudeIn(*earth->getAxesInertial(), time, q2);
            Rotation rot;
            attitude.getTransformFrom(*earth->getAxesInertial(), time, rot);
            w = rot.transformVector(w);

            printf("q1: %s\n", q1.toString().c_str());
            printf("q2: %s\n", q2.toString().c_str());
            printf("w: %s\n", w.toString().c_str());
            
            Quaternion expected_q{ 0.1767765511429616, 0.6846532341017603, -0.4820425466535251, 0.5173344983816410};
            Vector3d expected_w{0.0011327929510306, -0.0000000000000000, 0.0000000000000000};
            for(int i = 0; i < 4; i++)
            {
                EXPECT_NEAR(q1[i], expected_q[i], 1e-13) << "q1[" << i << "]";
                EXPECT_NEAR(q2[i], expected_q[i], 1e-13) << "q2[" << i << "]";
            }
            for(int i = 0; i < 3; i++)
            {
                EXPECT_NEAR(w[i], expected_w[i], 1e-12) << "w[" << i << "]";
            }
        }
    }
}


TEST_F(AttitudeProfileTest, AttitudeRelSunLH)
{
    {
        OrbElem orbElem{6678137, 0.02, 28.5_deg, 0, 0, 0};
        CartState state;
        auto earth = aGetEarth();
        double gm = earth->getGM();
        auto epoch = "3 Oct 2026 04:00:00.000"_utc;
        aOrbElemToCart(orbElem, gm, state.pos_, state.vel_);
        SharedPtr<EphemerisTwoBody> eph  = EphemerisTwoBody::New(earth->getFrameInertial(), gm, epoch, state);
        AttitudeRelSunLH attitude(eph, earth->getFrameInertial());

        {
            auto time = "3 Oct 2026 18:05:00.000"_utc;
            Quaternion q1, q2;
            Vector3d w;
            attitude.getAttitudeIn(*earth->getAxesInertial(), time, q1, w);
            attitude.getAttitudeIn(*earth->getAxesInertial(), time, q2);
            Rotation rot;
            attitude.getTransformFrom(*earth->getAxesInertial(), time, rot);
            w = rot.transformVector(w);
         

            printf("q1: %s\n", q1.toString().c_str());
            printf("q2: %s\n", q2.toString().c_str());
            printf("w: %s\n", w.toString().c_str());
            
            Quaternion expected_q{ 0.2366623265234935, -0.1502209089802685, 0.8237407032891829, -0.4928243860182566};
            Vector3d expected_w{0.0000191093202203, -0.0011326317600109, -0.0000078975880060};
            for(int i = 0; i < 4; i++)
            {
                EXPECT_NEAR(q1[i], expected_q[i], 1e-5) << "q1[" << i << "]";
                EXPECT_NEAR(q2[i], expected_q[i], 1e-5) << "q2[" << i << "]";
            }
            for(int i = 0; i < 3; i++)
            {
                EXPECT_NEAR(w[i], expected_w[i], 1e-8) << "w[" << i << "]";
            }
        }
    }
}


TEST_F(AttitudeProfileTest, AttitudeECFVelRadial)
{
    {
        OrbElem orbElem{6678137, 0.02, 28.5_deg, 0, 0, 0};
        CartState state;
        auto earth = aGetEarth();
        double gm = earth->getGM();
        auto epoch = "3 Oct 2026 04:00:00.000"_utc;
        aOrbElemToCart(orbElem, gm, state.pos_, state.vel_);
        SharedPtr<EphemerisTwoBody> eph  = EphemerisTwoBody::New(earth->getFrameInertial(), gm, epoch, state);
        AttitudeECFVelRadial attitude(eph, earth);

        {
            auto time = "3 Oct 2026 18:22:00.000"_utc;
            Quaternion q1, q2;
            Vector3d w;
            attitude.getAttitudeIn(*earth->getAxesInertial(), time, q1, w);
            attitude.getAttitudeIn(*earth->getAxesInertial(), time, q2);
            Rotation rot;
            attitude.getTransformFrom(*earth->getAxesInertial(), time, rot);
            w = rot.transformVector(w);

            printf("q1: %s\n", q1.toString().c_str());
            printf("q2: %s\n", q2.toString().c_str());
            printf("w: %s\n", w.toString().c_str());
            
            Quaternion expected_q{0.3767089342785476, 0.6543249192478061, -0.3245679589716010, -0.5697410981271073};
            Vector3d expected_w{-0.0000365437311113, 0.0011353871757996, 0.0000053706063609};
            for(int i = 0; i < 4; i++)
            {
                EXPECT_NEAR(q1[i], expected_q[i], 1e-12) << "q1[" << i << "]";
                EXPECT_NEAR(q2[i], expected_q[i], 1e-12) << "q2[" << i << "]";
            }
            for(int i = 0; i < 3; i++)
            {
                EXPECT_NEAR(w[i], expected_w[i], 1e-12) << "w[" << i << "]";
            }
        }
    }
}



TEST_F(AttitudeProfileTest, AttitudeAircraftZDown)
{
    {
        OrbElem orbElem{6678137, 0.02, 28.5_deg, 0, 0, 0};
        CartState state;
        auto earth = aGetEarth();
        double gm = earth->getGM();
        auto epoch = "3 Oct 2026 04:00:00.000"_utc;
        aOrbElemToCart(orbElem, gm, state.pos_, state.vel_);
        SharedPtr<EphemerisTwoBody> eph  = EphemerisTwoBody::New(earth->getFrameInertial(), gm, epoch, state);
        AttitudeAircraftZDown attitude(eph, earth);

        {
            auto time = "3 Oct 2026 20:27:00.000"_utc;
            Quaternion q1, q2;
            Vector3d w;
            attitude.getAttitudeIn(*earth->getAxesInertial(), time, q1, w);
            attitude.getAttitudeIn(*earth->getAxesInertial(), time, q2);
            Rotation rot;
            attitude.getTransformFrom(*earth->getAxesInertial(), time, rot);
            w = rot.transformVector(w);

            printf("q1: %s\n", q1.toString().c_str());
            printf("q2: %s\n", q2.toString().c_str());
            printf("w: %s\n", w.toString().c_str());
            
            Quaternion expected_q{0.7661228590691765, -0.4492756772317785, -0.4005507855093455, 0.2253135568212521};
            Vector3d expected_w{0.0000296023069400, -0.0011746630644052, -0.0000210656400908};
            for(int i = 0; i < 4; i++)
            {
                EXPECT_NEAR(q1[i], expected_q[i], 1e-12) << "q1[" << i << "]";
                EXPECT_NEAR(q2[i], expected_q[i], 1e-12) << "q2[" << i << "]";
            }
            for(int i = 0; i < 3; i++)
            {
                EXPECT_NEAR(w[i], expected_w[i], 1e-12) << "w[" << i << "]";
            }
        }
    }
}



TEST_F(AttitudeProfileTest, AttitudeMissile)
{
    {
        OrbElem orbElem{6678137, 0.02, 28.5_deg, 0, 0, 0};
        CartState state;
        auto earth = aGetEarth();
        double gm = earth->getGM();
        auto epoch = "3 Oct 2026 04:00:00.000"_utc;
        aOrbElemToCart(orbElem, gm, state.pos_, state.vel_);
        SharedPtr<EphemerisTwoBody> eph  = EphemerisTwoBody::New(earth->getFrameInertial(), gm, epoch, state);
        AttitudeMissile attitude(eph, earth);

        {
            auto time = "3 Oct 2026 20:32:00.000"_utc;
            Quaternion q1, q2;
            Vector3d w;
            attitude.getAttitudeIn(*earth->getAxesInertial(), time, q1, w);
            attitude.getAttitudeIn(*earth->getAxesInertial(), time, q2);
            Rotation rot;
            attitude.getTransformFrom(*earth->getAxesInertial(), time, rot);
            w = rot.transformVector(w);

            printf("q1: %s\n", q1.toString().c_str());
            printf("q2: %s\n", q2.toString().c_str());
            printf("w: %s\n", w.toString().c_str());

            Quaternion expected_q{0.6821727309392761, -0.4058501265195221, -0.5227042617892591, 0.3109763570985482};
            Vector3d expected_w{-0.0000000000000000, -0.0011798586987444, 0.0000000000000000};
            for(int i = 0; i < 4; i++)
            {
                EXPECT_NEAR(q1[i], expected_q[i], 1e-12) << "q1[" << i << "]";
                EXPECT_NEAR(q2[i], expected_q[i], 1e-12) << "q2[" << i << "]";
            }
            for(int i = 0; i < 3; i++)
            {
                EXPECT_NEAR(w[i], expected_w[i], 1e-12) << "w[" << i << "]";
            }
        }
    }
}



TEST_F(AttitudeProfileTest, AttitudeCbiVelSun)
{
    {
        OrbElem orbElem{6678137, 0.02, 28.5_deg, 0, 0, 0};
        CartState state;
        auto earth = aGetEarth();
        double gm = earth->getGM();
        auto epoch = "3 Oct 2026 04:00:00.000"_utc;
        aOrbElemToCart(orbElem, gm, state.pos_, state.vel_);
        SharedPtr<EphemerisTwoBody> eph  = EphemerisTwoBody::New(earth->getFrameInertial(), gm, epoch, state);
        AttitudeCbiVelSun attitude(eph, earth);

        {
            auto time = "3 Oct 2026 19:03:00.000"_utc;
            Quaternion q1, q2;
            Vector3d w;
            attitude.getAttitudeIn(*earth->getAxesInertial(), time, q1, w);
            attitude.getAttitudeIn(*earth->getAxesInertial(), time, q2);
            Rotation rot;
            attitude.getTransformFrom(*earth->getAxesInertial(), time, rot);
            w = rot.transformVector(w);

            printf("q1: %s\n", q1.toString().c_str());
            printf("q2: %s\n", q2.toString().c_str());
            printf("w: %s\n", w.toString().c_str());

            Quaternion expected_q{0.6562725082068822, -0.3831172601134498, -0.5558604536051824, 0.3369669362012423};
            Vector3d expected_w{-0.0000067446028583, -0.0011802552884750, 0.0000195534425498};
            for(int i = 0; i < 4; i++)
            {
                EXPECT_NEAR(q1[i], expected_q[i], 1e-5) << "q1[" << i << "]";
                EXPECT_NEAR(q2[i], expected_q[i], 1e-5) << "q2[" << i << "]";
            }
            for(int i = 0; i < 3; i++)
            {
                EXPECT_NEAR(w[i], expected_w[i], 1e-7) << "w[" << i << "]";
            }
        }
    }
    {
        auto orbit = MakeGeneralOrbit();
        AttitudeCbiVelSun profile;
        profile.setPoint(orbit.get());
        profile.setBody(aGetEarth());
        profile.setSun(aGetSun());

        TimePoint tp = TestTime();
        Vector3d pos, vel;
        ASSERT_EQ(orbit->getPosVelIn(aFrameECI(), tp, pos, vel), eNoError);
        Vector3d sunPos;
        ASSERT_EQ(aGetSun()->getPosIn(*aFrameECI(), tp, sunPos), eNoError);
        const Vector3d sunDir = sunPos - pos;

        Rotation rot;
        ASSERT_EQ(profile.getTransform(tp, rot), eNoError);
        ExpectOrthonormal(rot.getMatrix());

        // 体 X 严格对齐惯性系速度方向
        const Vector3d bodyX = rot.transformVector(vel.normalized());
        EXPECT_NEAR(bodyX[0], 1.0, 1e-12);
        EXPECT_NEAR(bodyX[1], 0.0, 1e-12);
        EXPECT_NEAR(bodyX[2], 0.0, 1e-12);

        // 体 Z 的太阳方向分量为正(约束到太阳，而不是背向太阳)
        EXPECT_GT(rot.transformVector(sunDir.normalized())[2], 0.0);

        // 两个重载给出同一旋转
        KinematicRotation kr;
        ASSERT_EQ(profile.getTransform(tp, kr), eNoError);
        ExpectSameMatrix(kr.getMatrix(), rot.getMatrix(), 1e-12);
    }
}



TEST_F(AttitudeProfileTest, SunPointing)
{
    {
        OrbElem orbElem{6678137, 0.02, 28.5_deg, 0, 0, 0};
        CartState state;
        auto earth = aGetEarth();
        double gm = earth->getGM();
        auto epoch = "3 Oct 2026 04:00:00.000"_utc;
        aOrbElemToCart(orbElem, gm, state.pos_, state.vel_);
        SharedPtr<EphemerisTwoBody> eph  = EphemerisTwoBody::New(earth->getFrameInertial(), gm, epoch, state);
        AttitudeSunPointing attitude(eph, earth);

        {
            auto time = "3 Oct 2026 19:07:00.000"_utc;
            Quaternion q1, q2;
            Vector3d w;
            attitude.getAttitudeIn(*earth->getAxesInertial(), time, q1, w);
            attitude.getAttitudeIn(*earth->getAxesInertial(), time, q2);
            Rotation rot;
            attitude.getTransformFrom(*earth->getAxesInertial(), time, rot);
            w = rot.transformVector(w);

            printf("q1: %s\n", q1.toString().c_str());
            printf("q2: %s\n", q2.toString().c_str());
            printf("w: %s\n", w.toString().c_str());

            Quaternion expected_q{-0.0398732141183855, -0.0795773656314848, 0.6396082886876180, 0.7635304884005085};
            Vector3d expected_w{0.0076166621831724, 0.0000002433829402, -0.0000000624623227};
            for(int i = 0; i < 4; i++)
            {
                EXPECT_NEAR(q1[i], expected_q[i], 1e-3) << "q1[" << i << "]";
                EXPECT_NEAR(q2[i], expected_q[i], 1e-3) << "q2[" << i << "]";
            }
            for(int i = 0; i < 3; i++)
            {
                EXPECT_NEAR(w[i], expected_w[i], 1e-4) << "w[" << i << "]";
            }
        }
    }
}

TEST_F(AttitudeProfileTest, SunPointingSemantics)
{
    auto orbit = MakeGeneralOrbit();
    AttitudeSunPointing profile;
    profile.setPoint(orbit.get());
    profile.setFrame(aFrameECI());
    profile.setSun(aGetSun());

    TimePoint tp = TestTime();
    Vector3d pos;
    ASSERT_EQ(orbit->getPosIn(aFrameECI(), tp, pos), eNoError);
    Vector3d sunPos;
    ASSERT_EQ(aGetSun()->getPosIn(*aFrameECI(), tp, sunPos), eNoError);
    const Vector3d sunDir = sunPos - pos;

    Rotation rot;
    ASSERT_EQ(profile.getTransform(tp, rot), eNoError);
    ExpectOrthonormal(rot.getMatrix());

    // 体 X 严格对齐太阳方向
    const Vector3d bodyX = rot.transformVector(sunDir.normalized());
    EXPECT_NEAR(bodyX[0], 1.0, 1e-12);
    EXPECT_NEAR(bodyX[1], 0.0, 1e-12);
    EXPECT_NEAR(bodyX[2], 0.0, 1e-12);

    // 体 Z 的对地方向分量为正(约束到原点，而不是背离原点)
    EXPECT_GT(rot.transformVector((-pos).normalized())[2], 0.0);

    // 两个重载给出同一旋转，且解析角速度与数值差分一致
    KinematicRotation kr;
    ASSERT_EQ(profile.getTransform(tp, kr), eNoError);
    ExpectSameMatrix(kr.getMatrix(), rot.getMatrix(), 1e-12);

    Vector3d wDiff;
    ASSERT_EQ(aAxesRotationRateByDifference(profile, *aFrameECI()->getAxes(), tp, wDiff), eNoError);
    const Vector3d wAnalytic = rot.transformVector(kr.getRotationRate());
    wDiff = rot.transformVector(wDiff);
    for (int i = 0; i < 3; i++)
        EXPECT_NEAR(wAnalytic[i], wDiff[i], 1e-9) << "w[" << i << "]";
}

TEST_F(AttitudeProfileTest, SunPointingEclpNormalSemantics)
{
    auto orbit = MakeGeneralOrbit();
    AttitudeSunPointingEclpNormal profile;
    profile.setPoint(orbit.get());
    profile.setFrame(aFrameECI());
    profile.setSun(aGetSun());

    TimePoint tp = TestTime();
    Vector3d pos;
    ASSERT_EQ(orbit->getPosIn(aFrameECI(), tp, pos), eNoError);
    Vector3d sunPos;
    ASSERT_EQ(aGetSun()->getPosIn(*aFrameECI(), tp, sunPos), eNoError);
    const Vector3d sunDir = sunPos - pos;

    // 黄道法向(黄道北极)在 J2000 惯性系下固定：ICRF 的 XY 平面绕 X 轴转过平黄赤交角
    const double obliquity = 84381.448 * kArcSecToRad;
    const Vector3d eclpNormal{0.0, -std::sin(obliquity), std::cos(obliquity)};

    Rotation rot;
    ASSERT_EQ(profile.getTransform(tp, rot), eNoError);
    ExpectOrthonormal(rot.getMatrix());

    // 体 X 严格对齐太阳方向
    const Vector3d bodyX = rot.transformVector(sunDir.normalized());
    EXPECT_NEAR(bodyX[0], 1.0, 1e-12);
    EXPECT_NEAR(bodyX[1], 0.0, 1e-12);
    EXPECT_NEAR(bodyX[2], 0.0, 1e-12);

    // 黄道法向落在体 XZ 平面内(体 Y 分量为零)，且体 Z 分量为正(约束到黄道法向，而不是背离)
    const Vector3d nInBody = rot.transformVector(eclpNormal);
    EXPECT_NEAR(nInBody[1], 0.0, 1e-12);
    EXPECT_GT(nInBody[2], 0.0);

    // 两个重载给出同一旋转，且解析角速度与数值差分一致
    KinematicRotation kr;
    ASSERT_EQ(profile.getTransform(tp, kr), eNoError);
    ExpectSameMatrix(kr.getMatrix(), rot.getMatrix(), 1e-12);

    Vector3d wDiff;
    ASSERT_EQ(aAxesRotationRateByDifference(profile, *aFrameECI()->getAxes(), tp, wDiff), eNoError);
    const Vector3d wAnalytic = rot.transformVector(kr.getRotationRate());
    wDiff = rot.transformVector(wDiff);
    for (int i = 0; i < 3; i++)
        EXPECT_NEAR(wAnalytic[i], wDiff[i], 1e-9) << "w[" << i << "]";
}

TEST_F(AttitudeProfileTest, AttitudeSunPointingEclpNormal)
{
    {
        auto orbit = MakeGeneralOrbit();
        AttitudeSunPointingEclpNormal attitude;
        attitude.setPoint(orbit.get());
        attitude.setFrame(aFrameECI());
        attitude.setSun(aGetSun());

        Quaternion q;
        Vector3d w;
        ASSERT_EQ(attitude.getAttitudeIn(*aFrameECI()->getAxes(), TestTime(), q, w), eNoError);

        const Quaternion expected{0.7511837261392353, 0.15580677025394402, 0.13027373347740306, -0.6280732555144087};
        for (int i = 0; i < 4; i++)
            EXPECT_NEAR(q[i], expected[i], 1e-4) << "q[" << i << "]";
    }
    {
        OrbElem orbElem{6678137, 0.02, 28.5_deg, 0, 0, 0};
        CartState state;
        auto earth = aGetEarth();
        double gm = earth->getGM();
        auto epoch = "3 Oct 2026 04:00:00.000"_utc;
        aOrbElemToCart(orbElem, gm, state.pos_, state.vel_);
        SharedPtr<EphemerisTwoBody> eph  = EphemerisTwoBody::New(earth->getFrameInertial(), gm, epoch, state);
        AttitudeSunPointingEclpNormal attitude(eph, earth);

        //     

        {
            auto time = "3 Oct 2026 17:06:00.000"_utc;
            Quaternion q1, q2;
            Vector3d w;
            attitude.getAttitudeIn(*earth->getAxesInertial(), time, q1, w);
            attitude.getAttitudeIn(*earth->getAxesInertial(), time, q2);
            Rotation rot;
            attitude.getTransformFrom(*earth->getAxesInertial(), time, rot);
            w = rot.transformVector(w);

            printf("q1: %s\n", q1.toString().c_str());
            printf("q2: %s\n", q2.toString().c_str());
            printf("w: %s\n", w.toString().c_str());

            Quaternion expected_q{ -0.0864338683742616, -0.0179206886292524, -0.2023133151411907, 0.9753344851042105};
            Vector3d expected_w{-0.0000000003120273, -0.0000000019138003, 0.0000001694258734};
            for(int i = 0; i < 4; i++)
            {
                EXPECT_NEAR(q1[i], expected_q[i], 1e-4) << "q1[" << i << "]";
                EXPECT_NEAR(q2[i], expected_q[i], 1e-4) << "q2[" << i << "]";
            }
            for(int i = 0; i < 3; i++)
            {
                EXPECT_NEAR(w[i], expected_w[i], 1e-9) << "w[" << i << "]";
            }
        }
    }
}

TEST_F(AttitudeProfileTest, SunPointingCbiZSemantics)
{
    auto orbit = MakeGeneralOrbit();
    auto earth = aGetEarth();
    AttitudeSunPointingCbiZ profile;
    profile.setPoint(orbit.get());
    profile.setBody(earth);     // 参考系被设为地球惯性系
    profile.setSun(aGetSun());

    TimePoint tp = TestTime();
    ASSERT_NE(profile.frame(), nullptr);
    ASSERT_EQ(profile.frame(), earth->getFrameInertial());

    Vector3d pos, sunPos;
    ASSERT_EQ(orbit->getPosIn(*profile.frame(), tp, pos), eNoError);
    ASSERT_EQ(aGetSun()->getPosIn(*profile.frame(), tp, sunPos), eNoError);
    const Vector3d sunDir = sunPos - pos;

    Rotation rot;
    ASSERT_EQ(profile.getTransform(tp, rot), eNoError);
    ExpectOrthonormal(rot.getMatrix());

    // 体 X 严格对齐太阳方向
    const Vector3d bodyX = rot.transformVector(sunDir.normalized());
    EXPECT_NEAR(bodyX[0], 1.0, 1e-12);
    EXPECT_NEAR(bodyX[1], 0.0, 1e-12);
    EXPECT_NEAR(bodyX[2], 0.0, 1e-12);

    // 两个重载给出同一旋转，且解析角速度与数值差分一致
    KinematicRotation kr;
    ASSERT_EQ(profile.getTransform(tp, kr), eNoError);
    ExpectSameMatrix(kr.getMatrix(), rot.getMatrix(), 1e-12);

    Vector3d wDiff;
    ASSERT_EQ(aAxesRotationRateByDifference(profile, *profile.frame()->getAxes(), tp, wDiff), eNoError);
    const Vector3d wAnalytic = rot.transformVector(kr.getRotationRate());
    wDiff = rot.transformVector(wDiff);
    for (int i = 0; i < 3; i++)
        EXPECT_NEAR(wAnalytic[i], wDiff[i], 1e-9) << "w[" << i << "]";
}

TEST_F(AttitudeProfileTest, AttitudeSunPointingCbiZ)
{
    {
        OrbElem orbElem{6678137, 0.02, 28.5_deg, 0, 0, 0};
        CartState state;
        auto earth = aGetEarth();
        double gm = earth->getGM();
        auto epoch = "3 Oct 2026 04:00:00.000"_utc;
        aOrbElemToCart(orbElem, gm, state.pos_, state.vel_);
        SharedPtr<EphemerisTwoBody> eph = EphemerisTwoBody::New(earth->getFrameInertial(), gm, epoch, state);
        AttitudeSunPointingCbiZ attitude(eph, earth);

        auto time = "3 Oct 2026 17:06:00.000"_utc;
        Quaternion q1, q2;
        Vector3d w;
        attitude.getAttitudeIn(*earth->getAxesInertial(), time, q1, w);
        attitude.getAttitudeIn(*earth->getAxesInertial(), time, q2);

        for (int i = 0; i < 4; i++)
            EXPECT_NEAR(q1[i], q2[i], 1e-12) << "q[" << i << "]";

        // 把参考系设为该天体的惯性系
        Vector3d pos, sunPos;
        ASSERT_EQ(eph->getPosIn(*earth->getFrameInertial(), time, pos), eNoError);
        ASSERT_EQ(aGetSun()->getPosIn(*earth->getFrameInertial(), time, sunPos), eNoError);
        const Vector3d sunDir = sunPos - pos;

        Rotation rot;
        ASSERT_EQ(attitude.getTransformFrom(*earth->getAxesInertial(), time, rot), eNoError);
        ExpectOrthonormal(rot.getMatrix());

        const Vector3d bodyX = rot.transformVector(sunDir.normalized());
        EXPECT_NEAR(bodyX[0], 1.0, 1e-12);
        EXPECT_NEAR(bodyX[1], 0.0, 1e-12);
        EXPECT_NEAR(bodyX[2], 0.0, 1e-12);
    }
    {
        OrbElem orbElem{6678137, 0.02, 28.5_deg, 0, 0, 0};
        CartState state;
        auto earth = aGetEarth();
        double gm = earth->getGM();
        auto epoch = "3 Oct 2026 04:00:00.000"_utc;
        aOrbElemToCart(orbElem, gm, state.pos_, state.vel_);
        SharedPtr<EphemerisTwoBody> eph  = EphemerisTwoBody::New(earth->getFrameInertial(), gm, epoch, state);
        AttitudeSunPointingCbiZ attitude(eph, earth);

        {
            auto time = "3 Oct 2026 17:20:00.000"_utc;
            Quaternion q1, q2;
            Vector3d w;
            attitude.getAttitudeIn(*earth->getAxesInertial(), time, q1, w);
            attitude.getAttitudeIn(*earth->getAxesInertial(), time, q2);
            Rotation rot;
            attitude.getTransformFrom(*earth->getAxesInertial(), time, rot);
            w = rot.transformVector(w);

            printf("q1: %s\n", q1.toString().c_str());
            printf("q2: %s\n", q2.toString().c_str());
            printf("w: %s\n", w.toString().c_str());

            Quaternion expected_q{-0.0811663356192398, -0.0349029184530175, -0.0028440759850293, 0.9960851989048921};
            Vector3d expected_w{-0.0000000139399289, 0.0000000873317882, 0.0000001986697525};
            for(int i = 0; i < 4; i++)
            {
                EXPECT_NEAR(q1[i], expected_q[i], 1e-4) << "q1[" << i << "]";
                EXPECT_NEAR(q2[i], expected_q[i], 1e-4) << "q2[" << i << "]";
            }
            for(int i = 0; i < 3; i++)
            {
                EXPECT_NEAR(w[i], expected_w[i], 1e-9) << "w[" << i << "]";
            }
        }
    }
}


TEST_F(AttitudeProfileTest, AttitudeSpinning)
{
    {
        OrbElem orbElem{6678137, 0.02, 28.5_deg, 0, 0, 0};
        CartState state;
        auto earth = aGetEarth();
        double gm = earth->getGM();
        auto epoch = "3 Oct 2026 04:00:00.000"_utc;
        aOrbElemToCart(orbElem, gm, state.pos_, state.vel_);
        SharedPtr<EphemerisTwoBody> eph  = EphemerisTwoBody::New(earth->getFrameInertial(), gm, epoch, state);
        AttitudeSpinning attitude;

        attitude.setReferenceAxes(earth->getAxesInertial());
        attitude.setSpinAxisInBody({1, 2, 3});
        attitude.setSpinAxisInFrame({3, 2, 1});
        attitude.setSpinRate(1.2000028061219963_revs/1_min);
        attitude.setSpinOffset(0.0);
        attitude.setEpoch("3 Oct 2026 04:00:00.000"_utc);

        {
            auto time = "3 Oct 2026 19:11:00.000"_utc;
            Quaternion q1, q2;
            Vector3d w;
            attitude.getAttitudeIn(*earth->getAxesInertial(), time, q1, w);
            attitude.getAttitudeIn(*earth->getAxesInertial(), time, q2);
            Rotation rot;
            attitude.getTransformFrom(*earth->getAxesInertial(), time, rot);
            w = rot.transformVector(w);

            printf("q1: %s\n", q1.toString().c_str());
            printf("q2: %s\n", q2.toString().c_str());
            printf("w: %s\n", w.toString().c_str());

            Quaternion expected_q{0.8215948822754436, 0.1291995822775872, 0.5399970234153091, 0.1291995822775874};
            Vector3d expected_w{0.0335851167036829, 0.0671702334073658, 0.1007553501110487};
            for(int i = 0; i < 4; i++)
            {
                EXPECT_NEAR(q1[i], expected_q[i], 1e-12) << "q1[" << i << "]";
                EXPECT_NEAR(q2[i], expected_q[i], 1e-12) << "q2[" << i << "]";
            }
            for(int i = 0; i < 3; i++)
            {
                EXPECT_NEAR(w[i], expected_w[i], 1e-12) << "w[" << i << "]";
            }
        }
    }
}

/// pinning 
TEST_F(AttitudeProfileTest, AttitudeSpinningBaseOrientation)
{
    auto earth = aGetEarth();
    AttitudeSpinning attitude;
    attitude.setReferenceAxes(earth->getAxesInertial());
    attitude.setSpinRate(0.0);
    attitude.setSpinOffset(0.0);
    attitude.setEpoch("3 Oct 2026 04:00:00.000"_utc);
    const auto time = "3 Oct 2026 04:00:00.000"_utc;

    struct Case
    {
        Vector3d inFrame;
        Vector3d inBody;
        Quaternion expected;
    };
    const Case cases[] = {
        {{1, 0, 0}, {0, 1, 0}, {0.5, 0.5, 0.5, -0.5}},
        {{0, 1, 0}, {1, 0, 0}, {0.5, -0.5, -0.5, 0.5}},
        {{1, 0, 0}, {1, 1, 1}, {0.8204732385702833, 0.3398511429799874, 0.4247082002778670, -0.1759198966061612}},
        {{1, 1, 0}, {0, 1, 1}, {0.8535533905932738, -0.3535533905932737, 0.1464466094067262, -0.3535533905932737}},
        {{3, 2, -1}, {1, -2, 3}, {0.2428301997936722, -0.9143927645907802, 0.2428301997936721, -0.2143661825017182}},
        {{0, 0, 1}, {0.8660254037844387, 0.49999999999999994, 0.0}, {0.6123724356957945, 0.6123724356957946, -0.3535533905932738, 0.3535533905932738}},
    };

    for (int k = 0; k < static_cast<int>(sizeof(cases) / sizeof(cases[0])); k++)
    {
        const Case& c = cases[k];
        attitude.setSpinAxisInFrame(c.inFrame);
        attitude.setSpinAxisInBody(c.inBody);
        Quaternion q;
        ASSERT_EQ(attitude.getAttitudeIn(*earth->getAxesInertial(), time, q), eNoError);

        double dot = 0.0;
        for (int i = 0; i < 4; i++)
            dot += q[i] * c.expected[i];
        if (dot < 0.0)
        {
            q.w() = -q.w();
            q.x() = -q.x();
            q.y() = -q.y();
            q.z() = -q.z();
        }

        for (int i = 0; i < 4; i++)
            EXPECT_NEAR(q[i], c.expected[i], 1e-12) << "case " << k << " [" << i << "]";
    }
}



// ============================================
// T4: 引擎与仓库里既有的坐标系函数一致(offset = 0)
//     aFrameToVVLHMatrix / aFrameToVNCMatrix 是仅有的两个现成参照
// ============================================
TEST_F(AttitudeProfileTest, MatchesExistingLocalFrameFunctions)
{
    auto orbit = MakeGeneralOrbit();
    AttitudeECIVVLH      ecivvlh;
    AttitudeECFVelRadial ecfRadial;
    ecivvlh.setPoint(orbit.get());
    ecivvlh.setBody(aGetEarth());
    ecfRadial.setPoint(orbit.get());
    ecfRadial.setBody(aGetEarth());

    for (int i = 0; i < 6; i++)
    {
        TimePoint tp = TestTime() + i * 300.0;

        Vector3d posICRF, velICRF, posECF, velECF;
        ASSERT_EQ(orbit->getPosVelIn(aFrameECI(), tp, posICRF, velICRF), eNoError);
        ASSERT_EQ(orbit->getPosVelIn(aFrameECF(), tp, posECF, velECF), eNoError);

        // 惯性系速度 + 对地约束 == VVLH
        Matrix3d expected;
        ASSERT_EQ(aFrameToVVLHMatrix(posICRF, velICRF, expected), eNoError);
        Rotation rot;
        ASSERT_EQ(ecivvlh.getTransform(tp, rot), eNoError);
        ExpectSameMatrix(rot.getMatrix(), expected, 1e-12);

        // 固连系速度 + 径向约束 == VNC
        ASSERT_EQ(aFrameToVNCMatrix(posECF, velECF, expected), eNoError);
        ASSERT_EQ(ecfRadial.getTransform(tp, rot), eNoError);
        ExpectSameMatrix(rot.getMatrix(), expected, 1e-12);
    }
}

// ============================================
// T7: ECF 速度对齐 + 径向约束
// ============================================
TEST_F(AttitudeProfileTest, ECFVelRadialSemantics)
{
    auto orbit = MakeGeneralOrbit();
    AttitudeECFVelRadial profile;
    profile.setPoint(orbit.get());
    profile.setBody(aGetEarth());

    TimePoint tp = TestTime();
    Vector3d pos, vel;
    ASSERT_EQ(orbit->getPosVelIn(aFrameECF(), tp, pos, vel), eNoError);

    Rotation rot;
    ASSERT_EQ(profile.getTransform(tp, rot), eNoError);
    ExpectOrthonormal(rot.getMatrix());

    // 体 X 严格沿固连系速度
    const Vector3d bodyX = rot.transformVector(vel.normalized());
    EXPECT_NEAR(bodyX[0], 1.0, 1e-12);
    EXPECT_NEAR(bodyX[1], 0.0, 1e-12);
    EXPECT_NEAR(bodyX[2], 0.0, 1e-12);

    // 体 Z 的径向分量为正(约束在径向而不是对地)
    EXPECT_GT(rot.transformVector(pos.normalized())[2], 0.0);
}

// ============================================
// T8: 对地指向 + 轨道法向约束
// ============================================
TEST_F(AttitudeProfileTest, NadirNormalSemantics)
{
    auto orbit = MakeCircularEquatorialOrbit();
    AttitudeNadirNormal profile;
    profile.setPoint(orbit.get());
    profile.setFrame(aGetEarth()->getFrameInertial());

    TimePoint tp = TestEpoch();
    Rotation rot;
    ASSERT_EQ(profile.getTransform(tp, rot), eNoError);
    ExpectOrthonormal(rot.getMatrix());

    // 体 Z 沿对地(-X)，体 X 沿轨道法向(+Z)
    EXPECT_NEAR(rot.transformVector(Vector3d{-1.0, 0.0, 0.0})[2], 1.0, 1e-12);
    EXPECT_NEAR(rot.transformVector(Vector3d{0.0, 0.0, 1.0})[0], 1.0, 1e-12);

    // 体 X 与体 Z 正交
    const Matrix3d& m = rot.getMatrix();
    EXPECT_NEAR(m(0, 0) * m(2, 0) + m(0, 1) * m(2, 1) + m(0, 2) * m(2, 2), 0.0, 1e-14);
}

// ============================================
// T9: 机体 Z 朝下姿态 —— 与 ECFVelRadial 共用对齐，约束轴相反
//     本剖面的语义是反推得到的，这条测试把它固定成可执行的断言
// ============================================
TEST_F(AttitudeProfileTest, AircraftZDownVersusECFVelRadial)
{
    auto orbit = MakeGeneralOrbit();
    AttitudeAircraftZDown zDown;
    AttitudeECFVelRadial  radial;
    zDown.setPoint(orbit.get());
    zDown.setBody(aGetEarth());
    radial.setPoint(orbit.get());
    radial.setBody(aGetEarth());

    TimePoint tp = TestTime();
    Vector3d pos, vel;
    ASSERT_EQ(orbit->getPosVelIn(aFrameECF(), tp, pos, vel), eNoError);

    Rotation rotDown, rotRadial;
    ASSERT_EQ(zDown.getTransform(tp, rotDown), eNoError);
    ASSERT_EQ(radial.getTransform(tp, rotRadial), eNoError);
    ExpectOrthonormal(rotDown.getMatrix());

    // 两者都是"体 X 沿固连系速度"，故第一行必须完全一致
    for (int c = 0; c < 3; c++)
        EXPECT_NEAR(rotDown.getMatrix()(0, c), rotRadial.getMatrix()(0, c), 1e-12);

    // 约束轴相反：朝下姿态的体 Z 径向分量为负，径向约束的为正
    const Vector3d radialDir = pos.normalized();
    EXPECT_LT(rotDown.transformVector(radialDir)[2], 0.0);
    EXPECT_GT(rotRadial.transformVector(radialDir)[2], 0.0);
}


// ============================================
// T11: 退化输入
// ============================================
TEST_F(AttitudeProfileTest, DegenerateGeometry)
{
    // 位置与速度平行(纯径向轨迹)：无法定姿
    {
        TestPoint point;
        point.frame_ = aFrameECI();
        point.pos_ = Vector3d{7000e3, 0.0, 0.0};
        point.vel_ = Vector3d{1000.0, 0.0, 0.0};

        AttitudeECIVVLH profile;
        profile.setPoint(&point);
        profile.setBody(aGetEarth());
        Rotation rot;
        EXPECT_EQ(profile.getTransform(TestEpoch(), rot), eErrorInvalidParam);
    }

    // 速度为零：无法定姿
    {
        TestPoint point;
        point.frame_ = aFrameECI();
        point.pos_ = Vector3d{7000e3, 0.0, 0.0};
        point.vel_ = Vector3d{0.0, 0.0, 0.0};

        AttitudeECIVVLH profile;
        profile.setPoint(&point);
        profile.setBody(aGetEarth());
        Rotation rot;
        EXPECT_EQ(profile.getTransform(TestEpoch(), rot), eErrorInvalidParam);
    }
 
}


// ============================================
// T13: 自旋姿态
// ============================================
TEST_F(AttitudeProfileTest, Spinning)
{
    const double rate = 0.25;
    TimePoint epoch = TestEpoch();
    TimePoint tp = TestTime();

    // 绕体 Z 与参考系 Z 自旋：退化为绕 Z 的匀速旋转
    {
        AttitudeSpinning profile;
        profile.setSpinAxisInFrame(Vector3d::UnitZ());
        profile.setSpinAxisInBody(Vector3d::UnitZ());
        profile.setSpinRate(rate);
        profile.setEpoch(epoch);

        Rotation rot;
        ASSERT_EQ(profile.getTransform(tp, rot), eNoError);
        Matrix3d expected;
        aRotationZMatrix(rate * (tp - epoch), expected);
        ExpectSameMatrix(rot.getMatrix(), expected, 1e-13);
        ExpectOrthonormal(rot.getMatrix());

        // 正角速度表示绕自旋轴的右手旋转
        KinematicRotation kr;
        ASSERT_EQ(profile.getTransform(tp, kr), eNoError);
        EXPECT_NEAR(kr.getRotationRate()[0], 0.0, 1e-14);
        EXPECT_NEAR(kr.getRotationRate()[1], 0.0, 1e-14);
        EXPECT_NEAR(kr.getRotationRate()[2], rate, 1e-14);
    }

    // 自旋轴倾斜：任何时刻自旋轴在体系中都应保持为 (0,0,1)，
    // 且相对参考系的角速度恒为 rate * axis
    {
        const Vector3d axis = Vector3d{1.0, 2.0, 3.0}.normalized();
        AttitudeSpinning profile;
        profile.setSpinAxisInFrame(axis);
        profile.setSpinAxisInBody(Vector3d::UnitZ());
        profile.setSpinRate(rate);
        profile.setEpoch(epoch);

        for (int i = 0; i < 5; i++)
        {
            TimePoint t = tp + i * 7.0;
            Rotation rot;
            ASSERT_EQ(profile.getTransform(t, rot), eNoError);
            ExpectOrthonormal(rot.getMatrix());

            const Vector3d inBody = rot.transformVector(axis);
            EXPECT_NEAR(inBody[0], 0.0, 1e-13);
            EXPECT_NEAR(inBody[1], 0.0, 1e-13);
            EXPECT_NEAR(inBody[2], 1.0, 1e-13);

            KinematicRotation kr;
            ASSERT_EQ(profile.getTransform(t, kr), eNoError);
            for (int k = 0; k < 3; k++)
                EXPECT_NEAR(kr.getRotationRate()[k], axis[k] * rate, 1e-14) << "time step " << i;
        }
    }

    // 自旋轴为零向量属于参数错误
    {
        AttitudeSpinning profile;
        profile.setSpinAxisInFrame(Vector3d::Zero());
        Rotation rot;
        EXPECT_EQ(profile.getTransform(TestTime(), rot), eErrorInvalidParam);
    }
}

// ============================================
// T15: Mover 集成 —— 姿态属性向下转型
// ============================================
TEST_F(AttitudeProfileTest, MoverAttitudeRoundTrip)
{
    Mover mover;

    AttitudeECIVVLH* profile = new AttitudeECIVVLH();
    mover.setAttitudeProfile(profile);
    EXPECT_EQ(mover.getAttitudeProfile(), profile);
    EXPECT_EQ(mover.orientation(), static_cast<Axes*>(profile));

    mover.setOrientation(AxesFrozen::New(aAxesECF(), TestEpoch(), aAxesICRF()));
    EXPECT_NE(mover.getAttitudeProfile(), nullptr);
}


GTEST_MAIN()

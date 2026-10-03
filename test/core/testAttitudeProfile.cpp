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

namespace {

/// @brief 测试用姿态剖面：体轴系相对父轴系以恒定角速度绕 Z 轴旋转
/// @details 直接返回 Rz(rate * t)，即"父系到体系"的转换矩阵。
///          按约定，其角速度应为 +rate * z(在父系下分解)。
class TestSpinZ : public AttitudeProfileBase
{
public:
    explicit TestSpinZ(double rate) : rate_(rate) {}

    using AttitudeProfileBase::getTransform;
    errc_t getTransform(const TimePoint& tp, Rotation& rotation) const override
    {
        Matrix3d mtx;
        aRotationZMatrix(rate_ * (tp - epoch_), mtx);
        rotation = Rotation::FromMatrix(mtx);
        return eNoError;
    }

    TimePoint epoch_{};
    double rate_{0.0};
};

/// @brief 测试用姿态剖面：恒为单位旋转
class TestIdentity : public AttitudeProfileBase
{
public:
    using AttitudeProfileBase::getTransform;
    errc_t getTransform(const TimePoint& tp, Rotation& rotation) const override
    {
        rotation = Rotation::Identity();
        return eNoError;
    }
};

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

}  // namespace

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



TEST_F(AttitudeProfileTest, TransformMapsParentToBody)
{
    const double rate = 0.1;   // rad/s
    TestSpinZ profile(rate);
    profile.epoch_ = TestEpoch();

    TimePoint tp = TestTime();
    const double t = tp - profile.epoch_;

    Rotation rot;
    ASSERT_EQ(profile.getTransform(tp, rot), eNoError);

    const Matrix3d& m = rot.getMatrix();
    const double c = std::cos(rate * t);
    const double s = std::sin(rate * t);
    EXPECT_NEAR(m(0, 0), c, 1e-14);
    EXPECT_NEAR(m(0, 1), s, 1e-14);
    EXPECT_NEAR(m(1, 0), -s, 1e-14);
    EXPECT_NEAR(m(1, 1), c, 1e-14);

    // transformVector 把父系分量映到体系分量：v_body = M * v_parent
    Vector3d bodyX = rot.transformVector(Vector3d{1.0, 0.0, 0.0});
    EXPECT_NEAR(bodyX[0], c, 1e-14);
    EXPECT_NEAR(bodyX[1], -s, 1e-14);
    EXPECT_NEAR(bodyX[2], 0.0, 1e-14);

    // transformVectorInv 把体系分量映回父系：父系下的体 X 轴
    Vector3d parentX = rot.transformVectorInv(Vector3d{1.0, 0.0, 0.0});
    EXPECT_NEAR(parentX[0], c, 1e-14);
    EXPECT_NEAR(parentX[1], s, 1e-14);
    EXPECT_NEAR(parentX[2], 0.0, 1e-14);
}

// ============================================
// T1b: 经由 aAxesTransform 的链路方向
//      aAxesTransform(s, t) 给的是 v_t = rot * v_s，故 剖面 -> ICRF 应得到 M 的逆
// ============================================
TEST_F(AttitudeProfileTest, AxesTransformDirection)
{
    const double rate = 0.1;
    TestSpinZ profile(rate);
    profile.epoch_ = TestEpoch();

    TimePoint tp = TestTime();
    const double t = tp - profile.epoch_;

    ASSERT_EQ(profile.getParent(), aAxesICRF());

    Rotation rot;
    ASSERT_EQ(aAxesTransform(&profile, aAxesICRF(), tp, rot), eNoError);

    const Matrix3d& m = rot.getMatrix();
    EXPECT_NEAR(m(0, 0), std::cos(rate * t), 1e-13);
    EXPECT_NEAR(m(0, 1), -std::sin(rate * t), 1e-13);
    EXPECT_NEAR(m(1, 0), std::sin(rate * t), 1e-13);
}

// ============================================
// T2: 角速度的符号与参考系
//     Rz(+n*t) 对应 +n * z；Rz(-n*t) 对应 -n * z
// ============================================
TEST_F(AttitudeProfileTest, AngularVelocitySign)
{
    TimePoint tp = TestTime();

    {
        const double rate = 0.1;
        TestSpinZ profile(rate);
        profile.epoch_ = TestEpoch();
        KinematicRotation kr;
        ASSERT_EQ(profile.getTransform(tp, kr), eNoError);
        EXPECT_NEAR(kr.getRotationRate()[0], 0.0, 1e-9);
        EXPECT_NEAR(kr.getRotationRate()[1], 0.0, 1e-9);
        EXPECT_NEAR(kr.getRotationRate()[2], rate, 1e-9);
    }
    {
        const double rate = -0.35;
        TestSpinZ profile(rate);
        profile.epoch_ = TestEpoch();
        KinematicRotation kr;
        ASSERT_EQ(profile.getTransform(tp, kr), eNoError);
        EXPECT_NEAR(kr.getRotationRate()[2], rate, 1e-9);
    }
}

// ============================================
// T2b: 两个重载给出一致的旋转矩阵；恒等剖面的角速度严格为零
// ============================================
TEST_F(AttitudeProfileTest, KinematicRotationMatchesRotation)
{
    TimePoint tp = TestTime();

    TestSpinZ profile(0.2);
    profile.epoch_ = TestEpoch();
    Rotation rot;
    KinematicRotation kr;
    ASSERT_EQ(profile.getTransform(tp, rot), eNoError);
    ASSERT_EQ(profile.getTransform(tp, kr), eNoError);
    ExpectSameMatrix(kr.getMatrix(), rot.getMatrix(), 1e-14);

    TestIdentity identity;
    ASSERT_EQ(identity.getTransform(tp, kr), eNoError);
    EXPECT_NEAR(kr.getRotationRate().norm(), 0.0, 1e-12);
}

// ============================================
// T3: 未设置载体与参考坐标系的默认行为
// ============================================
TEST_F(AttitudeProfileTest, DefaultPointAndFrame)
{
    TestIdentity profile;
    EXPECT_EQ(profile.getPoint(), nullptr);
    EXPECT_EQ(profile.getFrame(), aFrameECI());
    EXPECT_EQ(profile.getParent(), aAxesICRF());

    // setFrame(nullptr) 恢复为自然参考坐标系
    profile.setFrame(aFrameECF());
    EXPECT_EQ(profile.getFrame(), aFrameECF());
    EXPECT_EQ(profile.getParent(), aAxesECF());
    profile.setFrame(nullptr);
    EXPECT_EQ(profile.getFrame(), aFrameECI());
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
// T12: 固定姿态与 YPR 固定姿态
// ============================================
TEST_F(AttitudeProfileTest, FixedAndYPR)
{
    // 默认是单位旋转，角速度严格为零(不是 1e-16 量级)
    {
        AttitudeFixed profile;
        Rotation rot;
        KinematicRotation kr;
        ASSERT_EQ(profile.getTransform(TestTime(), rot), eNoError);
        ASSERT_EQ(profile.getTransform(TestTime(), kr), eNoError);
        for (int r = 0; r < 3; r++)
            for (int c = 0; c < 3; c++)
                EXPECT_EQ(rot.getMatrix()(r, c), (r == c) ? 1.0 : 0.0);
        EXPECT_EQ(kr.getRotationRate().norm(), 0.0);
    }

    const double yaw = 42.0 * kDegToRad;
    AttitudeYPRFixedECI profile;
    profile.setYaw(yaw);

    Rotation rot;
    ASSERT_EQ(profile.getTransform(TestTime(), rot), eNoError);
    const Matrix3d& m = rot.getMatrix();
    EXPECT_NEAR(m(0, 0), std::cos(yaw), 1e-15);
    EXPECT_NEAR(m(0, 1), std::sin(yaw), 1e-15);

    // 与欧拉角转换函数针对同一转序的结果一致
    Euler euler;
    euler.angle1_ = yaw;
    euler.angle2_ = 0.0;
    euler.angle3_ = 0.0;
    Matrix3d expected;
    ASSERT_EQ(aEulerToMatrix(euler, profile.getUiSequence(), expected), eNoError);
    ExpectSameMatrix(rot.getMatrix(), expected, 1e-15);

    // 往返：由矩阵反解欧拉角应还原输入
    Euler back;
    ASSERT_EQ(aMatrixToEuler(m, profile.getUiSequence(), back), eNoError);
    EXPECT_NEAR(back.angle1_, yaw, 1e-12);
    EXPECT_NEAR(back.angle2_, 0.0, 1e-12);
    EXPECT_NEAR(back.angle3_, 0.0, 1e-12);

    KinematicRotation kr;
    ASSERT_EQ(profile.getTransform(TestTime(), kr), eNoError);
    EXPECT_EQ(kr.getRotationRate().norm(), 0.0);
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

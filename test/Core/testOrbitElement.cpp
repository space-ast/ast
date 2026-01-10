/// @file      testOrbitElement.cpp
/// @brief     轨道根数转换往返测试
/// @details   测试各种轨道根数（经典轨道根数、修正轨道根数、春分点根数、改进春分点轨道根数、直角坐标）之间的双向转换精度。
/// @author    axel
/// @date      20.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

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

#include "AstCore/OrbitElement.hpp"
#include "AstCore/Vector.hpp"
#include "AstCore/Constants.h"
#include "AstTest/AstTestMacro.h"
#include <cstdio>

AST_USING_NAMESPACE

// 测试常量
const double CB_RADIUS = 6378137.0; // 地球赤道半径 [m]
const double GM = 3.986004418e14;    // 地球引力参数 [m^3/s^2]
const double EPS = 1e-7;             // 一般精度要求
const double POS_EPS = 1e-3;         // 位置精度要求（米）
const double VEL_EPS = 1e-3;         // 速度精度要求（米/秒）
const double ANGLE_EPS = 1e-9;       // 角度精度要求（弧度）

/// @brief 测试经典轨道根数与直角坐标之间的转换
TEST(OrbitElement, CartConversion)
{
    printf("测试: 经典轨道根数 ←→ 直角坐标\n");
    
    // 测试用例1：低地球轨道
    {
        // 初始经典轨道根数
        OrbElem coe1 = {
            7000000.0,  // 长半轴 [m]
            0.01,       // 偏心率
            0.1,        // 轨道倾角 [rad]
            0.2,        // 升交点赤经 [rad]
            0.3,        // 近拱点角 [rad]
            0.4         // 真近点角 [rad]
        };
        
        // 转换到直角坐标
        Vector3d pos1, vel1;
        err_t err = aOrbElemToCart(coe1, GM, pos1, vel1);
        EXPECT_EQ(err, eNoError);
        
        // 转换回经典轨道根数
        OrbElem coe1_back;
        err = aCartToOrbElem(pos1, vel1, GM, coe1_back);
        EXPECT_EQ(err, eNoError);
        
        // 验证转换精度
        EXPECT_NEAR(coe1.a(), coe1_back.a(), EPS);
        EXPECT_NEAR(coe1.e(), coe1_back.e(), EPS);
        EXPECT_NEAR(coe1.i(), coe1_back.i(), ANGLE_EPS);
        
        // 角度参数需要考虑模2π的情况
        double raan_diff = fabs(coe1.raan() - coe1_back.raan());
        if (raan_diff > kPI) raan_diff = 2 * kPI - raan_diff;
        EXPECT_NEAR(raan_diff, 0.0, ANGLE_EPS);
        
        double argper_diff = fabs(coe1.argper() - coe1_back.argper());
        if (argper_diff > kPI) argper_diff = 2 * kPI - argper_diff;
        EXPECT_NEAR(argper_diff, 0.0, ANGLE_EPS);
        
        double trueA_diff = fabs(coe1.trueA() - coe1_back.trueA());
        if (trueA_diff > kPI) trueA_diff = 2 * kPI - trueA_diff;
        EXPECT_NEAR(trueA_diff, 0.0, ANGLE_EPS);
    }
    
    // 测试用例2：同步轨道
    {
        // 初始直角坐标（简化的地球同步轨道）
        Vector3d pos2 = {42164000.0, 0.0, 0.0};
        Vector3d vel2 = {0.0, 3074.65, 0.0};
        
        // 转换到经典轨道根数
        OrbElem coe2;
        err_t err = aCartToOrbElem(pos2, vel2, GM, coe2);
        EXPECT_EQ(err, eNoError);
        
        // 转换回直角坐标
        Vector3d pos2_back, vel2_back;
        err = aOrbElemToCart(coe2, GM, pos2_back, vel2_back);
        EXPECT_EQ(err, eNoError);
        
        // 验证转换精度
        EXPECT_NEAR(pos2.x(), pos2_back.x(), POS_EPS);
        EXPECT_NEAR(pos2.y(), pos2_back.y(), POS_EPS);
        EXPECT_NEAR(pos2.z(), pos2_back.z(), POS_EPS);
        EXPECT_NEAR(vel2.x(), vel2_back.x(), VEL_EPS);
        EXPECT_NEAR(vel2.y(), vel2_back.y(), VEL_EPS);
        EXPECT_NEAR(vel2.z(), vel2_back.z(), VEL_EPS);
    }
    
}

/// @brief 测试改进春分点轨道根数与直角坐标之间的转换
TEST(OrbitElement, ModEquinElemCartConversion)
{
    printf("测试: 改进春分点轨道根数 ←→ 直角坐标\n");
    
    // 测试用例：从改进春分点轨道根数到直角坐标再返回
    {
        // 初始改进春分点轨道根数
        ModEquinElem mee1 = {
            6930000.0,  // 半通径 [m]
            0.005,      // f
            0.008,      // g
            0.01,       // h
            0.02,       // k
            0.5         // L [rad]
        };
        
        // 转换到直角坐标
        Vector3d pos1, vel1;
        aModEquinElemToCart(mee1, GM, pos1, vel1);
        
        // 转换回改进春分点轨道根数
        ModEquinElem mee1_back;
        err_t err = aCartToModEquinElem(pos1, vel1, GM, mee1_back);
        EXPECT_EQ(err, eNoError);
        
        // 验证转换精度
        EXPECT_NEAR(mee1.p(), mee1_back.p(), EPS);
        EXPECT_NEAR(mee1.f(), mee1_back.f(), EPS);
        EXPECT_NEAR(mee1.g(), mee1_back.g(), EPS);
        EXPECT_NEAR(mee1.h(), mee1_back.h(), EPS);
        EXPECT_NEAR(mee1.k(), mee1_back.k(), EPS);
        
        // L需要考虑模2π的情况
        double L_diff = fabs(mee1.L() - mee1_back.L());
        if (L_diff > kPI) L_diff = 2 * kPI - L_diff;
        EXPECT_NEAR(L_diff, 0.0, ANGLE_EPS);
    }
    
}

/// @brief 测试修正轨道根数与直角坐标之间的转换
TEST(OrbitElement, ModOrbElemCartConversion)
{
    printf("测试: 修正轨道根数 ←→ 直角坐标\n");
    
    // 测试用例
    {
        // 初始修正轨道根数
        ModOrbElem moe1 = {
            6800000.0,  // 近拱点半径 [m]
            0.02,       // 偏心率
            0.15,       // 轨道倾角 [rad]
            0.25,       // 升交点赤经 [rad]
            0.35,       // 近拱点角 [rad]
            0.45        // 真近点角 [rad]
        };
        
        // 转换到直角坐标
        Vector3d pos1, vel1;
        aModOrbElemToCart(moe1, GM, pos1, vel1);
        
        // 转换回修正轨道根数
        ModOrbElem moe1_back;
        err_t err = aCartToModOrbElem(pos1, vel1, GM, moe1_back);
        EXPECT_EQ(err, eNoError);
        
        // 验证转换精度
        EXPECT_NEAR(moe1.rp(), moe1_back.rp(), EPS);
        EXPECT_NEAR(moe1.e(), moe1_back.e(), EPS);
        EXPECT_NEAR(moe1.i(), moe1_back.i(), ANGLE_EPS);
        
        // 角度参数需要考虑模2π的情况
        double raan_diff = fabs(moe1.raan() - moe1_back.raan());
        if (raan_diff > kPI) raan_diff = 2 * kPI - raan_diff;
        EXPECT_NEAR(raan_diff, 0.0, ANGLE_EPS);
        
        double argper_diff = fabs(moe1.argper() - moe1_back.argper());
        if (argper_diff > kPI) argper_diff = 2 * kPI - argper_diff;
        EXPECT_NEAR(argper_diff, 0.0, ANGLE_EPS);
        
        double trueA_diff = fabs(moe1.trueA() - moe1_back.trueA());
        if (trueA_diff > kPI) trueA_diff = 2 * kPI - trueA_diff;
        EXPECT_NEAR(trueA_diff, 0.0, ANGLE_EPS);
    }
    
}

/// @brief 测试春分点根数与直角坐标之间的转换
TEST(OrbitElement, EquinElemCartConversion)
{
    printf("测试: 春分点根数 ←→ 直角坐标\n");
    
    // 测试用例
    {
        // 初始春分点根数
        EquinElem ee1 = {
            7100000.0,  // 长半轴 [m]
            0.006,      // h
            0.009,      // k
            0.015,      // p
            0.025,      // q
            0.6         // lambda [rad]
        };
        
        // 转换到直角坐标
        Vector3d pos1, vel1;
        aEquinElemToCart(ee1, GM, pos1, vel1);
        
        // 转换回春分点根数
        EquinElem ee1_back;
        aCartToEquinElem(pos1, vel1, GM, ee1_back);
        
        // 验证转换精度
        EXPECT_NEAR(ee1.a(), ee1_back.a(), EPS);
        EXPECT_NEAR(ee1.h(), ee1_back.h(), EPS);
        EXPECT_NEAR(ee1.k(), ee1_back.k(), EPS);
        EXPECT_NEAR(ee1.p(), ee1_back.p(), EPS);
        EXPECT_NEAR(ee1.q(), ee1_back.q(), EPS);
        
        // lambda需要考虑模2π的情况
        double lambda_diff = fabs(ee1.lambda() - ee1_back.lambda());
        if (lambda_diff > kPI) lambda_diff = 2 * kPI - lambda_diff;
        EXPECT_NEAR(lambda_diff, 0.0, ANGLE_EPS);
    }
    
}

/// @brief 测试各种轨道根数之间的相互转换
TEST(OrbitElement, ElementInterconversion)
{
    printf("测试: 各种轨道根数之间的相互转换\n");
    
    // 初始经典轨道根数
    OrbElem coe_original = {
        7000000.0,  // 长半轴 [m]
        0.01,       // 偏心率
        0.1,        // 轨道倾角 [rad]
        0.2,        // 升交点赤经 [rad]
        0.3,        // 近拱点角 [rad]
        0.4         // 真近点角 [rad]
    };
    
    // 经典轨道根数 -> 改进春分点轨道根数 -> 经典轨道根数
    {
        ModEquinElem mee;
        err_t err = aOrbElemToModEquinElem(coe_original, mee);
        EXPECT_EQ(err, eNoError);
        
        OrbElem coe_back;
        err = aModEquinElemToOrbElem(mee, coe_back);
        EXPECT_EQ(err, eNoError);
        
        // 验证转换精度
        EXPECT_NEAR(coe_original.a(), coe_back.a(), EPS);
        EXPECT_NEAR(coe_original.e(), coe_back.e(), EPS);
        EXPECT_NEAR(coe_original.i(), coe_back.i(), ANGLE_EPS);
    }
    
    // 经典轨道根数 -> 春分点根数 -> 经典轨道根数
    {
        EquinElem ee;
        coe2ee(coe_original.data(), ee.data());
        
        OrbElem coe_back;
        ee2coe(ee.data(), coe_back.data());
        
        // 验证转换精度
        EXPECT_NEAR(coe_original.a(), coe_back.a(), EPS);
        EXPECT_NEAR(coe_original.e(), coe_back.e(), EPS);
        EXPECT_NEAR(coe_original.i(), coe_back.i(), ANGLE_EPS);
    }
    
    // 经典轨道根数 -> 修正轨道根数 -> 经典轨道根数
    {
        ModOrbElem moe;
        coe2moe(coe_original.data(), moe.data());
        
        OrbElem coe_back;
        moe2coe(moe.data(), coe_back.data());
        
        // 验证转换精度
        EXPECT_NEAR(coe_original.a(), coe_back.a(), EPS);
        EXPECT_NEAR(coe_original.e(), coe_back.e(), EPS);
        EXPECT_NEAR(coe_original.i(), coe_back.i(), ANGLE_EPS);
    }
    
}

/// @brief 测试特殊轨道情况下的轨道根数转换
TEST(OrbitElement, SpecialOrbitalCases) {
    printf("测试: 特殊轨道情况\n");
    
    // 测试用例1: 近圆形轨道（偏心率接近0）
    {
        OrbElem coe_circular = {
            7000000.0,  // 长半轴 [m]
            1e-7,       // 接近0的偏心率
            0.1,        // 轨道倾角 [rad]
            0.2,        // 升交点赤经 [rad]
            0.3,        // 近拱点角 [rad]
            0.4         // 真近点角 [rad]
        };
        
        // 转换到直角坐标
        Vector3d pos, vel;
        err_t err = aOrbElemToCart(coe_circular, GM, pos, vel);
        EXPECT_EQ(err, eNoError);
        
        // 转换回经典轨道根数
        OrbElem coe_back;
        err = aCartToOrbElem(pos, vel, GM, coe_back);
        EXPECT_EQ(err, eNoError);
        
        // 验证转换精度（对于近圆轨道，近拱点角可能不准确，但其他参数应保持一致）
        EXPECT_NEAR(coe_circular.a(), coe_back.a(), EPS);
        EXPECT_NEAR(coe_circular.e(), coe_back.e(), 1e-6); // 放宽偏心率精度要求
        EXPECT_NEAR(coe_circular.i(), coe_back.i(), ANGLE_EPS);
    }
    
    // 测试用例2: 零倾角轨道
    {
        OrbElem coe_zero_inc = {
            7000000.0,  // 长半轴 [m]
            0.01,       // 偏心率
            0.0,        // 零轨道倾角
            0.2,        // 升交点赤经 [rad]
            0.3,        // 近拱点角 [rad]
            0.4         // 真近点角 [rad]
        };
        
        // 转换到直角坐标
        Vector3d pos, vel;
        err_t err = aOrbElemToCart(coe_zero_inc, GM, pos, vel);
        EXPECT_EQ(err, eNoError);
        
        // 转换回经典轨道根数
        OrbElem coe_back;
        err = aCartToOrbElem(pos, vel, GM, coe_back);
        EXPECT_EQ(err, eNoError);
        
        // 验证转换精度（对于零倾角轨道，升交点赤经可能不准确）
        EXPECT_NEAR(coe_zero_inc.a(), coe_back.a(), EPS);
        EXPECT_NEAR(coe_zero_inc.e(), coe_back.e(), EPS);
        EXPECT_NEAR(coe_zero_inc.i(), coe_back.i(), ANGLE_EPS);
    }
    
    // 测试用例3: 临界情况 - 抛物线轨道检测
    {
        ModOrbElem moe_parabolic = {
            6800000.0,  // 近拱点半径 [m]
            1.0,        // 抛物线轨道偏心率 = 1.0
            0.1,        // 轨道倾角 [rad]
            0.2,        // 升交点赤经 [rad]
            0.3,        // 近拱点角 [rad]
            0.4         // 真近点角 [rad]
        };
        
        // 测试错误处理
        Vector3d pos, vel;
        err_t err = aModOrbElemToCart(moe_parabolic, GM, pos, vel);
        EXPECT_NE(err, eNoError); // 应该返回错误
    }
}

/// @brief 测试轨道根数之间的直接转换函数
TEST(OrbitElement, DirectConversionFunctions) {
    printf("测试: 直接转换函数\n");
    
    // 测试用例1: 春分点根数 <-> 修正轨道根数
    {
        // 初始春分点根数
        EquinElem ee_original = {
            7000000.0,  // 长半轴 [m]
            0.005,      // h
            0.008,      // k
            0.01,       // p
            0.02,       // q
            0.5         // lambda [rad]
        };
        
        // 转换到修正轨道根数
        ModOrbElem moe;
        err_t err = aEquinElemToModOrb(ee_original, moe);
        EXPECT_EQ(err, eNoError);
        
        // 转换回春分点根数
        EquinElem ee_back;
        err = aModOrbToEquinElem(moe, ee_back);
        EXPECT_EQ(err, eNoError);
        
        // 验证转换精度
        EXPECT_NEAR(ee_original.a(), ee_back.a(), EPS);
        EXPECT_NEAR(ee_original.h(), ee_back.h(), EPS);
        EXPECT_NEAR(ee_original.k(), ee_back.k(), EPS);
        EXPECT_NEAR(ee_original.p(), ee_back.p(), EPS);
        EXPECT_NEAR(ee_original.q(), ee_back.q(), EPS);
        
        // lambda需要考虑模2π的情况
        double lambda_diff = fabs(ee_original.lambda() - ee_back.lambda());
        if (lambda_diff > kPI) lambda_diff = 2 * kPI - lambda_diff;
        EXPECT_NEAR(lambda_diff, 0.0, ANGLE_EPS);
    }
    
    // 测试用例2: 改进春分点轨道根数 <-> 修正轨道根数
    {
        // 初始改进春分点轨道根数
        ModEquinElem mee_original = {
            6930000.0,  // 半通径 [m]
            0.005,      // f
            0.008,      // g
            0.01,       // h
            0.02,       // k
            0.5         // L [rad]
        };
        
        // 转换到修正轨道根数 (通过中间步骤)
        ModOrbElem moe;
        mee2moe(mee_original.data(), moe.data());
        
        // 转换回改进春分点轨道根数
        ModEquinElem mee_back;
        moe2mee(moe.data(), mee_back.data());
        
        // 验证转换精度
        EXPECT_NEAR(mee_original.p(), mee_back.p(), EPS);
        EXPECT_NEAR(mee_original.f(), mee_back.f(), EPS);
        EXPECT_NEAR(mee_original.g(), mee_back.g(), EPS);
        EXPECT_NEAR(mee_original.h(), mee_back.h(), EPS);
        EXPECT_NEAR(mee_original.k(), mee_back.k(), EPS);
        
        // L需要考虑模2π的情况
        double L_diff = fabs(mee_original.L() - mee_back.L());
        if (L_diff > kPI) L_diff = 2 * kPI - L_diff;
        EXPECT_NEAR(L_diff, 0.0, ANGLE_EPS);
    }
    
    // 测试用例3: 春分点根数 <-> 改进春分点轨道根数
    {
        // 初始春分点根数
        EquinElem ee_original = {
            7000000.0,  // 长半轴 [m]
            0.005,      // h
            0.008,      // k
            0.01,       // p
            0.02,       // q
            0.5         // lambda [rad]
        };
        
        // 转换到改进春分点轨道根数
        ModEquinElem mee;
        ee2mee(ee_original.data(), mee.data());
        
        // 转换回春分点根数
        EquinElem ee_back;
        mee2ee(mee.data(), ee_back.data());
        
        // 验证转换精度
        EXPECT_NEAR(ee_original.a(), ee_back.a(), EPS);
        EXPECT_NEAR(ee_original.h(), ee_back.h(), EPS);
        EXPECT_NEAR(ee_original.k(), ee_back.k(), EPS);
        EXPECT_NEAR(ee_original.p(), ee_back.p(), EPS);
        EXPECT_NEAR(ee_original.q(), ee_back.q(), EPS);
        
        // lambda需要考虑模2π的情况
        double lambda_diff = fabs(ee_original.lambda() - ee_back.lambda());
        if (lambda_diff > kPI) lambda_diff = 2 * kPI - lambda_diff;
        EXPECT_NEAR(lambda_diff, 0.0, ANGLE_EPS);
    }
}

/// @brief 测试错误处理
TEST(OrbitElement, ErrorHandling) {
    printf("测试: 错误处理\n");
    
    // 测试用例1: 抛物线轨道错误处理 (e=1)
    {
        ModOrbElem moe_parabolic = {
            6800000.0,  // 近拱点半径 [m]
            1.0,        // 抛物线轨道偏心率 = 1.0
            0.1,        // 轨道倾角 [rad]
            0.2,        // 升交点赤经 [rad]
            0.3,        // 近拱点角 [rad]
            0.4         // 真近点角 [rad]
        };
        
        // 测试moe2rv函数的错误处理
        Vector3d pos, vel;
        err_t err = moe2rv(moe_parabolic.data(), GM, pos.data(), vel.data());
        EXPECT_EQ(err, eErrorInvalidParam);
        
        // 测试moe2coe函数的错误处理
        OrbElem coe;
        err = moe2coe(moe_parabolic.data(), coe.data());
        EXPECT_EQ(err, eErrorInvalidParam);
        
        // 测试ee2moe函数的错误处理
        EquinElem ee;
        err = moe2ee(moe_parabolic.data(), ee.data());
        err = ee2moe(ee.data(), moe_parabolic.data());
        EXPECT_NE(err, eNoError);
    }
    
    // 测试用例2: 临界角错误处理 (inc=π)
    {
        // 创建一个临界情况的输入
        double moe_data[6] = {
            6800000.0,  // 近拱点半径 [m]
            0.01,       // 偏心率
            kPI,        // 轨道倾角 = π (180度)
            0.2,        // 升交点赤经 [rad]
            0.3,        // 近拱点角 [rad]
            0.4         // 真近点角 [rad]
        };
        
        EquinElem ee;
        err_t err = moe2ee(moe_data, ee.data());
        EXPECT_EQ(err, eErrorInvalidParam);
    }
}

/// @brief 测试多种轨道类型的一致性
TEST(OrbitElement, OrbitalConsistency) {
    printf("测试: 多种轨道类型的一致性\n");
    
    // 测试用例: 从同一起始点转换到不同轨道根数，再转回直角坐标，验证结果一致性
    {
        // 初始直角坐标
        Vector3d pos_original = {6778137.0, 0.0, 0.0};  // 低地球轨道位置
        Vector3d vel_original = {0.0, 7726.0, 0.0};      // 低地球轨道速度
        
        // 转换到不同的轨道根数表示
        OrbElem coe;
        ModOrbElem moe;
        EquinElem ee;
        ModEquinElem mee;
        
        err_t err = aCartToOrbElem(pos_original, vel_original, GM, coe);
        EXPECT_EQ(err, eNoError);
        
        err = aCartToModOrbElem(pos_original, vel_original, GM, moe);
        EXPECT_EQ(err, eNoError);
        
        aCartToEquinElem(pos_original, vel_original, GM, ee);
        
        err = aCartToModEquinElem(pos_original, vel_original, GM, mee);
        EXPECT_EQ(err, eNoError);
        
        // 从不同轨道根数转回直角坐标
        Vector3d pos_coe, vel_coe;
        Vector3d pos_moe, vel_moe;
        Vector3d pos_ee, vel_ee;
        Vector3d pos_mee, vel_mee;
        
        aOrbElemToCart(coe, GM, pos_coe, vel_coe);
        aModOrbElemToCart(moe, GM, pos_moe, vel_moe);
        aEquinElemToCart(ee, GM, pos_ee, vel_ee);
        aModEquinElemToCart(mee, GM, pos_mee, vel_mee);
        
        // 验证所有转换结果一致性
        EXPECT_NEAR(pos_coe.x(), pos_moe.x(), POS_EPS);
        EXPECT_NEAR(pos_coe.y(), pos_moe.y(), POS_EPS);
        EXPECT_NEAR(pos_coe.z(), pos_moe.z(), POS_EPS);
        EXPECT_NEAR(vel_coe.x(), vel_moe.x(), VEL_EPS);
        EXPECT_NEAR(vel_coe.y(), vel_moe.y(), VEL_EPS);
        EXPECT_NEAR(vel_coe.z(), vel_moe.z(), VEL_EPS);
        
        EXPECT_NEAR(pos_coe.x(), pos_ee.x(), POS_EPS);
        EXPECT_NEAR(pos_coe.y(), pos_ee.y(), POS_EPS);
        EXPECT_NEAR(pos_coe.z(), pos_ee.z(), POS_EPS);
        EXPECT_NEAR(vel_coe.x(), vel_ee.x(), VEL_EPS);
        EXPECT_NEAR(vel_coe.y(), vel_ee.y(), VEL_EPS);
        EXPECT_NEAR(vel_coe.z(), vel_ee.z(), VEL_EPS);
        
        EXPECT_NEAR(pos_coe.x(), pos_mee.x(), POS_EPS);
        EXPECT_NEAR(pos_coe.y(), pos_mee.y(), POS_EPS);
        EXPECT_NEAR(pos_coe.z(), pos_mee.z(), POS_EPS);
        EXPECT_NEAR(vel_coe.x(), vel_mee.x(), VEL_EPS);
        EXPECT_NEAR(vel_coe.y(), vel_mee.y(), VEL_EPS);
        EXPECT_NEAR(vel_coe.z(), vel_mee.z(), VEL_EPS);
    }
}

/// @brief 测试Delaunay轨道根数与经典轨道根数之间的转换
TEST(OrbitElement, DelaunayConversion)
{
    printf("测试: Delaunay轨道根数 ←→ 经典轨道根数\n");
    
    // 测试用例1：正常轨道
    {
        // 初始经典轨道根数
        OrbElem coe_original = {
            7000000.0,  // 长半轴 [m]
            0.01,       // 偏心率
            0.1,        // 轨道倾角 [rad]
            0.2,        // 升交点赤经 [rad]
            0.3,        // 近拱点角 [rad]
            0.4         // 真近点角 [rad]
        };
        
        // 转换到Delaunay轨道根数
        DelaunayElem dela;
        err_t err = aOrbElemToDelaunay(coe_original, GM, dela);
        EXPECT_EQ(err, eNoError);
        
        // 转换回经典轨道根数
        OrbElem coe_back;
        err = aDelaunayToOrbElem(dela, GM, coe_back);
        EXPECT_EQ(err, eNoError);
        
        // 验证转换精度
        EXPECT_NEAR(coe_original.a(), coe_back.a(), EPS);
        EXPECT_NEAR(coe_original.e(), coe_back.e(), EPS);
        EXPECT_NEAR(coe_original.i(), coe_back.i(), ANGLE_EPS);
        
        // 角度参数需要考虑模2π的情况
        double raan_diff = fabs(coe_original.raan() - coe_back.raan());
        if (raan_diff > kPI) raan_diff = 2 * kPI - raan_diff;
        EXPECT_NEAR(raan_diff, 0.0, ANGLE_EPS);
        
        double argper_diff = fabs(coe_original.argper() - coe_back.argper());
        if (argper_diff > kPI) argper_diff = 2 * kPI - argper_diff;
        EXPECT_NEAR(argper_diff, 0.0, ANGLE_EPS);
        
        double trueA_diff = fabs(coe_original.trueA() - coe_back.trueA());
        if (trueA_diff > kPI) trueA_diff = 2 * kPI - trueA_diff;
        EXPECT_NEAR(trueA_diff, 0.0, ANGLE_EPS);
    }
    
    // 测试用例2：近圆轨道
    {
        // 初始经典轨道根数（近圆轨道）
        OrbElem coe_original = {
            7000000.0,  // 长半轴 [m]
            1e-7,       // 接近0的偏心率
            0.1,        // 轨道倾角 [rad]
            0.2,        // 升交点赤经 [rad]
            0.3,        // 近拱点角 [rad]
            0.4         // 真近点角 [rad]
        };
        
        // 转换到Delaunay轨道根数
        DelaunayElem dela;
        err_t err = aOrbElemToDelaunay(coe_original, GM, dela);
        EXPECT_EQ(err, eNoError);
        
        // 转换回经典轨道根数
        OrbElem coe_back;
        err = aDelaunayToOrbElem(dela, GM, coe_back);
        EXPECT_EQ(err, eNoError);
        
        // 验证转换精度（近圆轨道）
        EXPECT_NEAR(coe_original.a(), coe_back.a(), EPS);
        EXPECT_NEAR(coe_original.e(), coe_back.e(), 1e-6); // 放宽偏心率精度要求
        EXPECT_NEAR(coe_original.i(), coe_back.i(), ANGLE_EPS);
    }
    
    // 测试用例3：零倾角轨道
    {
        // 初始经典轨道根数（零倾角轨道）
        OrbElem coe_original = {
            7000000.0,  // 长半轴 [m]
            0.01,       // 偏心率
            0.0,        // 零轨道倾角
            0.2,        // 升交点赤经 [rad]
            0.3,        // 近拱点角 [rad]
            0.4         // 真近点角 [rad]
        };
        
        // 转换到Delaunay轨道根数
        DelaunayElem dela;
        err_t err = aOrbElemToDelaunay(coe_original, GM, dela);
        EXPECT_EQ(err, eNoError);
        
        // 转换回经典轨道根数
        OrbElem coe_back;
        err = aDelaunayToOrbElem(dela, GM, coe_back);
        EXPECT_EQ(err, eNoError);
        
        // 验证转换精度（零倾角轨道）
        EXPECT_NEAR(coe_original.a(), coe_back.a(), EPS);
        EXPECT_NEAR(coe_original.e(), coe_back.e(), EPS);
        EXPECT_NEAR(coe_original.i(), coe_back.i(), ANGLE_EPS);
    }
}

GTEST_MAIN()
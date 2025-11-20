/// @file      testOrbitElement.cpp
/// @brief     轨道根数转换往返测试
/// @details   测试各种轨道根数（经典轨道根数、修正轨道根数、春分点根数、改进春分点轨道根数、直角坐标）之间的双向转换精度。
/// @author    jinke18
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
int testOrbElemCartConversion()
{
    printf("测试: 经典轨道根数 ↔ 直角坐标\n");
    
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
        ASSERT_EQ(err, eNoError);
        
        // 转换回经典轨道根数
        OrbElem coe1_back;
        err = aCartToOrbElem(pos1, vel1, GM, coe1_back);
        ASSERT_EQ(err, eNoError);
        
        // 验证转换精度
        ASSERT_NEAR(coe1.a(), coe1_back.a(), EPS);
        ASSERT_NEAR(coe1.e(), coe1_back.e(), EPS);
        ASSERT_NEAR(coe1.i(), coe1_back.i(), ANGLE_EPS);
        
        // 角度参数需要考虑模2π的情况
        double raan_diff = fabs(coe1.raan() - coe1_back.raan());
        if (raan_diff > kPI) raan_diff = 2 * kPI - raan_diff;
        ASSERT_NEAR(raan_diff, 0.0, ANGLE_EPS);
        
        double argper_diff = fabs(coe1.argper() - coe1_back.argper());
        if (argper_diff > kPI) argper_diff = 2 * kPI - argper_diff;
        ASSERT_NEAR(argper_diff, 0.0, ANGLE_EPS);
        
        double trueA_diff = fabs(coe1.trueA() - coe1_back.trueA());
        if (trueA_diff > kPI) trueA_diff = 2 * kPI - trueA_diff;
        ASSERT_NEAR(trueA_diff, 0.0, ANGLE_EPS);
    }
    
    // 测试用例2：同步轨道
    {
        // 初始直角坐标（简化的地球同步轨道）
        Vector3d pos2 = {42164000.0, 0.0, 0.0};
        Vector3d vel2 = {0.0, 3074.65, 0.0};
        
        // 转换到经典轨道根数
        OrbElem coe2;
        err_t err = aCartToOrbElem(pos2, vel2, GM, coe2);
        ASSERT_EQ(err, eNoError);
        
        // 转换回直角坐标
        Vector3d pos2_back, vel2_back;
        err = aOrbElemToCart(coe2, GM, pos2_back, vel2_back);
        ASSERT_EQ(err, eNoError);
        
        // 验证转换精度
        ASSERT_NEAR(pos2.x(), pos2_back.x(), POS_EPS);
        ASSERT_NEAR(pos2.y(), pos2_back.y(), POS_EPS);
        ASSERT_NEAR(pos2.z(), pos2_back.z(), POS_EPS);
        ASSERT_NEAR(vel2.x(), vel2_back.x(), VEL_EPS);
        ASSERT_NEAR(vel2.y(), vel2_back.y(), VEL_EPS);
        ASSERT_NEAR(vel2.z(), vel2_back.z(), VEL_EPS);
    }
    
    return 0;
}

/// @brief 测试改进春分点轨道根数与直角坐标之间的转换
int testModEquinElemCartConversion()
{
    printf("测试: 改进春分点轨道根数 ↔ 直角坐标\n");
    
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
        ASSERT_EQ(err, eNoError);
        
        // 验证转换精度
        ASSERT_NEAR(mee1.p(), mee1_back.p(), EPS);
        ASSERT_NEAR(mee1.f(), mee1_back.f(), EPS);
        ASSERT_NEAR(mee1.g(), mee1_back.g(), EPS);
        ASSERT_NEAR(mee1.h(), mee1_back.h(), EPS);
        ASSERT_NEAR(mee1.k(), mee1_back.k(), EPS);
        
        // L需要考虑模2π的情况
        double L_diff = fabs(mee1.L() - mee1_back.L());
        if (L_diff > kPI) L_diff = 2 * kPI - L_diff;
        ASSERT_NEAR(L_diff, 0.0, ANGLE_EPS);
    }
    
    return 0;
}

/// @brief 测试修正轨道根数与直角坐标之间的转换
int testModOrbElemCartConversion()
{
    printf("测试: 修正轨道根数 ↔ 直角坐标\n");
    
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
        ASSERT_EQ(err, eNoError);
        
        // 验证转换精度
        ASSERT_NEAR(moe1.rp(), moe1_back.rp(), EPS);
        ASSERT_NEAR(moe1.e(), moe1_back.e(), EPS);
        ASSERT_NEAR(moe1.i(), moe1_back.i(), ANGLE_EPS);
        
        // 角度参数需要考虑模2π的情况
        double raan_diff = fabs(moe1.raan() - moe1_back.raan());
        if (raan_diff > kPI) raan_diff = 2 * kPI - raan_diff;
        ASSERT_NEAR(raan_diff, 0.0, ANGLE_EPS);
        
        double argper_diff = fabs(moe1.argper() - moe1_back.argper());
        if (argper_diff > kPI) argper_diff = 2 * kPI - argper_diff;
        ASSERT_NEAR(argper_diff, 0.0, ANGLE_EPS);
        
        double trueA_diff = fabs(moe1.trueA() - moe1_back.trueA());
        if (trueA_diff > kPI) trueA_diff = 2 * kPI - trueA_diff;
        ASSERT_NEAR(trueA_diff, 0.0, ANGLE_EPS);
    }
    
    return 0;
}

/// @brief 测试春分点根数与直角坐标之间的转换
int testEquinElemCartConversion()
{
    printf("测试: 春分点根数 ↔ 直角坐标\n");
    
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
        ASSERT_NEAR(ee1.a(), ee1_back.a(), EPS);
        ASSERT_NEAR(ee1.h(), ee1_back.h(), EPS);
        ASSERT_NEAR(ee1.k(), ee1_back.k(), EPS);
        ASSERT_NEAR(ee1.p(), ee1_back.p(), EPS);
        ASSERT_NEAR(ee1.q(), ee1_back.q(), EPS);
        
        // lambda需要考虑模2π的情况
        double lambda_diff = fabs(ee1.lambda() - ee1_back.lambda());
        if (lambda_diff > kPI) lambda_diff = 2 * kPI - lambda_diff;
        ASSERT_NEAR(lambda_diff, 0.0, ANGLE_EPS);
    }
    
    return 0;
}

/// @brief 测试各种轨道根数之间的相互转换
int testElementInterconversion()
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
        ASSERT_EQ(err, eNoError);
        
        OrbElem coe_back;
        err = aModEquinElemToOrbElem(mee, coe_back);
        ASSERT_EQ(err, eNoError);
        
        // 验证转换精度
        ASSERT_NEAR(coe_original.a(), coe_back.a(), EPS);
        ASSERT_NEAR(coe_original.e(), coe_back.e(), EPS);
        ASSERT_NEAR(coe_original.i(), coe_back.i(), ANGLE_EPS);
    }
    
    // 经典轨道根数 -> 春分点根数 -> 经典轨道根数
    {
        EquinElem ee;
        coe2ee(coe_original.data(), ee.data());
        
        OrbElem coe_back;
        ee2coe(ee.data(), coe_back.data());
        
        // 验证转换精度
        ASSERT_NEAR(coe_original.a(), coe_back.a(), EPS);
        ASSERT_NEAR(coe_original.e(), coe_back.e(), EPS);
        ASSERT_NEAR(coe_original.i(), coe_back.i(), ANGLE_EPS);
    }
    
    // 经典轨道根数 -> 修正轨道根数 -> 经典轨道根数
    {
        ModOrbElem moe;
        coe2moe(coe_original.data(), moe.data());
        
        OrbElem coe_back;
        moe2coe(moe.data(), coe_back.data());
        
        // 验证转换精度
        ASSERT_NEAR(coe_original.a(), coe_back.a(), EPS);
        ASSERT_NEAR(coe_original.e(), coe_back.e(), EPS);
        ASSERT_NEAR(coe_original.i(), coe_back.i(), ANGLE_EPS);
    }
    
    return 0;
}

int main()
{
    printf("开始轨道根数转换往返测试...\n\n");
    
    int rc = 0;
    
    rc |= testOrbElemCartConversion();
    printf("\n");
    
    rc |= testModEquinElemCartConversion();
    printf("\n");
    
    rc |= testModOrbElemCartConversion();
    printf("\n");
    
    rc |= testEquinElemCartConversion();
    printf("\n");
    
    rc |= testElementInterconversion();
    printf("\n");
    
    if (rc == 0) {
        printf("所有轨道根数转换测试完成，全部通过！\n");
    } else {
        printf("轨道根数转换测试完成，但有失败项！\n");
    }
    
    return rc;
}
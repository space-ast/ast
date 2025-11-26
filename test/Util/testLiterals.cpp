/// @file      testLiterals.cpp
/// @brief     测试自定义字面量功能
/// @details   测试所有单位转换字面量的正确性
/// @author    jinke18
/// @date      21.11.2025
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
 
#include "AstUtil/Literals.hpp"
#include "AstTest/AstTestMacro.h"
#include "AstUtil/Constants.h" // 包含常量定义用于测试

AST_USING_NAMESPACE
using namespace ast::literals;

// 测试角度单位字面量
TEST(Literals, AngleLiterals)
{
    // 测试度转弧度
    EXPECT_NEAR(1.0_deg, kDegToRad, 1e-15);
    EXPECT_NEAR(2.0_deg, 2.0 * kDegToRad, 1e-15);
    EXPECT_NEAR(90.0_deg, kPI / 2.0, 1e-15);
    EXPECT_NEAR(180.0_deg, kPI, 1e-15);
    EXPECT_NEAR(360.0_deg, kTwoPI, 1e-15);
    
    // 测试弧度单位（直接返回值）
    EXPECT_NEAR(1.0_rad, 1.0, 1e-15);
    EXPECT_NEAR(2.0_rad, 2.0, 1e-15);
    
    // 测试整数参数
    EXPECT_NEAR(45_deg, kPI / 4.0, 1e-15);
    EXPECT_NEAR(3_rad, 3.0, 1e-15);
    
}

// 测试长度单位字面量
TEST(Literals, LengthLiterals)
{
    
    // 测试千米转米
    EXPECT_NEAR(1.0_km, 1000.0, 1e-15);
    EXPECT_NEAR(2.5_km, 2500.0, 1e-15);
    
    // 测试米单位（直接返回值）
    EXPECT_NEAR(1.0_m, 1.0, 1e-15);
    EXPECT_NEAR(100.0_m, 100.0, 1e-15);
    
    // 测试厘米转米
    EXPECT_NEAR(1.0_cm, 0.01, 1e-15);
    EXPECT_NEAR(50.0_cm, 0.5, 1e-15);
    
    // 测试毫米转米
    EXPECT_NEAR(1.0_mm, 0.001, 1e-15);
    EXPECT_NEAR(1000.0_mm, 1.0, 1e-15);
    
    // 测试微米转米
    EXPECT_NEAR(1.0_um, 1e-6, 1e-15);
    EXPECT_NEAR(1000000.0_um, 1.0, 1e-15);
    
    // 测试纳米转米
    EXPECT_NEAR(1.0_nm, 1e-9, 1e-15);
    EXPECT_NEAR(1000000000.0_nm, 1.0, 1e-15);
    
    // 测试整数参数
    EXPECT_NEAR(5_km, 5000.0, 1e-15);
    EXPECT_NEAR(100_cm, 1.0, 1e-15);
    
    // 测试单位换算的一致性
    EXPECT_NEAR(1_km, 1000_m, 1e-15);
    EXPECT_NEAR(1_m, 100_cm, 1e-15);
    EXPECT_NEAR(1_cm, 10_mm, 1e-15);
    
}

// 测试时间单位字面量
TEST(Literals, TimeLiterals)
{
    // 测试秒单位（直接返回值）
    EXPECT_NEAR(1.0_s, 1.0, 1e-15);
    EXPECT_NEAR(10.0_s, 10.0, 1e-15);
    
    // 测试分钟转秒
    EXPECT_NEAR(1.0_min, 60.0, 1e-15);
    EXPECT_NEAR(2.5_min, 150.0, 1e-15);
    
    // 测试小时转秒
    EXPECT_NEAR(1.0_h, 3600.0, 1e-15);
    EXPECT_NEAR(0.5_h, 1800.0, 1e-15);
    
    // 测试天转秒
    EXPECT_NEAR(1.0_day, 86400.0, 1e-15);
    EXPECT_NEAR(0.5_day, 43200.0, 1e-15);
    
    // 测试毫秒转秒
    EXPECT_NEAR(1.0_ms, 0.001, 1e-15);
    EXPECT_NEAR(1000.0_ms, 1.0, 1e-15);
    
    // 测试微秒转秒
    EXPECT_NEAR(1.0_us, 1e-6, 1e-15);
    EXPECT_NEAR(1000000.0_us, 1.0, 1e-15);
    
    // 测试纳秒转秒
    EXPECT_NEAR(1.0_ns, 1e-9, 1e-15);
    EXPECT_NEAR(1000000000.0_ns, 1.0, 1e-15);
    
    // 测试皮秒转秒
    EXPECT_NEAR(1.0_ps, 1e-12, 1e-15);
    EXPECT_NEAR(1000000000000.0_ps, 1.0, 1e-15);
    
    // 测试整数参数
    EXPECT_NEAR(2_min, 120.0, 1e-15);
    EXPECT_NEAR(3_h, 10800.0, 1e-15);
    
    // 测试单位换算的一致性
    EXPECT_NEAR(1_h, 60_min, 1e-15);
    EXPECT_NEAR(1_min, 60_s, 1e-15);
    EXPECT_NEAR(1_s, 1000_ms, 1e-15);
    
}

// 测试天文单位字面量
TEST(Literals, AstronomicalLiterals)
{
    // 测试天文单位转米
    EXPECT_NEAR(1.0_au, kAU, 1e-10);
    EXPECT_NEAR(2.0_au, 2.0 * kAU, 1e-10);
    
    // 测试整数参数
    EXPECT_NEAR(3_au, 3.0 * kAU, 1e-10);
    
}

// 测试各种单位组合的实际应用场景
TEST(Literals, PracticalScenarios)
{
    // 场景1：计算地球周长（约40075公里）
    double earthCircumference = 2 * kPI * kEarthRadius;
    double circumferenceInKm = earthCircumference / 1000.0;
    EXPECT_NEAR(circumferenceInKm, 40075.0_km / 1000.0, 1.0); // 允许1公里误差
    
    // 场景2：计算光在一天内传播的距离
    double lightSpeedInKmPerSec = kLightSpeed / 1000.0;
    double lightDistanceInOneDay = lightSpeedInKmPerSec * 86400.0;
    EXPECT_NEAR(lightDistanceInOneDay, (kLightSpeed * 1.0_day) / 1000.0, 1.0);
    
    // 场景3：测试角度计算
    double angleSum = 30_deg + 60_deg;
    EXPECT_NEAR(angleSum, 90_deg, 1e-15);
    
}


GTEST_MAIN()
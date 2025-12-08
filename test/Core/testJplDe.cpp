/// @file      testJplDe.cpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      5.12.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "AstTest/AstTestMacro.h"
#include "AstCore/JplDe.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/RunTime.hpp"
#include "AstCore/Vector.hpp"
#include "AstUtil/FileSystem.hpp"

AST_USING_NAMESPACE

TEST(JplDe, Load)
{
    JplDe jplDe;
    std::vector<std::string> files{
        "SolarSystem/plneph.430",
        "Test/leDE1941.405",
        "Test/leDE18002100.424",
        "Test/leDE1900.421"
    };
    for (auto file : files) {
        fs::path filepath = fs::path(aDataDirGet()) / file;
        EXPECT_TRUE(jplDe.open(filepath.c_str()) == 0);
    }
}


TEST(JplDe, PosVel)
{
    JplDe jplDe;
    err_t err = jplDe.openDefault();
    EXPECT_FALSE(err);
    auto time = TimePoint::FromUTC({2025, 12, 6, 4, 0, 0});
    Vector3d pos, vel;
    err = jplDe.getPosVelICRF(time, JplDe::eEarth, JplDe::eSSBarycenter, pos, vel);
    Vector3d expect_pos { 40340688655.266181945800781,    129187016414.9415588378906250,    56021612758.1787185668945313 };
    Vector3d expect_vel { -29081.3914269203341973,    7466.4619473275215569,    3236.8305386958791132 };
    EXPECT_FALSE(err);
    for (int i = 0; i < 3; i++) {
        EXPECT_NEAR(pos[i], expect_pos[i], 1e-1);
        EXPECT_NEAR(vel[i], expect_vel[i], 1e-4);
    }

}

TEST(JplDe, IsOpenAndClose)
{
    JplDe jplDe;
    // 初始状态应该是关闭的
    EXPECT_FALSE(jplDe.isOpen());
    
    // 打开默认文件后应该是打开的
    err_t err = jplDe.openDefault();
    EXPECT_FALSE(err);
    EXPECT_TRUE(jplDe.isOpen());
    
    // 关闭后应该是关闭的
    jplDe.close();
    EXPECT_FALSE(jplDe.isOpen());
}

TEST(JplDe, GetPosICRF)
{
    JplDe jplDe;
    err_t err = jplDe.openDefault();
    EXPECT_FALSE(err);
    
    auto time = TimePoint::FromUTC({2025, 12, 6, 4, 0, 0});
    Vector3d pos;
    
    // 测试只获取位置的函数
    err = jplDe.getPosICRF(time, JplDe::eEarth, JplDe::eSSBarycenter, pos);
    EXPECT_FALSE(err);
    
    // 验证位置是否合理（可以使用与PosVel测试相同的预期值）
    Vector3d expect_pos { 40340688655.266181945800781,    129187016414.9415588378906250,    56021612758.1787185668945313 };
    for (int i = 0; i < 3; i++) {
        EXPECT_NEAR(pos[i], expect_pos[i], 1e-1);
    }
}



TEST(JplDe, GetNutation)
{
    JplDe jplDe;
    err_t err = jplDe.openDefault();
    EXPECT_FALSE(err);
    
    auto time = TimePoint::FromUTC({2025, 12, 6, 4, 0, 0});
    double nutLong, nutObl;
    
    // 测试获取章动角的函数
    err = jplDe.getNutation(time, nutLong, nutObl);
    EXPECT_FALSE(err);
    
    // 章动角应该很小（弧度制）
    EXPECT_TRUE(std::abs(nutLong) < 1e-3);
    EXPECT_TRUE(std::abs(nutObl) < 1e-3);
}

TEST(JplDe, GetLibration)
{
    JplDe jplDe;
    err_t err = jplDe.openDefault();
    EXPECT_FALSE(err);
    
    auto time = TimePoint::FromUTC({2025, 12, 6, 4, 0, 0});
    Vector3d ang, angRate;
    
    // 测试获取月面天平动的函数
    err = jplDe.getLibration(time, ang, angRate);
    EXPECT_FALSE(err);
    
    // 天平动角应该合理
    // EXPECT_TRUE(std::abs(ang[0]) < 1e-2);   // 进动角应该很小
    // EXPECT_TRUE(std::abs(ang[1]) < 1e-2);   // 章动角应该很小
    // EXPECT_TRUE(std::abs(angRate[0]) < 1e-6); // 角速度应该很小
}

GTEST_MAIN()
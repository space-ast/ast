/// @file      testJplDe.cpp
/// @brief     
/// @details   ~
/// @author    axel
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

#include "ast/AstTestMacro.h"
#include "ast/JplDe.hpp"
#include "ast/TimePoint.hpp"
#include "ast/RunTime.hpp"
#include "ast/Vector.hpp"
#include "ast/FileSystem.hpp"
#include "ast/Literals.hpp"
#include "ast/Euler.hpp"
#include "ast/RunTimeJplDe.hpp"

AST_USING_NAMESPACE

TEST(JplDe, Load)
{
    aInitialize();
    JplDe jplDe;
    std::vector<std::string> files = aTestGetConfigStringVector("JPLDE_FILES");
    for (auto file : files) {
        EXPECT_TRUE(jplDe.open(file.c_str()) == 0);
    }
}


TEST(JplDe, PosVel)
{
    {
        JplDe jplDe;
        errc_t err = jplDe.openDefault();
        EXPECT_FALSE(err);
        int denum = jplDe.getEphemVersion();
        EXPECT_EQ(denum, 430);
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
    {
        JplDe jplDe;
        errc_t err = jplDe.openDefault();
        EXPECT_FALSE(err);
        int denum = jplDe.getEphemVersion();
        EXPECT_EQ(denum, 430);
        auto time = TimePoint::FromUTC({2026, 1, 1, 0, 0, 0});
        Vector3d pos, vel;
        err = jplDe.getPosVelICRF(time, JplDe::eSun, JplDe::eEarth, pos, vel);
        Vector3d expect_pos { 26074199.566242_km,    -132831361.091847_km,    -57579750.394130_km };
        Vector3d expect_vel { 29.78885492_km/s,    4.95152585_km/s,    2.14630130_km/s };
        EXPECT_FALSE(err);
        for (int i = 0; i < 3; i++) {
            EXPECT_NEAR(pos[i], expect_pos[i], 1e-1_m);
            EXPECT_NEAR(vel[i], expect_vel[i], 1e-4_m/s);
        }
    }
}

TEST(JplDe, IsOpenAndClose)
{
    JplDe jplDe;
    // 初始状态应该是关闭的
    EXPECT_FALSE(jplDe.isOpen());
    
    // 打开默认文件后应该是打开的
    errc_t err = jplDe.openDefault();
    EXPECT_FALSE(err);
    EXPECT_TRUE(jplDe.isOpen());
    
    // 关闭后应该是关闭的
    jplDe.close();
    EXPECT_FALSE(jplDe.isOpen());
}

TEST(JplDe, GetPosICRF)
{
    JplDe jplDe;
    errc_t err = jplDe.openDefault();
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
    errc_t err = jplDe.openDefault();
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
    errc_t err = jplDe.openDefault();
    EXPECT_FALSE(err);
    
    auto time = TimePoint::FromUTC({2025, 12, 6, 4, 0, 0});
    Euler ang, angRate;
    
    // 测试获取月面天平动的函数
    err = jplDe.getLibration(time, ang, angRate);
    EXPECT_FALSE(err);
    
    // 天平动角应该合理
    // EXPECT_TRUE(std::abs(ang[0]) < 1e-2);   // 进动角应该很小
    // EXPECT_TRUE(std::abs(ang[1]) < 1e-2);   // 章动角应该很小
    // EXPECT_TRUE(std::abs(angRate[0]) < 1e-6); // 角速度应该很小
}

// 常量查询相关测试（按名 / 按索引 / 懒加载 / 错误处理）——
// 数据依赖：默认 DE 星历文件 plneph.430（DE430，ncon=229，小端）。
TEST(JplDe, GetNumConstants)
{
    JplDe jplDe;
    errc_t err = jplDe.openDefault();
    EXPECT_FALSE(err);

    // DE430 头部 ncon 字段 = 229
    uint32_t n = jplDe.getNumConstants();
    EXPECT_EQ(n, (uint32_t)229);
}

TEST(JplDe, GetConstants)
{
    JplDe jplDe;
    errc_t err = jplDe.openDefault();
    EXPECT_FALSE(err);
    auto list = jplDe.getConstants();
    int i = 0;
    for (const auto& item : list) {
        i++;
        printf("%3d: %6s = %.16g\n", i, item.first.c_str(), item.second);
    }
}

TEST(JplDe, GetConstant)
{
    JplDe jplDe;
    errc_t err = jplDe.openDefault();
    EXPECT_FALSE(err);
    EXPECT_DOUBLE_EQ(jplDe.getConstant("AU") * 1000, jplDe.getAU());
    EXPECT_DOUBLE_EQ(jplDe.getConstant("EMRAT"), jplDe.getEarthMoonMassRatio());
       
}

TEST(JplDe, GetConstantByName)
{
    JplDe jplDe;
    errc_t err = jplDe.openDefault();
    EXPECT_FALSE(err);

    // 按名查询已知常量（DE430 表内）
    double v = 0;
    err = jplDe.getConstant("AU", v);
    EXPECT_EQ(err, eNoError);
    EXPECT_NEAR(v, 149597870.7, 1e-3);        // 天文单位，km

    err = jplDe.getConstant("EMRAT", v);
    EXPECT_EQ(err, eNoError);
    EXPECT_NEAR(v, 81.30056907419062, 1e-12); // 地球/月球质量比

    err = jplDe.getConstant("GM1", v);
    EXPECT_EQ(err, eNoError);
    EXPECT_NEAR(v, 4.91248045036476e-11, 1e-20); // 太阳 GM，km^3/s^2

    err = jplDe.getConstant("DENUM", v);
    EXPECT_EQ(err, eNoError);
    EXPECT_NEAR(v, 430.0, 1e-9);              // DE 版本号
}

TEST(JplDe, GetConstantByIndex)
{
    JplDe jplDe;
    errc_t err = jplDe.openDefault();
    EXPECT_FALSE(err);

    // 索引 0 应为 DENUM（DE 版本号 430）
    double v = 0; std::string name;
    err = jplDe.getConstant(0, v, &name);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(name, "DENUM");
    EXPECT_NEAR(v, 430.0, 1e-9);

    // 索引 9 应为 AU，且与按名查询一致
    double vAu = 0, vIdx = 0; std::string nameAu;
    err = jplDe.getConstant(9, vIdx, &nameAu);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(nameAu, "AU");
    err = jplDe.getConstant("AU", vAu);
    EXPECT_EQ(err, eNoError);
    EXPECT_DOUBLE_EQ(vAu, vIdx);   // 同一数据源，应严格相等

    // 不传名称（name=nullptr）也应正常
    err = jplDe.getConstant(9, vIdx);
    EXPECT_EQ(err, eNoError);
    EXPECT_DOUBLE_EQ(vAu, vIdx);
}

TEST(JplDe, GetConstantErrors)
{
    // 未打开文件：应返回错误
    {
        JplDe jplDe;
        double v;
        EXPECT_TRUE(jplDe.getConstant(0, v) != eNoError);
        EXPECT_TRUE(jplDe.getConstant("AU", v) != eNoError);
    }
    // 打开后：越界索引 / 未知名称返回错误
    {
        JplDe jplDe;
        ASSERT_FALSE(jplDe.openDefault());
        double v;
        uint32_t n = jplDe.getNumConstants();
        // 越界（等于总数、负数）
        EXPECT_TRUE(jplDe.getConstant((int)n, v) != eNoError);
        EXPECT_TRUE(jplDe.getConstant(-1, v) != eNoError);
        // 未知名
        EXPECT_TRUE(jplDe.getConstant("NOT_A_CONSTANT", v) != eNoError);
        // 关闭后再次查询：应返回错误
        jplDe.close();
        EXPECT_TRUE(jplDe.getConstant("AU", v) != eNoError);
    }
}

TEST(JplDe, GetConstantLazyLoad)
{
    JplDe jplDe;
    ASSERT_FALSE(jplDe.openDefault());

    uint32_t n = jplDe.getNumConstants();
    // 第 0 个与最后一个索引都可查询（懒加载首次触发）
    double v0, vLast; std::string nm0, nmLast;
    EXPECT_EQ(jplDe.getConstant(0, v0, &nm0), eNoError);
    EXPECT_EQ(jplDe.getConstant((int)n - 1, vLast, &nmLast), eNoError);
    EXPECT_FALSE(nm0.empty());
    EXPECT_FALSE(nmLast.empty());

    // 懒加载后，按名与按索引交叉校验：取末位常量的名字，再按名反查
    double vByName;
    EXPECT_EQ(jplDe.getConstant(nmLast, vByName), eNoError);
    EXPECT_DOUBLE_EQ(vByName, vLast);
}

TEST(JplDe, GetBodyGM)
{
    JplDe jplDe;
    errc_t err = jplDe.openDefault();
    EXPECT_FALSE(err);
    double gmSun1 = jplDe.getBodyGM(JplDe::eSun);
    double gmSun2;
    aJplDeGetBodyGM(JplDe::eSun, gmSun2);
    printf("gmSun1 = %.16g\n", gmSun1);
    printf("gmSun2 = %.16g\n", gmSun2);
    EXPECT_DOUBLE_EQ(gmSun1, gmSun2);
    EXPECT_NEAR(gmSun1, kSunGrav, kSunGrav * 1e-5);
}

GTEST_MAIN()
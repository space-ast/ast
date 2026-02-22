///
/// @file      testGravityField.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-14
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

#include "AstCore/GravityField.hpp"
#include "AstCore/RunTime.hpp"
#include "AstUtil/String.hpp"
#include "AstTest/Test.h"
#include <vector>


AST_USING_NAMESPACE

TEST(GravityField, loadWGS84)
{
    GravityField gf;
    err_t err = gf.load(aDataDirGet() + "/Test/CentralBodies/Earth/WGS84.grv");
    EXPECT_EQ(err, eNoError);

    EXPECT_EQ(gf.getModelName(), "WGS84");
    EXPECT_EQ(gf.getCentralBodyName(), "Earth");
    EXPECT_EQ(gf.getMaxDegree(), 70);
    EXPECT_EQ(gf.getMaxOrder(), 70);
    EXPECT_EQ(gf.getGM(), 3.986004418E+14);
    EXPECT_EQ(gf.getRefDistance(), 6378137.0);
    EXPECT_TRUE(gf.isNormalized());
    EXPECT_FALSE(gf.isIncludesPermTide());

    EXPECT_EQ(gf.getCnm(2, 0), -4.841668500000e-04);
    EXPECT_EQ(gf.getSnm(2, 0), 0.0);

    EXPECT_EQ(gf.getCnm(3, 0), 9.570639000000e-07);
    EXPECT_EQ(gf.getSnm(3, 0), 0.0);

    EXPECT_EQ(gf.getCnm(64, 51), 7.870636900000e-10);
    EXPECT_EQ(gf.getSnm(64, 51), 1.562802400000e-10);
}


TEST(GravityField, loadJGM3)
{
    std::vector<std::string> files;
    files.push_back(aDataDirGet() + "/Test/GMAT/gravity/earth/JGM3.cof");
    files.push_back(aDataDirGet() + "/Test/CentralBodies/Earth/JGM3.grv");
    
    for(auto &file : files){
        GravityField gf;
        err_t err = gf.load(file);
        EXPECT_EQ(err, eNoError);

        EXPECT_EQ(gf.getMaxDegree(), 70);
        EXPECT_EQ(gf.getMaxOrder(), 70);
        EXPECT_EQ(gf.getGM(), 3.98600441500e+14);
        EXPECT_EQ(gf.getRefDistance(), 6378136.300000);
        EXPECT_TRUE(gf.isNormalized());
        EXPECT_FALSE(gf.isIncludesPermTide());

        EXPECT_EQ(gf.getCnm(2, 0), -4.8416537488647e-04);
        EXPECT_EQ(gf.getSnm(2, 0), 0.0);


        EXPECT_EQ(gf.getCnm(56, 51), 2.0872465219905e-09);
        EXPECT_EQ(gf.getSnm(56, 51), -2.2104224180283e-10);
    }

    // load gfc
    {
        std::string file = aDataDirGet() + "/Test/satkit/JGM3.gfc";
        GravityField gf;
        err_t err = gf.load(file);
        EXPECT_EQ(err, eNoError);

        EXPECT_EQ(gf.getMaxDegree(), 70);
        EXPECT_EQ(gf.getMaxOrder(), 70);
        EXPECT_EQ(gf.getGM(), 3.98600441500e+14);
        EXPECT_EQ(gf.getRefDistance(), 6378136.300000);
        EXPECT_TRUE(gf.isNormalized());
        EXPECT_FALSE(gf.isIncludesPermTide());

        EXPECT_EQ(gf.getCnm(2, 0), -0.484169548456e-03);
        EXPECT_EQ(gf.getSnm(2, 0), 0.0);


        EXPECT_EQ(gf.getCnm(56, 51), 0.208724652199e-08);
        EXPECT_EQ(gf.getSnm(56, 51), -0.221042241803e-09);
    }
}


TEST(GravityField, load_gfc)
{
    std::string file = aDataDirGet() + "/Test/satkit/JGM2.gfc";
    GravityField gf;
    err_t err = gf.load(file);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(gf.getMaxDegree(), 70);
    EXPECT_EQ(gf.getMaxOrder(), 70);
    EXPECT_EQ(gf.getGM(), 0.3986004415E+15);
    EXPECT_EQ(gf.getRefDistance(), 0.6378136300E+07);
    EXPECT_TRUE(gf.isNormalized());
    EXPECT_EQ(gf.getCnm(3, 0), 0.957122390e-06);
    EXPECT_EQ(gf.getSnm(3, 0), 0.0);
    EXPECT_EQ(gf.getCnm(56, 51), 0.218529600e-08);
    EXPECT_EQ(gf.getSnm(56, 51), -.487515800e-09);

}


TEST(GravityField, normalize)
{
    GravityField gf_WGS84, gf_WGS84_old;
    err_t err;
    err = gf_WGS84.load(aDataDirGet() + "/Test/CentralBodies/Earth/WGS84.grv");
    EXPECT_EQ(err, eNoError);
    err = gf_WGS84_old.load(aDataDirGet() + "/Test/CentralBodies/Earth/WGS84_old.grv");
    EXPECT_EQ(err, eNoError);

    GravityField gf_WGS84_old_normalized = gf_WGS84_old.normalized();
    for(int n = 0; n <= gf_WGS84_old_normalized.getMaxDegree(); n++){
        for(int m = 0; m <= n; m++){
            // printf("n = %d, m = %d\n", n, m);
            // printf("WGS84: %g, WGS84_old_normalized: %g\n", gf_WGS84.getCnm(n, m), gf_WGS84_old_normalized.getCnm(n, m));
            // printf("WGS84: %g, WGS84_old_normalized: %g\n", gf_WGS84.getSnm(n, m), gf_WGS84_old_normalized.getSnm(n, m));
            if(n==12 && m ==7) continue;
            EXPECT_NEAR(gf_WGS84_old_normalized.getCnm(n, m), gf_WGS84.getCnm(n, m), fabs(gf_WGS84.getCnm(n, m)) * 5e-6);
            EXPECT_NEAR(gf_WGS84_old_normalized.getSnm(n, m), gf_WGS84.getSnm(n, m), fabs(gf_WGS84.getSnm(n, m)) * 5e-6);
        }
    }

    GravityField gf_WGS84_unnormalized = gf_WGS84.unnormalized();
    for(int n = 0; n <= gf_WGS84_old.getMaxDegree(); n++){
        for(int m = 0; m <= n; m++){
            // printf("n = %d, m = %d\n", n, m);
            // printf("WGS84_old: %g, WGS84_unnormalized: %g\n", gf_WGS84_old.getCnm(n, m), gf_WGS84_unnormalized.getCnm(n, m));
            // printf("WGS84_old: %g, WGS84_unnormalized: %g\n", gf_WGS84_old.getSnm(n, m), gf_WGS84_unnormalized.getSnm(n, m));
            if(n==12 && m ==7) continue;
            EXPECT_NEAR(gf_WGS84_unnormalized.getCnm(n, m), gf_WGS84_old.getCnm(n, m), fabs(gf_WGS84_unnormalized.getCnm(n, m)) * 5e-6);
            EXPECT_NEAR(gf_WGS84_unnormalized.getSnm(n, m), gf_WGS84_old.getSnm(n, m), fabs(gf_WGS84_unnormalized.getSnm(n, m)) * 5e-6);
        }
    }

}

GTEST_MAIN();

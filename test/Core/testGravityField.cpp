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
}

GTEST_MAIN();

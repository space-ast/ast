///
/// @file      testMagnetosphere.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-15
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


#include "AstWeather/Magnetosphere.hpp"
#include "AstTest/Test.h"

AST_USING_NAMESPACE

TEST(Magnetosphere, igrf13)
{
    {
        double date = 2010.5260273972603;
        int itype = 1;
        double alt = 0.0;
        double lat = 65;
        double elong = 85;
        double colat = 90 - lat;
        double x = 0.0;
        double y = 0.0;
        double z = 0.0;
        double f = 0.0;
        int ret = igrf13syn(0, date, 1, alt, colat, elong, x, y, z, f);

        printf("x = %f, y = %f, z = %f, f = %f\n", x, y, z, f);
        EXPECT_NEAR(x, 9295.415328, 0.001);
        EXPECT_NEAR(y, 2559.788960, 0.001);
        EXPECT_NEAR(z, 59670.378618, 0.001);
        EXPECT_NEAR(f, 60444.283022, 0.001);
    }

    {
        double date = 2010;
        int itype = 1;
        double alt = 0.0;
        double lat = 65;
        double elong = 85;
        double colat = 90 - lat;
        double x = 0.0;
        double y = 0.0;
        double z = 0.0;
        double f = 0.0;
        int ret = igrf13syn(0, date, 1, alt, colat, elong, x, y, z, f);

        printf("x = %f, y = %f, z = %f, f = %f\n", x, y, z, f);
        EXPECT_NEAR(x, 9315.267451, 0.001);
        EXPECT_NEAR(y, 2568.004564, 0.001);
        EXPECT_NEAR(z, 59659.366911, 0.001);
        EXPECT_NEAR(f, 60436.817548, 0.001);
    }
}


GTEST_MAIN()


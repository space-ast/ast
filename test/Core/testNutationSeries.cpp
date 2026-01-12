///
/// @file      testNutationSeries.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-11
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

#include "AstCore/NutationSeries.hpp"
#include "AstCore/RunTime.hpp"
#include "AstTest/Test.h"


AST_USING_NAMESPACE

TEST(NutationSeries, loadSTK)
{
    {    
        NutationSeries series;
        err_t ret = series.loadSTK(aDataDirGet() + "/Test/ICRF/IAU2000A_S.dat");
        EXPECT_EQ(ret, eNoError);
    }
    {
        NutationSeries series;
        err_t ret = series.loadSTK(aDataDirGet() + "/Test/ICRF/IAU2000A_X.dat");
        EXPECT_EQ(ret, eNoError);
    }
    {
        NutationSeries series;
        err_t ret = series.loadSTK(aDataDirGet() + "/Test/ICRF/IAU2000A_Y.dat");
        EXPECT_EQ(ret, eNoError);
    }
    {    
        NutationSeries series;
        err_t ret = series.loadSTK(aDataDirGet() + "/Test/ICRF/IAU2006_S.dat");
        EXPECT_EQ(ret, eNoError);
    }
    {
        NutationSeries series;
        err_t ret = series.loadSTK(aDataDirGet() + "/Test/ICRF/IAU2006_X.dat");
        EXPECT_EQ(ret, eNoError);
    }
    {
        NutationSeries series;
        err_t ret = series.loadSTK(aDataDirGet() + "/Test/ICRF/IAU2006_Y.dat");
        EXPECT_EQ(ret, eNoError);
    }
}


TEST(NutationSeries, loadIERS)
{
    {
        NutationSeries series;
        err_t ret = series.loadIERS(aDataDirGet() + "/IERS-conventions/2010/tab5.2a.txt");
        EXPECT_EQ(ret, eNoError);
    }
    {
        NutationSeries series;
        err_t ret = series.loadIERS(aDataDirGet() + "/IERS-conventions/2010/tab5.2b.txt");
        EXPECT_EQ(ret, eNoError);
    }
    {
        NutationSeries series;
        err_t ret = series.loadIERS(aDataDirGet() + "/IERS-conventions/2010/tab5.2d.txt");
        EXPECT_EQ(ret, eNoError);
    }

}


TEST(NutationSeries, eval)
{

}

GTEST_MAIN();


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
#include "AstCore/TimePoint.hpp"
#include "AstCore/FundamentalArguments.hpp"
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


TEST(NutationSeries, load)
{
    {
        NutationSeries series;
        err_t ret = series.load(aDataDirGet() + "/IERS-conventions/2010/tab5.2a.txt");
        EXPECT_EQ(ret, eNoError);
    }
    {
        NutationSeries series;
        err_t ret = series.load(aDataDirGet() + "/IERS-conventions/2010/tab5.2b.txt");
        EXPECT_EQ(ret, eNoError);
    }
    {
        NutationSeries series;
        err_t ret = series.load(aDataDirGet() + "/IERS-conventions/2010/tab5.2d.txt");
        EXPECT_EQ(ret, eNoError);
    }

    {
        NutationSeries series;
        err_t ret = series.load(aDataDirGet() + "/Test/ICRF/IAU2000A_S.dat");
        EXPECT_EQ(ret, eNoError);
    }
    {
        NutationSeries series;
        err_t ret = series.load(aDataDirGet() + "/Test/ICRF/IAU2000A_X.dat");
        EXPECT_EQ(ret, eNoError);
    }
    {
        NutationSeries series;
        err_t ret = series.load(aDataDirGet() + "/Test/ICRF/IAU2000A_Y.dat");
        EXPECT_EQ(ret, eNoError);
    }
    {
        NutationSeries series;
        err_t ret = series.load(aDataDirGet() + "/Test/ICRF/IAU2006_S.dat");
        EXPECT_EQ(ret, eNoError);
    }
    {
        NutationSeries series;
        err_t ret = series.load(aDataDirGet() + "/Test/ICRF/IAU2006_X.dat");
        EXPECT_EQ(ret, eNoError);
    }
    {
        NutationSeries series;
        err_t ret = series.load(aDataDirGet() + "/Test/ICRF/IAU2006_Y.dat");
        EXPECT_EQ(ret, eNoError);
    }
}


TEST(NutationSeries, evaluate)
{
    {
        NutationSeries XSeries;
        NutationSeries YSeries;
        err_t retX, retY;
        double x, y, x2, y2;
        
        retX = XSeries.load(aDataDirGet() + "/Test/ICRF/IAU2006_X.dat");
        retY = YSeries.load(aDataDirGet() + "/Test/ICRF/IAU2006_Y.dat");
        EXPECT_EQ(retX, eNoError);
        EXPECT_EQ(retY, eNoError);
        TimePoint tp = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
        double t = tp.julianCenturyFromJ2000TT();
        FundamentalArguments fundargs;
        aFundamentalArguments_IERS2010(t, fundargs);

        x = XSeries.eval(t, fundargs);
        y = YSeries.eval(t, fundargs);
        printf("t = %.20f, x = %.20f, y = %.20f\n", t, x, y);
        EXPECT_NEAR(x, 2.5367573298124e-3, 1e-16);
        EXPECT_NEAR(y, 3.1632222519258e-5, 1e-18);

        retX = XSeries.load(aDataDirGet() + "/IERS-conventions/2010/tab5.2a.txt");
        retY = YSeries.load(aDataDirGet() + "/IERS-conventions/2010/tab5.2b.txt");
        EXPECT_EQ(retX, eNoError);
        EXPECT_EQ(retY, eNoError);

        x2 = XSeries.eval(t, fundargs);
        y2 = YSeries.eval(t, fundargs);
        EXPECT_EQ(x2, x);
        EXPECT_EQ(y2, y);
    }
}


GTEST_MAIN();


///
/// @file      testScanf.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-20
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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

#include <stdio.h>
#include "ast/Test.h"



TEST(Scanf, Basic)
{
    
    const std::string str1 = 
R"(36
36
3.98600436000e+14
6378137.0

 2  0     -0.484164990000e-03   0.000000000e+00

)";
    const std::string str2 = 
R"(36
3.98600436000e+14
6378137.0

 2  0     -0.484164990000e-03   0.000000000e+00
)";

    const std::string str3 =
R"(
stk.v.4.0

Model			 MRO110C
CentralBody		 Mars
Degree			 110
Order			 110
Gm				 0.4282837564100000E+14
RefDistance		 3.39600000000000e+006
Normalized		 Yes
)";
    {
        double a, b, c, d;
        int status1 = sscanf(str1.c_str(), "%lf %lf %lf %lf", &a, &b, &c, &d);
        int status2 = sscanf(str2.c_str(), "%lf %lf %lf %lf", &a, &b, &c, &d);
        int status3 = sscanf(str3.c_str(), "%lf %lf %lf %lf", &a, &b, &c, &d);
        printf("status1: %d\n", status1);
        printf("status2: %d\n", status2);
        printf("status3: %d\n", status3);
        EXPECT_EQ(status3, 0);
    }
    {
        int a, b;
        double c, d;
        int status11 = sscanf(str1.c_str(), "%d %d %lf %lf", 
               &a, &b, &c, &d);
        int status12 = sscanf(str1.c_str(), "%d %lf %lf", 
               &a, &c, &d);
        int status21 = sscanf(str2.c_str(), "%d %d %lf %lf", 
               &a, &b, &c, &d);
        int status22 = sscanf(str2.c_str(), "%d %lf %lf", 
               &a, &c, &d);
        int status31 = sscanf(str3.c_str(), "%d %d %lf %lf", 
               &a, &b, &c, &d);
        int status32 = sscanf(str3.c_str(), "%d %lf %lf", 
               &a, &c, &d);
        printf("status11: %d\n", status11);
        printf("status12: %d\n", status12);
        printf("status21: %d\n", status21);
        printf("status22: %d\n", status22);
        printf("status31: %d\n", status31);
        printf("status32: %d\n", status32);
    }
}

GTEST_MAIN();


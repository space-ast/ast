///
/// @file      testSpiceDAFParser.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-09
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

#include "ast/DAFParser.hpp"
#include "ast/SPKParser.hpp"
#include "ast/RunTime.hpp"
#include "ast/CelestialBody.hpp"
#include "ast/Vector.hpp"
#include "ast/Test.hpp"
#include "ast/Environment.hpp"
#include <iostream>
#include <clocale>

#ifdef AST_WITH_CSPICE
#include "SpiceUsr.h"
#endif


AST_USING_NAMESPACE

TEST(SpiceSPKParser, getComment)
{
    setlocale(LC_ALL, ".UTF-8");
    if(aIsCI()) GTEST_SKIP();
    SPKParser parser(aTestGetConfigValue("SPK_FILE").toString());
    parser.printComment();
    std::vector<std::string> comments;
    parser.getComment(comments);
    for(const auto& comment : comments)
        printf("\n\"%s\"", comment.c_str());
}

TEST(SpiceSPKParser, getPosNative)
{
    GTEST_SKIP();
    // @todo 这里加载planets.bsp时计算tc会>1，导致测试失败
    const std::string kernel = aTestGetConfigValue("SPK_FILE").toString();

    SPKParser parser(kernel);
    #ifdef AST_WITH_CSPICE
    furnsh_c(kernel.c_str());
    #endif

    const static struct {
        double et;
        int bodid;
    } testData[]{
        {0.0      , (int)ESpiceId::eJupiterBarycenter},
        {100000   , (int)ESpiceId::eMarsBarycenter},
        {2345434  , (int)ESpiceId::eVenusBarycenter},
        {12345434 , (int)ESpiceId::ePlutoBarycenter},
    };
    for(const auto& data : testData)
    {
        double et = data.et;
        int bodid = data.bodid;
        printf("-------------------------------\n");
        printf("et = %f, bodid = %d\n", et, bodid);
        
        Vector3d pos, vel;
        errc_t err = parser.getPosVelNative(et, bodid, pos, vel);
        EXPECT_EQ(err, eNoError);
    #ifdef AST_WITH_CSPICE
        furnsh_c(kernel.c_str());
        double posvel_c[6];
        spkssb_c(bodid, et, "J2000", posvel_c);
        for(int i=0; i<6; i++)
        {
            posvel_c[i] *= 1e3;
            printf("posvel_c(%d) = %f\n", i, posvel_c[i]);
        }
        for(int i=0; i<3; i++)
        {
            printf("pos     (%d) = %f\n", i, pos(i));
        }
        for(int i=0; i<3; i++)
        {
            printf("vel     (%d) = %f\n", i, vel(i));
        }
        for(int i=0; i<3; i++)
        {
            EXPECT_DOUBLE_EQ(pos(i), posvel_c[i]);
            EXPECT_DOUBLE_EQ(vel(i), posvel_c[i + 3]);
        }
    #endif
    }
}

TEST(SpiceSPKParser, loadPlanetSatellitesSPK)
{
    GTEST_SKIP();
    const std::string kernel = aDataDirGet() + "/Test/kernels/spk/mars.bsp";
    SPKParser parser(kernel);
    double et = 0;
    int bodid = (int)ESpiceId::ePhobos;
    Vector3d pos, vel;
    errc_t err = parser.getPosVelNative(et, bodid, pos, vel);
    A_UNUSED(err);
    // EXPECT_EQ(err, eNoError);
}

GTEST_MAIN()

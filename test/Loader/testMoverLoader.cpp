///
/// @file      testMoverLoader.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-14
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

#include "AstSim/Mover.hpp"
#include "AstSim/Aircraft.hpp"
#include "AstSim/Satellite.hpp"
#include "AstLoader/MoverLoader.hpp"
#include "AstCore/RunTime.hpp"
#include "AstTest/Test.h"
#include "AstUtil/StringView.hpp"
#include "AstMath/Vector.hpp"
#include <string>


AST_USING_NAMESPACE

template<typename MoverType>
void testLoadMover(StringView filepath)
{
    MoverType mover;
    errc_t ret = aLoadMover(filepath, mover);
    EXPECT_EQ(ret, eNoError);

    // ret = mover.generateEphemeris();
    // EXPECT_EQ(ret, eNoError);

    // 检查是否成功加载了运动模型
    auto motionProfile = mover.getMotionProfile();
    EXPECT_NE(motionProfile, nullptr);

    // 检查是否成功加载了星历
    // auto ephemeris = mover.getEphemeris();
    // EXPECT_NE(ephemeris, nullptr);

}

TEST(MoverLoaderTest, LoadAircraft)
{
    if(aIsGithubCI()) 
        GTEST_SKIP();

    aInitialize();
    std::vector<std::string> files = aTestGetConfigStringVector("STK_AIRCRAFT_FILES");
    for(auto& file: files)
    {
        printf("loading %s\n", file.c_str());

        testLoadMover<Aircraft>(file);

        printf("loaded %s\n", file.c_str());
    }
}



TEST(MoverLoaderTest, LoadSatellite)
{
    if(aIsGithubCI()) 
        GTEST_SKIP();

    aInitialize();
    std::vector<std::string> files = aTestGetConfigStringVector("STK_SATELLITE_FILES");
    
    for(auto& file: files)
    {
        printf("loading %s\n", file.c_str());
        testLoadMover<Satellite>(file);
        printf("loaded %s\n", file.c_str());
    }
}




GTEST_MAIN();


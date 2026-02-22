///
/// @file      testRotationData.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-20
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

#include "AstCore/RotationalData.hpp"
#include "AstCore/RunTime.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Literals.hpp"
#include "AstTest/Test.hpp"
#include <vector>
#include <string>

AST_USING_NAMESPACE
using namespace _AST literals;

TEST(RotationalData, load)
{
    RotationalData data;
    std::string datadir = aDataDirGet();
    std::vector<std::string> files;
    files.push_back(datadir + "/SolarSystem/Jupiter/JupiterAttitude2009.rot");
    files.push_back(datadir + "/SolarSystem/Mars/MarsAttitude2000.rot");
    files.push_back(datadir + "/SolarSystem/Moon/MoonAttitude2000.rot");
    files.push_back(datadir + "/SolarSystem/Neptune/NeptuneAttitude2000.rot");

    for(const auto &file : files){
        err_t rc = data.load(file);
        if(rc != eNoError){
            aError("failed to load file %s", file.c_str());
        }
        EXPECT_EQ(rc, eNoError);
    }
}


TEST(RotationalData, MarsAttitude2000)
{
    RotationalData data;
    std::string datadir = aDataDirGet();
    err_t rc = data.load(datadir + "/SolarSystem/Mars/MarsAttitude2000.rot");
    EXPECT_EQ(rc, eNoError);
    // test Fixed Frame
    {
        auto tp = TimePoint::FromUTC(2026, 2, 20, 0, 0, 0);
        Rotation rotation;
        data.getICRFToFixedTransform(tp, rotation);
        Vector3d posICRF{1000_km, 3000_km, 4000_km};
        Vector3d posFixed = rotation.transformVector(posICRF);
        printf("posICRF: %s\n", posICRF.toString().c_str());
        printf("posFixed: %s\n", posFixed.toString().c_str());
        Vector3d posFixedExpect {-2059074.881399545352906, -3990737.457134430315, 2415414.080652357825 };
        EXPECT_NEAR(posFixedExpect[0], posFixed[0], 1e-4);
        EXPECT_NEAR(posFixedExpect[1], posFixed[1], 1e-4);
        EXPECT_NEAR(posFixedExpect[2], posFixed[2], 1e-8);
    }
    // test Inertial Frame
    {
        auto tp = TimePoint::FromUTC(2026, 2, 20, 0, 0, 0);
        Rotation rotation;
        data.getICRFToInertialTransform(tp, rotation);
        Vector3d posICRF{1000_km, 3000_km, 4000_km};
        Vector3d posInertial = rotation.transformVector(posICRF);
        printf("posICRF: %s\n", posICRF.toString().c_str());
        printf("posInertial: %s\n", posInertial.toString().c_str());
        Vector3d posInertialExpect {2891490.981155287474394, 3434583.120965975013, 2417213.000766226287  };
        EXPECT_NEAR(posInertialExpect[0], posInertial[0], 1e-9);
        EXPECT_NEAR(posInertialExpect[1], posInertial[1], 1e-9);
        EXPECT_NEAR(posInertialExpect[2], posInertial[2], 1e-9);
    }
    // test TOD Frame
    {
        auto tp = TimePoint::FromUTC(2026, 2, 20, 0, 0, 0);
        Rotation rotation;
        data.getICRFToTODTransform(tp, rotation);
        Vector3d posICRF{1000_km, 3000_km, 4000_km};
        Vector3d posTOD = rotation.transformVector(posICRF);
        printf("posICRF: %s\n", posICRF.toString().c_str());
        printf("posTOD: %s\n", posTOD.toString().c_str());
        Vector3d posTODExpect {2890870.954200856853276 , 3436.370169982297284_km, 2415.414080652355551_km };
        EXPECT_NEAR(posTODExpect[0], posTOD[0], 1e-9);
        EXPECT_NEAR(posTODExpect[1], posTOD[1], 1e-9);
        EXPECT_NEAR(posTODExpect[2], posTOD[2], 1e-9);
    }
    // test MOD Frame
    {
        auto tp = TimePoint::FromUTC(2026, 2, 20, 0, 0, 0);
        Rotation rotation;
        data.getICRFToMODTransform(tp, rotation);
        Vector3d posICRF{1000_km, 3000_km, 4000_km};
        Vector3d posMOD = rotation.transformVector(posICRF);
        printf("posICRF: %s\n", posICRF.toString().c_str());
        printf("posMOD: %s\n", posMOD.toString().c_str());
        Vector3d posMODExpect {2890870.954200856853276 , 3436.370169982297284_km, 2415.414080652355551_km };
        EXPECT_NEAR(posMODExpect[0], posMOD[0], 1e-9);
        EXPECT_NEAR(posMODExpect[1], posMOD[1], 1e-9);
        EXPECT_NEAR(posMODExpect[2], posMOD[2], 1e-9);
    }
}

TEST(RotationalData, NeptuneAttitude2000)
{
    RotationalData data;
    std::string datadir = aDataDirGet();
    err_t rc = data.load(datadir + "/SolarSystem/Neptune/NeptuneAttitude2000.rot");
    EXPECT_EQ(rc, eNoError);
    // test Fixed Frame
    {
        auto tp = TimePoint::FromUTC(2026, 2, 22, 0, 0, 0);
        Rotation rotation;
        data.getICRFToFixedTransform(tp, rotation);
        Vector3d posICRF{5000_km, 4000_km, 3000_km};
        Vector3d posFixed = rotation.transformVector(posICRF);
        printf("posICRF: %s\n", posICRF.toString().c_str());
        printf("posFixed: %s\n", posFixed.toString().c_str());
        Vector3d posFixedExpect {-5882.733663869602424_km, 3702.339432079606468_km, 1298.509672526069835_km};
        EXPECT_NEAR(posFixedExpect[0], posFixed[0], 1e-4);
        EXPECT_NEAR(posFixedExpect[1], posFixed[1], 1e-4);
        EXPECT_NEAR(posFixedExpect[2], posFixed[2], 1e-8);
    }
    // test Inertial Frame
    {
        auto tp = TimePoint::FromUTC(2026, 2, 22, 0, 0, 0);
        Rotation rotation;
        data.getICRFToInertialTransform(tp, rotation);
        Vector3d posICRF{5000_km, 4000_km, 3000_km};
        Vector3d posInertial = rotation.transformVector(posICRF);
        printf("posICRF: %s\n", posICRF.toString().c_str());
        printf("posInertial: %s\n", posInertial.toString().c_str());
        Vector3d posInertialExpect {6318.488287596971531_km, 2902.846898545570184_km, 1284.595517314404333_km};
        EXPECT_NEAR(posInertialExpect[0], posInertial[0], 1e-9);
        EXPECT_NEAR(posInertialExpect[1], posInertial[1], 1e-9);
        EXPECT_NEAR(posInertialExpect[2], posInertial[2], 1e-9);
    }
    // test TOD Frame
    {
        auto tp = TimePoint::FromUTC(2026, 2, 22, 0, 0, 0);
        Rotation rotation;
        data.getICRFToTODTransform(tp, rotation);
        Vector3d posICRF{5000_km, 4000_km, 3000_km};
        Vector3d posTOD = rotation.transformVector(posICRF);
        printf("posICRF: %s\n", posICRF.toString().c_str());
        printf("posTOD: %s\n", posTOD.toString().c_str());
        Vector3d posTODExpect {6321.470523044497895_km, 2890.135473751319751_km, 1298.509672526065515_km };
        EXPECT_NEAR(posTODExpect[0], posTOD[0], 1e-9);
        EXPECT_NEAR(posTODExpect[1], posTOD[1], 1e-9);
        EXPECT_NEAR(posTODExpect[2], posTOD[2], 1e-9);
    }
    // test MOD Frame
    {
        auto tp = TimePoint::FromUTC(2026, 2, 22, 0, 0, 0);
        Rotation rotation;
        data.getICRFToMODTransform(tp, rotation);
        Vector3d posICRF{5000_km, 4000_km, 3000_km};
        Vector3d posMOD = rotation.transformVector(posICRF);
        printf("posICRF: %s\n", posICRF.toString().c_str());
        printf("posMOD: %s\n", posMOD.toString().c_str());
        Vector3d posMODExpect {6318.963219494652549_km, 2889.313747884160875_km, 1312.467103192060449_km };
        EXPECT_NEAR(posMODExpect[0], posMOD[0], 1e-9);
        EXPECT_NEAR(posMODExpect[1], posMOD[1], 1e-9);
        EXPECT_NEAR(posMODExpect[2], posMOD[2], 1e-9);
    }
   
}


TEST(RotationalData, JupiterAttitude2009)
{
    RotationalData data;
    std::string datadir = aDataDirGet();
    err_t rc = data.load(datadir + "/SolarSystem/Jupiter/JupiterAttitude2009.rot");
    EXPECT_EQ(rc, eNoError);
    // test Fixed Frame
    {
        auto tp = TimePoint::FromUTC(2026, 3, 22, 0, 0, 0);
        Rotation rotation;
        data.getICRFToFixedTransform(tp, rotation);
        Vector3d posICRF{3000_km, 4000_km, 5000_km};
        Vector3d posFixed = rotation.transformVector(posICRF);
        printf("posICRF: %s\n", posICRF.toString().c_str());
        printf("posFixed: %s\n", posFixed.toString().c_str());
        Vector3d posFixedExpect {5658.579843367369904_km, -3229.597213573279078_km, 2747.758358065032098_km };
        EXPECT_NEAR(posFixedExpect[0], posFixed[0], 1e-4);
        EXPECT_NEAR(posFixedExpect[1], posFixed[1], 2e-4);
        EXPECT_NEAR(posFixedExpect[2], posFixed[2], 1e-9);
    }
    // test Inertial Frame
    {
        auto tp = TimePoint::FromUTC(2026, 3, 22, 0, 0, 0);
        Rotation rotation;
        data.getICRFToInertialTransform(tp, rotation);
        Vector3d posICRF{3000_km, 4000_km, 5000_km};
        Vector3d posInertial = rotation.transformVector(posICRF);
        printf("posICRF: %s\n", posICRF.toString().c_str());
        printf("posInertial: %s\n", posInertial.toString().c_str());
        Vector3d posInertialExpect {2862.668762481611793_km, 5852.819746658852637_km, 2747.658743263475571_km};
        EXPECT_NEAR(posInertialExpect[0], posInertial[0], 1e-9);
        EXPECT_NEAR(posInertialExpect[1], posInertial[1], 1e-8);
        EXPECT_NEAR(posInertialExpect[2], posInertial[2], 1e-9);
    }
    // test TOD Frame
    {
        auto tp = TimePoint::FromUTC(2026, 3, 22, 0, 0, 0);
        Rotation rotation;
        data.getICRFToTODTransform(tp, rotation);
        Vector3d posICRF{3000_km, 4000_km, 5000_km};
        Vector3d posTOD = rotation.transformVector(posICRF);
        printf("posICRF: %s\n", posICRF.toString().c_str());
        printf("posTOD: %s\n", posTOD.toString().c_str());
        Vector3d posTODExpect {2862.703145508322905_km, 5852.756163243137053_km, 2747.758358065032098_km };
        EXPECT_NEAR(posTODExpect[0], posTOD[0], 1e-9);
        EXPECT_NEAR(posTODExpect[1], posTOD[1], 1e-9);
        EXPECT_NEAR(posTODExpect[2], posTOD[2], 1e-9);
    }
    // test MOD Frame
    {
        auto tp = TimePoint::FromUTC(2026, 3, 22, 0, 0, 0);
        Rotation rotation;
        data.getICRFToMODTransform(tp, rotation);
        Vector3d posICRF{3000_km, 4000_km, 5000_km};
        Vector3d posMOD = rotation.transformVector(posICRF);
        printf("posICRF: %s\n", posICRF.toString().c_str());
        printf("posMOD: %s\n", posMOD.toString().c_str());
        Vector3d posMODExpect {2862.503268078385190_km, 5852.918019026400543_km, 2747.621826743381462_km };
        EXPECT_NEAR(posMODExpect[0], posMOD[0], 1e-9);
        EXPECT_NEAR(posMODExpect[1], posMOD[1], 1e-9);
        EXPECT_NEAR(posMODExpect[2], posMOD[2], 1e-9);
    }
}


TEST(RotationalData, MoonAttitude2000)
{
    RotationalData data;
    std::string datadir = aDataDirGet();
    err_t rc = data.load(datadir + "/SolarSystem/Moon/MoonAttitude2000.rot");
    EXPECT_EQ(rc, eNoError);
    // test Fixed Frame
    {
        auto tp = TimePoint::FromUTC(2026, 3, 22, 0, 0, 0);
        Rotation rotation;
        data.getICRFToFixedTransform(tp, rotation);
        Vector3d posICRF{2000_km, 3000_km, 4000_km};
        Vector3d posFixed = rotation.transformVector(posICRF);
        printf("posICRF: %s\n", posICRF.toString().c_str());
        printf("posFixed: %s\n", posFixed.toString().c_str());
        Vector3d posFixedExpect {-4324.097951102970910_km, -1878.910816636085656_km, 2602.281893338038117_km};
        EXPECT_NEAR(posFixedExpect[0], posFixed[0], 1e-6);
        EXPECT_NEAR(posFixedExpect[1], posFixed[1], 1e-6);
        EXPECT_NEAR(posFixedExpect[2], posFixed[2], 1e-9);
    }
    // test Inertial Frame
    {
        auto tp = TimePoint::FromUTC(2026, 3, 22, 0, 0, 0);
        Rotation rotation;
        data.getICRFToInertialTransform(tp, rotation);
        Vector3d posICRF{2000_km, 3000_km, 4000_km};
        Vector3d posInertial = rotation.transformVector(posICRF);
        printf("posICRF: %s\n", posICRF.toString().c_str());
        printf("posInertial: %s\n", posInertial.toString().c_str());
        Vector3d posInertialExpect {1832.546782605972339_km, 4478.505910121954003_km, 2363.209068737513462_km};
        EXPECT_NEAR(posInertialExpect[0], posInertial[0], 1e-6);
        EXPECT_NEAR(posInertialExpect[1], posInertial[1], 1e-6);
        EXPECT_NEAR(posInertialExpect[2], posInertial[2], 1e-9);
    }
    // test TOD Frame
    {
        auto tp = TimePoint::FromUTC(2026, 3, 22, 0, 0, 0);
        Rotation rotation;
        data.getICRFToTODTransform(tp, rotation);
        Vector3d posICRF{2000_km, 3000_km, 4000_km};
        Vector3d posTOD = rotation.transformVector(posICRF);
        printf("posICRF: %s\n", posICRF.toString().c_str());
        printf("posTOD: %s\n", posTOD.toString().c_str());
        // @todo: 月球的TOD与其他天体的TOD定义有区别
    }
    // test MOD Frame
    {
        auto tp = TimePoint::FromUTC(2026, 3, 22, 0, 0, 0);
        Rotation rotation;
        data.getICRFToMODTransform(tp, rotation);
        Vector3d posICRF{2000_km, 3000_km, 4000_km};
        Vector3d posMOD = rotation.transformVector(posICRF);
        printf("posICRF: %s\n", posICRF.toString().c_str());
        printf("posMOD: %s\n", posMOD.toString().c_str());
        Vector3d posMODExpect {1999.775516607365034_km, 4344.474255799083039_km, 2475.164908420598294_km};
        EXPECT_NEAR(posMODExpect[0], posMOD[0], 1e-6);
        EXPECT_NEAR(posMODExpect[1], posMOD[1], 1e-6);
        EXPECT_NEAR(posMODExpect[2], posMOD[2], 1e-9);
    }
}

GTEST_MAIN()


///
/// @file      testObjectLoader.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-08
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "AstLoader/ObjectLoader.hpp"
#include "AstTest/Test.h"
#include "AstSim/Facility.hpp"
#include "AstSim/Target.hpp"
#include "AstSim/Place.hpp"
#include "AstSim/Sensor.hpp"

AST_USING_NAMESPACE

TEST(ObjectLoaderTest, LoadFacility)
{
    if(aIsCI())
        GTEST_SKIP();
        
    std::vector<std::string> files = aTestGetConfigStringVector("STK_FACILITY_FILES");
    for (const auto &file : files)
    {
        SharedPtr<Object> object;
        errc_t ret = aLoadObject(file, "Facility", object);
        Facility* facility = dynamic_cast<Facility*>(object.get());
        auto body = facility->body();
        EXPECT_TRUE(body != nullptr);
        EXPECT_TRUE(facility != nullptr);
        EXPECT_EQ(ret, 0);
        EXPECT_TRUE(object != nullptr);
    }
}

TEST(ObjectLoaderTest, LoadTarget)
{
    if(aIsCI())
        GTEST_SKIP();
        
    std::vector<std::string> files = aTestGetConfigStringVector("STK_TARGET_FILES");
    for (const auto &file : files)
    {
        SharedPtr<Object> object;
        errc_t ret = aLoadObject(file, "Target", object);
        Target* target = dynamic_cast<Target*>(object.get());
        auto body = target->body();
        EXPECT_TRUE(body != nullptr);
        EXPECT_TRUE(target != nullptr);
        EXPECT_EQ(ret, 0);
        EXPECT_TRUE(object != nullptr);
    }
}

TEST(ObjectLoaderTest, LoadPlace)
{
    if(aIsCI())
        GTEST_SKIP();
        
    std::vector<std::string> files = aTestGetConfigStringVector("STK_PLACE_FILES");
    for (const auto &file : files)
    {
        SharedPtr<Object> object;
        errc_t ret = aLoadObject(file, "Place", object);
        Place* place = dynamic_cast<Place*>(object.get());
        auto body = place->body();
        EXPECT_TRUE(body != nullptr);
        EXPECT_TRUE(place != nullptr);
        EXPECT_EQ(ret, 0);
        EXPECT_TRUE(object != nullptr);
    }
}

TEST(ObjectLoaderTest, LoadSensor)
{
    if(aIsCI())
        GTEST_SKIP();
        
    std::vector<std::string> files = aTestGetConfigStringVector("STK_SENSOR_FILES");
    for (const auto &file : files)
    {
        SharedPtr<Object> object;
        errc_t ret = aLoadObject(file, "Sensor", object);
        Sensor* sensor = dynamic_cast<Sensor*>(object.get());
        auto fov = sensor->getFieldOfView();
        EXPECT_TRUE(fov != nullptr);
        EXPECT_TRUE(sensor != nullptr);
        EXPECT_EQ(ret, 0);
        EXPECT_TRUE(object != nullptr);
    }
}

GTEST_MAIN();

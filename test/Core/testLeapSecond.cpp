/// @file      testLeapSecond.cpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      4.12.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。
 
 

#include "AstCore/LeapSecond.hpp"
#include "AstTest/AstTestMacro.h"
#include "AstCore/DateTime.hpp"
#include "AstCore/JulianDate.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstCore/RunTime.hpp"
#include <fstream>

AST_USING_NAMESPACE

TEST(LeapSecond, LoadDefault)
{
    LeapSecond leapSecond;
    err_t err = leapSecond.loadDefault();
    EXPECT_EQ(err, eNoError);
}

TEST(LeapSecond, normalize)
{
    DateTime dateTime{ 2023, 12, 31, 23, 59, 60 };
    dateTime.normalize();

    EXPECT_EQ(dateTime.year(), 2024);
    EXPECT_EQ(dateTime.month(), 1);
    EXPECT_EQ(dateTime.day(), 1);
    EXPECT_EQ(dateTime.hour(), 0);
    EXPECT_EQ(dateTime.minute(), 0);
    EXPECT_EQ(dateTime.second(), 0);
}

TEST(LeapSecond, normalizeUTC)
{
    {
        DateTime dateTime{ 2015, 6, 30, 23, 59, 60 };
        dateTime.normalizeUTC();

        EXPECT_EQ(dateTime.year(), 2015);
        EXPECT_EQ(dateTime.month(), 6);
        EXPECT_EQ(dateTime.day(), 30);
        EXPECT_EQ(dateTime.hour(), 23);
        EXPECT_EQ(dateTime.minute(), 59);
        EXPECT_EQ(dateTime.second(), 60);
    }
    {
        DateTime dateTime{ 2015, 6, 30, 23, 60, 0 };
        dateTime.normalizeUTC();

        EXPECT_EQ(dateTime.year(), 2015);
        EXPECT_EQ(dateTime.month(), 6);
        EXPECT_EQ(dateTime.day(), 30);
        EXPECT_EQ(dateTime.hour(), 23);
        EXPECT_EQ(dateTime.minute(), 59);
        EXPECT_EQ(dateTime.second(), 60);
    }
    {
        DateTime dateTime{ 2015, 6, 30, 23, 59, 62 };
        dateTime.normalizeUTC();

        EXPECT_EQ(dateTime.year(), 2015);
        EXPECT_EQ(dateTime.month(), 7);
        EXPECT_EQ(dateTime.day(), 1);
        EXPECT_EQ(dateTime.hour(), 0);
        EXPECT_EQ(dateTime.minute(), 0);
        EXPECT_EQ(dateTime.second(), 1);
    }
    {
        DateTime dateTime{ 2015, 6, 30, 23, 59, 60.12 };
        dateTime.normalizeUTC();

        EXPECT_EQ(dateTime.year(), 2015);
        EXPECT_EQ(dateTime.month(), 6);
        EXPECT_EQ(dateTime.day(), 30);
        EXPECT_EQ(dateTime.hour(), 23);
        EXPECT_EQ(dateTime.minute(), 59);
        EXPECT_EQ(dateTime.second(), 60.12);

    }
}

TEST(LeapSecond, normalizeLocal)
{
    {
        DateTime dateTime{ 2015, 7, 1, 7, 59, 60 };
        dateTime.normalizeLocal(8);

        EXPECT_EQ(dateTime.year(), 2015);
        EXPECT_EQ(dateTime.month(), 7);
        EXPECT_EQ(dateTime.day(), 1);
        EXPECT_EQ(dateTime.hour(), 7);
        EXPECT_EQ(dateTime.minute(), 59);
        EXPECT_EQ(dateTime.second(), 60);
    }
    {
        DateTime dateTime{ 2015, 7, 1, 7, 60, 0 };
        dateTime.normalizeLocal(8);

        EXPECT_EQ(dateTime.year(), 2015);
        EXPECT_EQ(dateTime.month(), 7);
        EXPECT_EQ(dateTime.day(), 1);
        EXPECT_EQ(dateTime.hour(), 7);
        EXPECT_EQ(dateTime.minute(), 59);
        EXPECT_EQ(dateTime.second(), 60);
    }
}


// 测试设置和获取闰秒数据
TEST(LeapSecond, setAndGetData) {
    LeapSecond leapSecond;
    
    // 测试setDefaultData
    leapSecond.setDefaultData();
    EXPECT_FALSE(leapSecond.data().empty());
    
    // 测试setData
    std::vector<double> mjds = {57000, 58000};
    std::vector<double> leapSeconds = {37, 38};
    leapSecond.setData(mjds, leapSeconds);
    EXPECT_EQ(leapSecond.data().size(), 2);
    EXPECT_EQ(leapSecond.data()[0].mjd, 57000);
    EXPECT_EQ(leapSecond.data()[0].leapSecond, 37);
    EXPECT_EQ(leapSecond.data()[1].mjd, 58000);
    EXPECT_EQ(leapSecond.data()[1].leapSecond, 38);
}

// 测试leapSecondUTC相关函数
TEST(LeapSecond, leapSecondUTC) {
    LeapSecond leapSecond;
    leapSecond.setDefaultData();
    
    // 测试已知日期的闰秒值
    // 2020-01-01 UTC对应的MJD约为58849，此时闰秒应为37
    double mjd2020 = 58849.0;
    double jd2020 = aMJDToJD_Imprecise(mjd2020);
    
    EXPECT_DOUBLE_EQ(leapSecond.leapSecondUTCMJD(mjd2020), 37.0);
    EXPECT_DOUBLE_EQ(leapSecond.leapSecondUTC(jd2020), 37.0);
    
    // 测试闰秒变更前一天
    // 最后一次闰秒发生在2017-01-01，MJD约为57754
    double mjdLeapDay = 57753.5; // 变更前一天
    EXPECT_DOUBLE_EQ(leapSecond.leapSecondUTCMJD(mjdLeapDay), 36.0);
    
    // 测试闰秒变更当天
    double mjdLeapDay2 = 57754.5; // 变更当天
    EXPECT_DOUBLE_EQ(leapSecond.leapSecondUTCMJD(mjdLeapDay2), 37.0);
}

// 测试leapSecondTAI相关函数
TEST(LeapSecond, leapSecondTAI) {
    LeapSecond leapSecond;
    leapSecond.setDefaultData();
    
    // TAI时间与UTC时间的关系：TAI = UTC + 闰秒
    // 对于2020-01-01，TAI比UTC快37秒
    double mjdUTC2020 = 58849.0;
    double mjdTAI2020 = mjdUTC2020 + 37.0 / 86400.0;
    double jdTAI2020 = aMJDToJD_Imprecise(mjdTAI2020);
    
    EXPECT_DOUBLE_EQ(leapSecond.leapSecondTAIMJD(mjdTAI2020), 37.0);
    EXPECT_DOUBLE_EQ(leapSecond.leapSecondTAI(jdTAI2020), 37.0);
}

// 测试getLodUTC相关函数
TEST(LeapSecond, getLodUTC) {
    LeapSecond leapSecond;
    leapSecond.setDefaultData();
    
    // 正常日期，一天应该是86400秒
    Date normalDate{ 2020, 1, 1 };
    EXPECT_DOUBLE_EQ(leapSecond.getLodUTC(normalDate), 86400.0);
    
    // 测试MJD版本
    double mjdNormal = aDateToMJD(normalDate);
    EXPECT_DOUBLE_EQ(leapSecond.getLodUTCMJD(mjdNormal), 86400.0);
    
    // 测试闰秒日的前一天（应该包含闰秒）
    // 最后一次闰秒是2017-01-01，前一天是2016-12-31
    Date leapDayBefore{ 2016, 12, 31 };
    double expectedLod = 86400.0 - 36.0 + 37.0; // 86401秒
    EXPECT_DOUBLE_EQ(leapSecond.getLodUTC(leapDayBefore), expectedLod);
    
    // 测试MJD版本
    double mjdLeapBefore = aDateToMJD(leapDayBefore);
    EXPECT_DOUBLE_EQ(leapSecond.getLodUTCMJD(mjdLeapBefore), expectedLod);
}

// 测试文件加载函数（使用临时文件）
TEST(LeapSecond, fileLoading) {
    LeapSecond leapSecond;
    
    // 创建临时HPIERS格式文件进行测试
    std::string tempFilePath = "temp_leap_second.dat";
    {
        std::ofstream tempFile(tempFilePath);
        tempFile << "# 临时闰秒测试文件\n";
        tempFile << "57023 1 1 2015 36\n";
        tempFile << "58119 1 1 2018 37\n";
        tempFile.close();
    }
    
    // 测试loadHPIERS
    err_t err = leapSecond.loadHPIERS(tempFilePath.c_str());
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(leapSecond.data().size(), 2);
    EXPECT_EQ(leapSecond.data()[0].mjd, 57023);
    EXPECT_EQ(leapSecond.data()[0].leapSecond, 36);
    
    // 测试load（应该调用loadHPIERS）
    LeapSecond leapSecond2;
    err = leapSecond2.load(tempFilePath.c_str());
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(leapSecond2.data().size(), 2);
    
    // 清理临时文件
    std::remove(tempFilePath.c_str());
    
    // 测试空指针和不存在的文件
    EXPECT_EQ(leapSecond.loadHPIERS(nullptr), eErrorNullInput);
    EXPECT_NE(leapSecond.loadHPIERS("non_existent_file.dat"), 0);
}


TEST(LeapSecond, loadATK)
{
    LeapSecond leapSecond;
    auto filepath = fs::path(aDataDirGet()) / "Test/LeapSecond.dat";
    err_t err = leapSecond.loadATK(filepath.c_str());
    EXPECT_EQ(err, 0);
    EXPECT_EQ(leapSecond.loadATK(nullptr), eErrorNullInput);
    EXPECT_NE(leapSecond.loadATK("non_existent_file.dat"), 0);
}
GTEST_MAIN()
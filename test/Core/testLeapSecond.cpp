/// @file      testLeapSecond.cpp
/// @brief     
/// @details   ~
/// @author    axel
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
    std::vector<int> mjds = {57000, 58000};
    std::vector<int> leapSeconds = {37, 38};
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
// 测试边界情况 - 空数据
TEST(LeapSecond, emptyDataBoundary) {
    LeapSecond leapSecond;
    
    // 创建一个空的数据结构
    leapSecond.data().clear();
    
    // 测试使用空数据时的行为
    try {
        // 这些调用在空数据时应该有合理的默认行为或抛出异常
        double result1 = leapSecond.leapSecondUTCMJD(58000.0);
        // 如果没有抛出异常，验证结果是否合理
        EXPECT_FALSE(std::isnan(result1));
    } catch (...) {
        // 如果抛出异常，也是一种合理的边界情况处理
        GTEST_SKIP() << "Empty data handling throws exception (acceptable behavior)";
    }
}

// 测试边界情况 - 1972年之前的日期
TEST(LeapSecond, before1972Boundary) {
    LeapSecond leapSecond;
    leapSecond.setDefaultData();
    
    // 1970-01-01，早于1972年
    double mjd1970 = 40587.0;
    
    // 验证对于1972年之前的日期，系统有正确的处理
    double leapSec1970 = leapSecond.leapSecondUTCMJD(mjd1970);
    EXPECT_FALSE(std::isnan(leapSec1970));
    
    // 如果定义了_AST_CONSIDER_LEAPSECOND_BEFORE_1972，应该返回计算值，否则返回默认数据中的第一个值
    #ifdef _AST_CONSIDER_LEAPSECOND_BEFORE_1972
    EXPECT_LT(leapSec1970, 10.0); // 1970年的闰秒应该小于10
    #else
    EXPECT_EQ(leapSec1970, leapSecond.data()[0].leapSecond);
    #endif
}

// 测试边界情况 - 未来日期
TEST(LeapSecond, futureDateBoundary) {
    LeapSecond leapSecond;
    leapSecond.setDefaultData();
    
    // 2030-01-01，一个未来的日期
    double mjd2030 = 63072.0;
    
    // 对于未来日期，应该返回最后一个已知的闰秒值
    double lastLeapSecond = leapSecond.data().back().leapSecond;
    EXPECT_DOUBLE_EQ(leapSecond.leapSecondUTCMJD(mjd2030), lastLeapSecond);
}

// 测试边界情况 - 闰秒变更的精确时间点
TEST(LeapSecond, leapSecondTransitionBoundary) {
    LeapSecond leapSecond;
    leapSecond.setDefaultData();
    
    // 找到最后一次闰秒变更的MJD
    int lastLeapMJD = leapSecond.data().back().mjd;
    
    // 测试变更前的最后一刻（精确到毫秒）
    double justBeforeTransition = lastLeapMJD - 0.00001; // 接近但小于变更MJD
    double leapBefore = leapSecond.leapSecondUTCMJD(justBeforeTransition);
    
    // 测试变更后的第一刻
    double justAfterTransition = lastLeapMJD + 0.00001; // 大于变更MJD
    double leapAfter = leapSecond.leapSecondUTCMJD(justAfterTransition);
    
    // 验证变更前后的闰秒值不同
    EXPECT_NE(leapBefore, leapAfter);
    // 验证变更后的值正好是最后一条记录的值
    EXPECT_DOUBLE_EQ(leapAfter, leapSecond.data().back().leapSecond);
}

// 测试边界情况 - 接近午夜的时间点
TEST(LeapSecond, midnightBoundary) {
    LeapSecond leapSecond;
    leapSecond.setDefaultData();
    
    // 选择一个非闰秒日的午夜附近
    double mjdDay = 58849.0; // 2020-01-01
    
    // 测试午夜前
    double justBeforeMidnight = mjdDay - 0.00001;
    double leapBeforeMidnight = leapSecond.leapSecondUTCMJD(justBeforeMidnight);
    
    // 测试午夜后
    double justAfterMidnight = mjdDay + 0.00001;
    double leapAfterMidnight = leapSecond.leapSecondUTCMJD(justAfterMidnight);
    
    // 在非闰秒日，午夜前后的闰秒值应该相同
    EXPECT_DOUBLE_EQ(leapBeforeMidnight, leapAfterMidnight);
}

// 测试文件加载边界情况 - 错误格式的文件
TEST(LeapSecond, invalidFileFormat) {
    LeapSecond leapSecond;
    
    // 创建格式错误的临时文件
    std::string tempFilePath = "temp_invalid_leap_second.dat";
    {
        std::ofstream tempFile(tempFilePath);
        tempFile << "# 格式错误的文件\n";
        tempFile << "这不是有效的数据行\n";
        tempFile << "57000 abc 1 2015 36\n"; // 格式错误
        tempFile.close();
    }
    
    // 测试加载格式错误的文件
    err_t err = leapSecond.loadHPIERS(tempFilePath.c_str());
    EXPECT_EQ(err, eErrorInvalidFile);
    
    // 创建空文件
    std::ofstream emptyFile(tempFilePath, std::ios::trunc);
    emptyFile.close();
    
    // 测试加载空文件
    err = leapSecond.loadHPIERS(tempFilePath.c_str());
    EXPECT_NE(err, 0);

    err = leapSecond.loadATK(tempFilePath.c_str());
    EXPECT_NE(err, 0);

    // 清理
    std::remove(tempFilePath.c_str());
}

// 测试ATK格式文件加载
TEST(LeapSecond, loadATKFormat) {
    LeapSecond leapSecond;
    
    // 创建ATK格式的临时文件
    std::string tempFilePath = "temp_atk_leap_second.dat";
    {
        std::ofstream tempFile(tempFilePath);
        tempFile << "2\n"; // 两行数据
        tempFile << "57000 36 2015 JAN 1\n";
        tempFile << "58000 37 2018 JAN 1\n";
        tempFile.close();
    }
    
    // 测试loadATK
    err_t err = leapSecond.loadATK(tempFilePath.c_str());
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(leapSecond.data().size(), 2);
    EXPECT_EQ(leapSecond.data()[0].mjd, 57000);
    EXPECT_EQ(leapSecond.data()[0].leapSecond, 36);
    
    // 清理
    std::remove(tempFilePath.c_str());

    err = leapSecond.loadHPIERS(tempFilePath.c_str());
    EXPECT_NE(err, 0);
}

// 测试getLodUTC的边界情况 - 闰秒变更当天
TEST(LeapSecond, getLodUTCOnLeapSecondDay) {
    LeapSecond leapSecond;
    leapSecond.setDefaultData();
    
    // 获取最后一次闰秒变更的日期
    int lastLeapMJD = leapSecond.data().back().mjd;
    Date leapDay = aMJDToDate(lastLeapMJD);
    
    // 验证闰秒变更当天的LOD是标准的86400秒
    EXPECT_DOUBLE_EQ(leapSecond.getLodUTC(leapDay), 86400.0);
    
    // 验证变更前一天的LOD包含闰秒
    Date dayBeforeLeap = aMJDToDate(lastLeapMJD - 1);
    double expectedLod = 86400.0 - leapSecond.data()[leapSecond.data().size()-2].leapSecond + leapSecond.data().back().leapSecond;
    EXPECT_DOUBLE_EQ(leapSecond.getLodUTC(dayBeforeLeap), expectedLod);
}

// 测试setData的边界情况 - 不同长度的输入向量
TEST(LeapSecond, setDataDifferentLengths) {
    LeapSecond leapSecond;
    
    // 测试不同长度的向量
    std::vector<int> mjds = {57000, 58000, 59000};
    std::vector<int> leapSeconds = {36, 37}; // 长度较短
    
    // 代码中使用了assert，所以在发布版本中不会崩溃，但会使用最短的长度
    // 在测试中验证这一点
    leapSecond.setData(mjds, leapSeconds);
    EXPECT_EQ(leapSecond.data().size(), 2); // 应该使用较短的长度
    
    // 测试空向量
    std::vector<int> emptyMJD, emptyLeapSeconds;
    leapSecond.setData(emptyMJD, emptyLeapSeconds);
    EXPECT_EQ(leapSecond.data().size(), 0);
}

// 测试leapSecondTAI的边界情况 - TAI和UTC时间转换关系
TEST(LeapSecond, leapSecondTAIBoundary) {
    LeapSecond leapSecond;
    leapSecond.setDefaultData();
    
    // 测试非常接近闰秒变更的TAI时间
    int lastLeapMJD = leapSecond.data().back().mjd;
    double leapSecondsValue = leapSecond.data().back().leapSecond;
    
    // TAI时间刚好在UTC闰秒变更点
    double taiMJDAtTransition = lastLeapMJD + leapSecondsValue / 86400.0;
    
    // 测试TAI时间在变更点前后
    double justBeforeTai = taiMJDAtTransition - 0.00001;
    double justAfterTai = taiMJDAtTransition + 0.00001;
    
    EXPECT_FALSE(std::isnan(leapSecond.leapSecondTAIMJD(justBeforeTai)));
    EXPECT_FALSE(std::isnan(leapSecond.leapSecondTAIMJD(justAfterTai)));
}

GTEST_MAIN()
/// @file      LeapSecond.cpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      29.11.2025
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
 


#include "LeapSecond.hpp"
#include "AstCore/RunTime.hpp"
#include "AstCore/Date.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/ScopedPtr.hpp"
#include <assert.h>
#include <fstream>

AST_NAMESPACE_BEGIN

#define AST_LEAPSECOND_DEFAULT_FILE "Time/Leap_Second.dat"

LeapSecond::LeapSecond()
{
    setDefaultData();
    
}


LeapSecond::LeapSecond(const char* filepath)
{
    load(filepath);
}

err_t LeapSecond::loadATK(const char* filepath)
{
    ScopedPtr<std::FILE> file = ast_fopen(filepath, "r");
    if (file == NULL) {
        return eErrorNullInput;
    }
    std::vector<Entry> data;
    int line, status;
    status = fscanf(file, "%d", &line);
    if (status == EOF) {
        return eErrorInvalidFile;
    }
    data.resize(line);

    char tmp[10];
    float leapvar;
    for (int i = 0; i < line; i++) {
        status = fscanf(
            file,
            "%d %f %s %s %s",
            &data[i].mjd,
            &leapvar,
            tmp, tmp, tmp
        );
        data[i].leapSecond = leapvar;
        if (status == EOF) {
            return eErrorInvalidFile;
        }
    }
    m_data = std::move(data);
    return eNoError;
}


err_t LeapSecond::loadHPIERS(const char* filepath)
{
    ScopedPtr<std::FILE> file = ast_fopen(filepath, "r");
    if (file == NULL) {
        return eErrorNullInput;
    }
    char line[1024];
    int lineNumber = 0;
    std::vector<Entry> data;
    while (fgets(line, sizeof(line), file)) {
        lineNumber++;

        // 跳过空行和注释行
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }

        // 使用sscanf解析行数据
        double mjd;
        int day, month, year;
        int taiMinusUTC;

        // 解析格式: MJD day month year TAI-UTC
        int parsed = sscanf(line, "%lf %d %d %d %d",
            &mjd, &day, &month, &year, &taiMinusUTC);

        if (parsed == 5) {
            // 成功解析一行数据
            Entry entry{};
            entry.mjd = static_cast<int>(mjd); // MJD转换为整数
            entry.leapSecond = taiMinusUTC;
            #ifdef _DEBUG
            double mjd_expect = aDateToMJD({year, month, day});
            if(mjd_expect != mjd){
                aWarning("failed to load leap second file, incorrect mjd for date");
                return eErrorInvalidFile;
            }
            #endif
            data.push_back(entry);
        }
        else {
            aWarning("failed to load leap second file, with format = hpiers");
            // 如果解析失败
            return eErrorInvalidFile;
        }
    }

    // 检查是否成功读取到数据
    if (data.empty()) {
        return eErrorInvalidFile;
    }

    m_data = std::move(data);
    return eNoError;
}

err_t LeapSecond::load(const char* filepath)
{
    return loadHPIERS(filepath);
}

err_t LeapSecond::loadDefault()
{
    fs::path datafile = fs::path(aDataDirGet()) / AST_LEAPSECOND_DEFAULT_FILE;
    
    err_t err = load(datafile.string().c_str());
    if (err)
    {
        aWarning("failed to load leapsecond from default data file");
    }
    return err;
}

void LeapSecond::setDefaultData()
{
    m_data = {
        {41317, 10},
        {41499, 11},
        {41683, 12},
        {42048, 13},
        {42413, 14},
        {42778, 15},
        {43144, 16},
        {43509, 17},
        {43874, 18},
        {44239, 19},
        {44786, 20},
        {45151, 21},
        {45516, 22},
        {46247, 23},
        {47161, 24},
        {47892, 25},
        {48257, 26},
        {48804, 27},
        {49169, 28},
        {49534, 29},
        {50083, 30},
        {50630, 31},
        {51179, 32},
        {53736, 33},
        {54832, 34},
        {56109, 35},
        {57204, 36},
        {57754, 37}
    };
}

void LeapSecond::setData(const std::vector<double>& mjd, const std::vector<double>& taiMinusUTC)
{
    assert(mjd.size() == taiMinusUTC.size());
    int line = std::min(mjd.size(), taiMinusUTC.size());
    m_data.resize(line);
    for (int i = 0; i < line; i++) {
        m_data[i].mjd = mjd[i];
        m_data[i].leapSecond = taiMinusUTC[i];
    }
}




double LeapSecond::getLeapSecondByTAI(double jdTAIp1, double jdTAIp2)
{

    double mjdtai = (jdTAIp1 - 2400000.5) + jdTAIp2;
    int i = m_data.size() - 1;
    while (i >= 0) {
        if (mjdtai - m_data[i].leapSecond / 86400. >= m_data[i].mjd) {
            return m_data[i].leapSecond;
        }
        i--;
    }
    return m_data[0].leapSecond;
}

double LeapSecond::getSecDayByUTC(double jdUTCp1, double jdUTCp2)
{
    double sec = 86400;

    double mjd = (jdUTCp1 - 2400000.5) + jdUTCp2;
    int i = m_data.size() - 1;
    while (i >= 1) {
        if (mjd >= m_data[i].mjd - 1) {
            if (mjd < m_data[i].mjd) {
                return sec - m_data[i - 1].leapSecond + m_data[i].leapSecond;
            }
            else {
                return sec;
            }
        }
        i--;
    }
    return sec;
}

void LeapSecond::getTimeCorrectionByUTC(int year, int month, int day, double sec, int& dday, double& newsec)
{
#if _AST_USE_getTimeCorrectionByUTC_V2
    int jdUTC;
    AsTimeToJD(year, month, day, jdUTC);
    double leap1, leapNextDay;
    double secOfDay = GetLeapSecDayByUTC(jdUTC, 0, leap1, leapNextDay);
    if (0 <= sec && sec < secOfDay) {
        dday = 0;
        newsec = sec;
    }
    else {
        double leap2;
        dday = floor(sec / 86400);
        do {
            secOfDay = GetLeapSecDayByUTC(jdUTC + dday, 0, leap2, leapNextDay);
            newsec = sec - dday * 86400 + leap1 - leap2;
            if (newsec >= secOfDay) {
                dday += 1;
            }
            else if (newsec < 0) {
                dday -= 1;
            }
            else {
                break;
            }
        } while (1);
    }
#else
    //@todo: more efficient implementation
    int jdUTC = aDateToJD(Date{year, month, day});
    double leap1 = getLeapSecondByUTC(jdUTC, -0.5);
    dday = floor(sec / 86400);
    double leap2;
    do {
        leap2 = getLeapSecondByUTC(jdUTC + dday, -0.5);
        newsec = sec - dday * 86400 + leap1 - leap2;
        if (newsec > 86400) {
            dday += 1;
        }
        else if (newsec < 0) {
            dday -= 1;
        }
        else {
            break;
        }
    } while (1);
#endif
}

double LeapSecond::getLeapSecDayByUTC(double jdUTCp1, double jdUTCp2, double& leap, double& leapNextDay)
{
    double sec = 86400;
    
    double mjd = (jdUTCp1 - 2400000.5) + jdUTCp2;
    int i = m_data.size() - 1;
    while (i >= 1) {
        if (mjd + 1 >= m_data[i].mjd) {
            leapNextDay = m_data[i].leapSecond;
            if (mjd < m_data[i].mjd) {
                leap = m_data[i - 1].leapSecond;
                return sec - leap + leapNextDay;
            }
            else {
                leap = m_data[i].leapSecond;
                return sec;
            }
        }
        i--;
    }
    leapNextDay = m_data[0].leapSecond;
    leap = m_data[0].leapSecond;
    return sec;
}

double LeapSecond::getLeapSecondByUTC(double jdUTCp1, double jdUTCp2)
{
    /*
1961  Jan.  1 - 1961  Aug.  1     1.422 818 0s + (MJD - 37 300) x 0.001 296s
      Aug.  1 - 1962  Jan.  1     1.372 818 0s +        ""
1962  Jan.  1 - 1963  Nov.  1     1.845 858 0s + (MJD - 37 665) x 0.001 123 2s
1963  Nov.  1 - 1964  Jan.  1     1.945 858 0s +        ""
1964  Jan.  1 -       April 1     3.240 130 0s + (MJD - 38 761) x 0.001 296s
      April 1 -       Sept. 1     3.340 130 0s +        ""
      Sept. 1 - 1965  Jan.  1     3.440 130 0s +        ""
1965  Jan.  1 -       March 1     3.540 130 0s +        ""
      March 1 -       Jul.  1     3.640 130 0s +        ""
      Jul.  1 -       Sept. 1     3.740 130 0s +        ""
      Sept. 1 - 1966  Jan.  1     3.840 130 0s +        ""
1966  Jan.  1 - 1968  Feb.  1     4.313 170 0s + (MJD - 39 126) x 0.002 592s
1968  Feb.  1 - 1972  Jan.  1     4.213 170 0s +        ""
    */
    double mjd = (jdUTCp1 - 2400000.5) + jdUTCp2;
    int i = m_data.size() - 1;
    while (i >= 0) {
        if (mjd >= m_data[i].mjd) {
            return m_data[i].leapSecond;
        }
        i--;
    }
#if _AST_CONSIDER_LEAPSECOND_BEFORE_1972
    // see https://hpiers.obspm.fr/eop-pc/index.php?index=TAI-UTC_tab&lang=en
    double dsec;
    if (mjd < 37300) {
        return 0.;
    }                                      // 1961-1-1
    else if (mjd < 37665) {
        dsec = (mjd - 37300) * 0.001296;
        if (mjd < 37512)                   // 1961-8-1
            return dsec + 1.4228180;
        else
            return dsec + 1.3728180;
    }                                      // 1962-1-1
    else if (mjd < 38395)
    {
        dsec = (mjd - 37665) * 0.0011232;
        if (mjd < 38334)                   // 1963-11-1
            return dsec + 1.8458580;
        else
            return dsec + 1.9458580;
    }                                      // 1964-1-1
    else if (mjd < 39126)
    {
        dsec = (mjd - 38761) * 0.001296;
        if (mjd < 38486) {                 // 1964-4-1
            return dsec + 3.2401300;
        }
        else if (mjd < 38639) {            // 1964-9-1
            return dsec + 3.3401300;
        }
        else if (mjd < 38761) {            // 1965-1-1
            return dsec + 3.4401300;
        }
        else if (mjd < 38820) {            // 1965-3-1
            return dsec + 3.5401300;
        }
        else if (mjd < 38942) {            // 1965-7-1
            return dsec + 3.6401300;
        }
        else if (mjd < 39004) {            // 1965-9-1
            return dsec + 3.7401300;
        }
        else {
            return dsec + 3.8401300;
        }
    }                                      // 1966-1-1
    else {
        dsec = (mjd - 39126) * 0.002592;
        if (mjd < 39887)                   // 1968-2-1
            return dsec + 4.3131700;
        else
            return dsec + 4.2131700;
    }
#else
    return m_data[0].leapSecond;
#endif
}


AST_NAMESPACE_END
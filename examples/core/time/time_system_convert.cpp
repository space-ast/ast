#include <AstCore/TimeSystem.hpp>
#include <AstCore/DateTime.hpp>
#include <AstCore/JulianDate.hpp>
#include <iostream>


int main() {
    AST_USING_NAMESPACE
    // 创建日期时间对象
    DateTime dttm;
    dttm.year() = 2023;
    dttm.month() = 1;
    dttm.day() = 1;
    dttm.hour() = 0;
    dttm.minute() = 0;
    dttm.second() = 0;

    std::cout << "UTC 日期时间:" << dttm.year() << "-" << dttm.month() << "-" << dttm.day() 
              << " " << dttm.hour() << ":" << dttm.minute() << ":" << dttm.second() << std::endl;

    // 转换为儒略日
    JulianDate utc_jd;
    aDateTimeToJD(dttm, utc_jd);
    std::cout << "UTC 儒略日:" << utc_jd.impreciseDay() << std::endl;

    // 转换为TAI时间
    JulianDate tai_jd;
    aUTCToTAI(utc_jd, tai_jd);
    std::cout << "TAI 儒略日:" << tai_jd.impreciseDay() << std::endl;

    // 转换为TT时间
    JulianDate tt_jd;
    aUTCToTT(utc_jd, tt_jd);
    std::cout << "TT 儒略日:" << tt_jd.impreciseDay() << std::endl;
}
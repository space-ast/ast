#include <AstCore/JulianDate.hpp>
#include <AstCore/DateTime.hpp>
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

    std::cout << "原始日期时间:" << dttm.year() << "-" << dttm.month() << "-" << dttm.day() 
              << " " << dttm.hour() << ":" << dttm.minute() << ":" << dttm.second() << std::endl;
    // 转换为儒略日
    JulianDate jd;
    aDateTimeToJD(dttm, jd);

    std::cout << "儒略日:" << jd.impreciseDay() << std::endl;

    // 转换回日期时间
    DateTime dttm2;
    aJDToDateTime(jd, dttm2);

    std::cout << "转换回日期时间：" << dttm2.year() << "-" << dttm2.month() << "-" << dttm2.day() 
              << " " << dttm2.hour() << ":" << dttm2.minute() << ":" << dttm2.second() << std::endl;
}
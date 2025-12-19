#include <AstCore/DateTime.hpp>
#include <iostream>

int main() {
    AST_USING_NAMESPACE
    // 创建日期时间对象 
    DateTime dttm;
    dttm.year() = 2023;
    dttm.month() = 12;
    dttm.day() = 31;
    dttm.hour() = 23;
    dttm.minute() = 59;
    dttm.second() = 60.5; // 包含小数秒

    // 规范化日期时间 
    std::cout << "规范化前:" << dttm.year() << "-" << dttm.month() << "-" << dttm.day() 
              << " " << dttm.hour() << ":" << dttm.minute() << ":" << dttm.second() << std::endl;

    aDateTimeNormalizeUTC(dttm);

    std::cout << "规范化后:" << dttm.year() << "-" << dttm.month() << "-" << dttm.day() 
              << " " << dttm.hour() << ":" << dttm.minute() << ":" << dttm.second() << std::endl;
}
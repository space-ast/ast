///
/// @file      py_time.cpp
/// @brief     时间系统Python绑定
/// @details   Date, DateTime, JulianDate, TimePoint等时间类的pybind11绑定
/// @author    axel
/// @date      2026-04-09
/// @copyright 版权所有 (C) 2026-present, space-axel/astpy
///

#include "pybind_ast.h"
#include "AstCore/Time/Date.hpp"
#include "AstCore/Time/DateTime.hpp"
#include "AstCore/Time/JulianDate.hpp"
#include "AstCore/Time/TimePoint.hpp"
#include "AstCore/Time/TimeSystem.hpp"

using namespace ast;

void bind_time(py::module& m)
{
    // Date class
    py::class_<Date>(m, "Date", "日历日期类")
        .def(py::init<>(), "默认构造函数")
        .def(py::init<int, int, int>(), "根据年月日创建日期", 
             py::arg("year"), py::arg("month"), py::arg("day"))
        .def_static("fromYMD", &Date::FromYMD, "根据年月日创建日期",
                    py::arg("year"), py::arg("month"), py::arg("day"))
        .def_static("fromJDAtNoon", &Date::FromJDAtNoon, "根据儒略日数创建日期",
                    py::arg("jd"))
        .def_static("fromYD", &Date::FromYD, "根据年份和年内天数创建日期",
                    py::arg("year"), py::arg("days"))
        .def_property("year", &Date::year, &Date::year, "年")
        .def_property("month", &Date::month, &Date::month, "月")
        .def_property("day", &Date::day, &Date::day, "日")
        .def("isLeapYear", &Date::isLeapYear, "是否为闰年")
        .def("dayOfWeek", &Date::dayOfWeek, "星期几(0=周日)")
        .def("dayOfYear", &Date::dayOfYear, "年内第几天")
        .def("dayInMonth", &Date::dayInMonth, "该月天数")
        .def("monthFullName", &Date::monthFullName, "月份英文全名")
        .def("monthShortName", &Date::monthShortName, "月份英文缩写")
        .def("weekDayFullName", &Date::weekDayFullName, "星期英文全名")
        .def("weekDayShortName", &Date::weekDayShortName, "星期英文缩写")
        .def("__str__", [](const Date& d) {
            return std::to_string(d.year()) + "-" + 
                   std::to_string(d.month()) + "-" + 
                   std::to_string(d.day());
        })
        .def("__repr__", [](const Date& d) {
            return "Date(" + std::to_string(d.year()) + ", " + 
                   std::to_string(d.month()) + ", " + 
                   std::to_string(d.day()) + ")";
        });

    // Time class
    py::class_<Time>(m, "Time", "时间类(时分秒)")
        .def(py::init<>(), "默认构造函数")
        .def(py::init<int, int, double>(), "根据时分秒创建时间",
             py::arg("hour"), py::arg("minute"), py::arg("second"))
        .def_property("hour", &Time::hour, &Time::hour, "时")
        .def_property("minute", &Time::minute, &Time::minute, "分")
        .def_property("second", &Time::second, &Time::second, "秒")
        .def("__str__", [](const Time& t) {
            return std::to_string(t.hour()) + ":" + 
                   std::to_string(t.minute()) + ":" + 
                   std::to_string(t.second());
        });

    // DateTime class
    py::class_<DateTime>(m, "DateTime", "日期时间类")
        .def(py::init<>(), "默认构造函数")
        .def(py::init<int, int, int, int, int, double>(), 
             "根据年月日时分秒创建日期时间",
             py::arg("year"), py::arg("month"), py::arg("day"),
             py::arg("hour"), py::arg("minute"), py::arg("second"))
        .def(py::init<const Date&, const Time&>(),
             "根据Date和Time创建日期时间")
        .def_static("fromYMDHMS", [](int y, int mo, int d, int h, int mi, double s) {
            return DateTime(Date::FromYMD(y, mo, d), Time(h, mi, s));
        }, "根据年月日时分秒创建日期时间",
           py::arg("year"), py::arg("month"), py::arg("day"),
           py::arg("hour"), py::arg("minute"), py::arg("second"))
        .def_property("date", &DateTime::date, &DateTime::date, "日期")
        .def_property("time", &DateTime::time, &DateTime::time, "时间")
        .def_property("year", &DateTime::year, &DateTime::year, "年")
        .def_property("month", &DateTime::month, &DateTime::month, "月")
        .def_property("day", &DateTime::day, &DateTime::day, "日")
        .def_property("hour", &DateTime::hour, &DateTime::hour, "时")
        .def_property("minute", &DateTime::minute, &DateTime::minute, "分")
        .def_property("second", &DateTime::second, &DateTime::second, "秒")
        .def("__str__", [](const DateTime& dt) {
            std::string str;
            aDateTimeFormatGregorian(dt, str);
            return str;
        });

    // JulianDate class
    py::class_<JulianDate>(m, "JulianDate", "儒略日")
        .def(py::init<>(), "默认构造函数")
        .def(py::init<int, double>(), "根据天数和秒数创建儒略日",
             py::arg("day"), py::arg("second"))
        .def_static("J2000", &JulianDate::J2000, "J2000.0历元儒略日")
        .def_static("fromImpreciseDay", &JulianDate::FromImpreciseDay, 
                    "从double天数创建儒略日", py::arg("jd"))
        .def_static("fromDateTime", &DateTime::FromDateTime,
                    "从DateTime创建儒略日", py::arg("dttm"))
        .def_property("day", &JulianDate::day, &JulianDate::day, "天数")
        .def_property("second", &JulianDate::second, &JulianDate::second, "秒数")
        .def("impreciseDay", &JulianDate::impreciseDay, "不精确的天数")
        .def("daysFromJ2000", &JulianDate::daysFromJ2000, "与J2000的天数差")
        .def("secondsFromJ2000", &JulianDate::secondsFromJ2000, "与J2000的秒数差")
        .def("julianCenturyFromJ2000", &JulianDate::julianCenturyFromJ2000, "儒略世纪")
        .def("__add__", [](const JulianDate& jd, double sec) { return jd + sec; })
        .def("__sub__", [](const JulianDate& jd, double sec) { return jd - sec; })
        .def("__repr__", [](const JulianDate& jd) {
            return "JulianDate(day=" + std::to_string(jd.day()) + 
                   ", second=" + std::to_string(jd.second()) + ")";
        });

    // TimePoint class
    py::class_<TimePoint>(m, "TimePoint", "时间点")
        .def(py::init<>(), "默认构造函数")
        .def(py::init<const JulianDate&, const TimeScale&>(),
             "根据儒略日和时间尺度创建时间点")
        .def(py::init<const DateTime&, const TimeScale&>(),
             "根据日期时间和时间尺度创建时间点")
        .def_property("jd", &TimePoint::jd, &TimePoint::jd, "儒略日")
        .def_property("scale", &TimePoint::scale, &TimePoint::scale, "时间尺度")
        .def("toTT", &TimePoint::toTT, "转换为地球时TT")
        .def("toTAI", &TimePoint::toTAI, "转换为原子时TAI")
        .def("toUTC", &TimePoint::toUTC, "转换为协调世界时UTC")
        .def("toTDB", &TimePoint::toTDB, "转换为质心动力学时TDB")
        .def("toGPST", &TimePoint::toGPST, "转换为GPS时")
        .def("__repr__", [](const TimePoint& tp) {
            DateTime dt;
            aJDToDateTime(tp.jd(), dt);
            std::string str;
            aDateTimeFormatISO8601(dt, str);
            return "TimePoint(scale=" + std::to_string(static_cast<int>(tp.scale())) + 
                   ", " + str + ")";
        });

    // 时间系统常量
    m.attr("TIME_SCALE_TT") = static_cast<int>(ETimeScale::eTT);
    m.attr("TIME_SCALE_TAI") = static_cast<int>(ETimeScale::eTAI);
    m.attr("TIME_SCALE_UTC") = static_cast<int>(ETimeScale::eUTC);
    m.attr("TIME_SCALE_UT1") = static_cast<int>(ETimeScale::eUT1);
    m.attr("TIME_SCALE_GPS") = static_cast<int>(ETimeScale::eGPS);
    m.attr("TIME_SCALE_TDB") = static_cast<int>(ETimeScale::eTDB);
    m.attr("TIME_SCALE_TCB") = static_cast<int>(ETimeScale::eTCB);

    // 时间转换函数
    m.def("aTAIToTT", [](const JulianDate& jd) { return aTAIToTT(jd); }, 
          "原子时(TAI)转地球时(TT)", py::arg("jd"));
    m.def("aTTToTAI", [](const JulianDate& jd) { return aTTToTAI(jd); }, 
          "地球时(TT)转原子时(TAI)", py::arg("jd"));
    m.def("aTAIToUTC", [](const JulianDate& jd) { return aTAIToUTC(jd); }, 
          "原子时(TAI)转协调世界时(UTC)", py::arg("jd"));
    m.def("aUTCToTAI", [](const JulianDate& jd) { return aUTCToTAI(jd); }, 
          "协调世界时(UTC)转原子时(TAI)", py::arg("jd"));
    m.def("aUTCToTT", [](const JulianDate& jd) { return aUTCToTT(jd); }, 
          "协调世界时(UTC)转地球时(TT)", py::arg("jd"));
    m.def("aTTToUTC", [](const JulianDate& jd) { return aTTToUTC(jd); }, 
          "地球时(TT)转协调世界时(UTC)", py::arg("jd"));
    m.def("aTTToTDB", [](const JulianDate& jd) { return aTTToTDB(jd); }, 
          "地球时(TT)转质心动力学时(TDB)", py::arg("jd"));
    m.def("aTDBToTT", [](const JulianDate& jd) { return aTDBToTT(jd); }, 
          "质心动力学时(TDB)转地球时(TT)", py::arg("jd"));
    m.def("aDateTimeToJD", [](const DateTime& dt) { 
        JulianDate jd; aDateTimeToJD(dt, jd); return jd; 
    }, "日期时间转儒略日", py::arg("dt"));
    m.def("aJDToDateTime", [](const JulianDate& jd) { 
        DateTime dt; aJDToDateTime(jd, dt); return dt; 
    }, "儒略日转日期时间", py::arg("jd"));
    m.def("aCurrentDateTimeUTC", []() { 
        DateTime dt; aCurrentDateTimeUTC(dt); return dt; 
    }, "获取当前UTC时间");
}

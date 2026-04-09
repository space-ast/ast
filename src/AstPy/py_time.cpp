/**
 * @file py_time.cpp
 * @brief 时间系统 Python 绑定实现
 * @author SpaceAST Development Team
 * @version 1.0.0
 * @date 2024
 */

#include "py_time.h"

namespace py = pybind11;
using namespace ast;

namespace astpy {

void bind_time(py::module& m) {
    // ============================================
    // 时间尺度枚举
    // ============================================
    py::enum_<TimeScale::ETimeScale>(m, "TimeScale", R"pbdoc(
        Time scale enumeration
        
        Enumeration values:
            eTT  - Terrestrial Time
            eTAI - International Atomic Time
            eUTC - Coordinated Universal Time
            eTDB - Barycentric Dynamical Time
    )pbdoc")
        .value("TT", TimeScale::ETimeScale::eTT, "Terrestrial Time")
        .value("TAI", TimeScale::ETimeScale::eTAI, "International Atomic Time")
        .value("UTC", TimeScale::ETimeScale::eUTC, "Coordinated Universal Time")
        .value("TDB", TimeScale::ETimeScale::eTDB, "Barycentric Dynamical Time")
        .export_values();

    // ============================================
    // TimeInterval 类
    // ============================================
    py::class_<TimeInterval>(m, "TimeInterval", R"pbdoc(
        Time interval class representing a duration between two time points.
        
        A TimeInterval represents the duration between a start and end time point.
    )pbdoc")
        .def(py::init<>(), "Create an empty time interval")
        .def(py::init<const TimePoint&, const TimePoint&>(),
             py::arg("start"), py::arg("end"),
             "Create a time interval with start and end time points")
        .def_property_readonly("start", &TimeInterval::GetStart,
            "Get the start time point")
        .def_property_readonly("end", &TimeInterval::GetEnd,
            "Get the end time point")
        .def("duration", &TimeInterval::Duration,
             "Get the duration in seconds")
        .def("__str__", &TimeInterval::ToString,
             "Get string representation");

    // ============================================
    // JulianDate 类
    // ============================================
    py::class_<JulianDate>(m, "JulianDate", R"pbdoc(
        Julian Date class for time representation.
        
        Julian Date is a continuous count of days since noon Universal Time 
        on January 1, 4713 BCE. This implementation uses integer days + 
        fractional seconds for numerical precision.
    )pbdoc")
        // 静态工厂方法
        .def_static("J2000", &JulianDate::J2000, R"pbdoc(
            Get the J2000 epoch (2451545.0 TT)
        )pbdoc")
        .def_static("FromImpreciseDay", &JulianDate::FromImpreciseDay,
            py::arg("jd"), R"pbdoc(
            Create JulianDate from imprecise day (continuous JD)
        )pbdoc")
        .def_static("FromDaySecond", &JulianDate::FromDaySecond,
            py::arg("day"), py::arg("second"), R"pbdoc(
            Create JulianDate from day number and seconds of day
        )pbdoc")
        .def_static("FromDateTime", &JulianDate::FromDateTime,
            py::arg("year"), py::arg("month"), py::arg("day"),
            py::arg("hour"), py::arg("minute"), py::arg("second"), R"pbdoc(
            Create JulianDate from date and time components
        )pbdoc")
        
        // 属性
        .def_property("day", &JulianDate::day, &JulianDate::setDay,
            "Get/set the day number")
        .def_property("second", &JulianDate::second, &JulianDate::setSecond,
            "Get/set the seconds of day")
        
        // 方法
        .def("impreciseDay", &JulianDate::impreciseDay,
             "Get the imprecise day (continuous JD)")
        .def("setImpreciseDay", &JulianDate::setImpreciseDay,
             py::arg("jd"), "Set from imprecise day")
        .def("dayFractional", &JulianDate::dayFractional,
             "Get the fractional part of the day")
        .def("julianCenturyFromJ2000", &JulianDate::julianCenturyFromJ2000,
             "Get Julian centuries from J2000")
        .def("daysFromJ2000", &JulianDate::daysFromJ2000,
             "Get days from J2000")
        .def("secondsFromJ2000", &JulianDate::secondsFromJ2000,
             "Get seconds from J2000")
        .def("shiftedBySecond", &JulianDate::shiftedBySecond,
             py::arg("second"), "Get a new JulianDate shifted by given seconds")
        .def("__add__", [](const JulianDate& self, double sec) {
            return self + sec;
        }, py::is_operator())
        .def("__sub__", [](const JulianDate& self, double sec) {
            return self - sec;
        }, py::is_operator())
        .def("__repr__", [](const JulianDate& self) {
            return "<JulianDate: " + std::to_string(self.impreciseDay()) + ">";
        });

    // ============================================
    // ModJulianDate 类 (简约儒略日)
    // ============================================
    py::class_<ModJulianDate>(m, "ModJulianDate", R"pbdoc(
        Modified Julian Date.
        
        MJD = JD - 2400000.5, commonly used in astronomy and satellite operations.
    )pbdoc")
        .def(py::init<>(), "Create an empty Modified Julian Date")
        .def(py::init<double>(), py::arg("mjd"), "Create from MJD value")
        .def_property("value", &ModJulianDate::Value, &ModJulianDate::SetValue,
            "Get/set the MJD value")
        .def("toJD", &ModJulianDate::ToJD, "Convert to Julian Date")
        .def_static("FromJD", &ModJulianDate::FromJD, py::arg("jd"),
            "Create from Julian Date")
        .def("__repr__", [](const ModJulianDate& self) {
            return "<ModJulianDate: " + std::to_string(self.Value()) + ">";
        });

    // ============================================
    // Date 类
    // ============================================
    py::class_<Date>(m, "Date", R"pbdoc(
        Date class for calendar date representation.
    )pbdoc")
        .def(py::init<>(), "Create an empty date")
        .def(py::init<int, int, int>(),
             py::arg("year"), py::arg("month"), py::arg("day"),
             "Create date from year, month, day")
        .def_property("year", &Date::Year, &Date::SetYear,
            "Get/set year")
        .def_property("month", &Date::Month, &Date::SetMonth,
            "Get/set month")
        .def_property("day", &Date::Day, &Date::SetDay,
            "Get/set day")
        .def("__repr__", [](const Date& self) {
            std::ostringstream oss;
            oss << self.Year() << "-" << self.Month() << "-" << self.Day();
            return "<Date: " + oss.str() + ">";
        });

    // ============================================
    // DateTime 类
    // ============================================
    py::class_<DateTime>(m, "DateTime", R"pbdoc(
        Date and time class.
    )pbdoc")
        .def(py::init<>(), "Create an empty datetime")
        .def(py::init<int, int, int, int, int, double>(),
             py::arg("year"), py::arg("month"), py::arg("day"),
             py::arg("hour"), py::arg("minute"), py::arg("second"),
             "Create datetime from components")
        .def_property("year", &DateTime::Year, &DateTime::SetYear,
            "Get/set year")
        .def_property("month", &DateTime::Month, &DateTime::SetMonth,
            "Get/set month")
        .def_property("day", &DateTime::Day, &DateTime::SetDay,
            "Get/set day")
        .def_property("hour", &DateTime::Hour, &DateTime::SetHour,
            "Get/set hour")
        .def_property("minute", &DateTime::Minute, &DateTime::SetMinute,
            "Get/set minute")
        .def_property("second", &DateTime::Second, &DateTime::SetSecond,
            "Get/set second")
        .def("toJulianDate", &DateTime::ToJulianDate,
             "Convert to JulianDate")
        .def_static("FromJulianDate", &DateTime::FromJulianDate,
            py::arg("jd"), "Create from JulianDate")
        .def("__repr__", [](const DateTime& self) {
            std::ostringstream oss;
            oss << self.Year() << "-" << self.Month() << "-" << self.Day() << " "
                << self.Hour() << ":" << self.Minute() << ":" << self.Second();
            return "<DateTime: " + oss.str() + ">";
        });

    // ============================================
    // TimePoint 类
    // ============================================
    py::class_<TimePoint>(m, "TimePoint", R"pbdoc(
        Absolute time point class.
        
        TimePoint represents a specific point in time, internally stored as
        a duration from J2000 TT epoch.
    )pbdoc")
        // 静态工厂方法
        .def_static("CurrentTime", &TimePoint::CurrentTime, R"pbdoc(
            Get the current system time
        )pbdoc")
        .def_static("TodayUTC", &TimePoint::TodayUTC, R"pbdoc(
            Get today's date at midnight UTC
        )pbdoc")
        .def_static("TomorrowUTC", &TimePoint::TomorrowUTC, R"pbdoc(
            Get tomorrow's date at midnight UTC
        )pbdoc")
        .def_static("J2000TT", &TimePoint::J2000TT, R"pbdoc(
            Get the J2000 epoch in TT
        )pbdoc")
        .def_static("FromUTC", static_cast<TimePoint(*)(int, int, int, int, int, double)>(
            &TimePoint::FromUTC),
            py::arg("year"), py::arg("month"), py::arg("day"),
            py::arg("hour"), py::arg("minute"), py::arg("second"),
            R"pbdoc(
            Create TimePoint from UTC components
        )pbdoc")
        .def_static("FromTT", &TimePoint::FromTT,
            py::arg("jdTT"), R"pbdoc(
            Create TimePoint from TT JulianDate
        )pbdoc")
        .def_static("FromTDB", &TimePoint::FromTDB,
            py::arg("jdTDB"), R"pbdoc(
            Create TimePoint from TDB JulianDate
        )pbdoc")
        .def_static("FromTAI", &TimePoint::FromTAI,
            py::arg("jdTAI"), R"pbdoc(
            Create TimePoint from TAI JulianDate
        )pbdoc")
        .def_static("FromImpreciseJDTT", &TimePoint::FromImpreciseJDTT,
            py::arg("jdTT"), R"pbdoc(
            Create TimePoint from imprecise TT Julian Date
        )pbdoc")
        
        // 方法
        .def("integerPart", &TimePoint::integerPart,
             "Get the integer part of the duration from J2000")
        .def("fractionalPart", &TimePoint::fractionalPart,
             "Get the fractional part of the duration from J2000")
        .def("daysFromJ2000TT", &TimePoint::daysFromJ2000TT,
             "Get days from J2000 in TT")
        .def("julianCenturyFromJ2000TT", &TimePoint::julianCenturyFromJ2000TT,
             "Get Julian centuries from J2000 in TT")
        .def("toTT", &TimePoint::toTT,
             "Convert to TT JulianDate")
        .def("toTDB", &TimePoint::toTDB,
             "Convert to TDB JulianDate")
        .def("toTAI", &TimePoint::toTAI,
             "Convert to TAI JulianDate")
        .def("toEpochSecond", &TimePoint::toEpochSecond,
             "Convert to epoch seconds")
        .def("durationFrom", &TimePoint::durationFrom,
             py::arg("other"), "Get duration from another time point (seconds)")
        .def("daysFrom", &TimePoint::daysFrom,
             py::arg("other"), "Get days from another time point")
        .def("shiftedBySecond", &TimePoint::shiftedBySecond,
             py::arg("second"), "Get a new TimePoint shifted by given seconds")
        .def("shiftedBySecondInTDB", &TimePoint::shiftedBySecondInTDB,
             py::arg("second"), "Get a new TimePoint shifted by given TDB seconds")
        .def("toString", &TimePoint::toString,
             py::arg("precision") = 6, "Convert to string representation")
        
        // 运算符
        .def("__add__", [](const TimePoint& self, double sec) {
            return self + sec;
        }, py::is_operator())
        .def("__sub__", [](const TimePoint& self, double sec) {
            return self - sec;
        }, py::is_operator())
        .def("__sub__", [](const TimePoint& self, const TimePoint& other) {
            return self - other;
        }, py::is_operator())
        
        .def("__repr__", [](const TimePoint& self) {
            return "<TimePoint: " + self.toString() + ">";
        });

    // ============================================
    // DaySecDuration 类
    // ============================================
    py::class_<DaySecDuration>(m, "DaySecDuration", R"pbdoc(
        Duration class with integer days and fractional seconds.
        
        This representation can handle very long durations without 
        precision loss.
    )pbdoc")
        .def(py::init<>(), "Create a zero duration")
        .def(py::init<int64_t, double>(),
             py::arg("days"), py::arg("seconds"),
             "Create from integer days and fractional seconds")
        .def(py::init<double>(),
             py::arg("seconds"), "Create from seconds (auto-converted)")
        .def_property("days", &DaySecDuration::GetDays, &DaySecDuration::SetDays,
            "Get/set the integer days")
        .def_property("seconds", &DaySecDuration::GetSeconds, &DaySecDuration::SetSeconds,
            "Get/set the fractional seconds")
        .def("toSeconds", &DaySecDuration::ToSeconds,
             "Convert total duration to seconds")
        .def("__repr__", [](const DaySecDuration& self) {
            return "<DaySecDuration: " + std::to_string(self.toSeconds()) + "s>";
        });
}

}  // namespace astpy

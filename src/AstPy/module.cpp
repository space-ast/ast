/**
 * @file module.cpp
 * @brief Python 模块主入口
 * @author SpaceAST Development Team
 * @version 1.0.0
 * @date 2024
 * 
 * SpaceAST Python 绑定模块
 * 
 * 本模块提供 SpaceAST C++ 航天仿真算法库的 Python 接口。
 * 主要功能包括：
 * - 时间系统（TimePoint, JulianDate, DateTime）
 * - 数学基础（Vector3d, Matrix3d, Quaternion, Euler）
 * - 轨道力学（OrbElem, CartState, TwoBody）
 * - 坐标系（Frame, Axes, Transform）
 * - 天体（CelestialBody, SolarSystem）
 * 
 * 使用示例:
 *     >>> import astpy
 *     >>> v = astpy.Vector3d(1, 2, 3)
 *     >>> tp = astpy.TimePoint.FromUTC(2024, 1, 1, 12, 0, 0)
 */

#include <pybind11/pybind11.h>

#include "pybind11.h"
#include "py_time.h"
#include "py_math.h"
#include "py_orbit.h"
#include "py_frame.h"
#include "py_body.h"
#include "py_quantity.h"

namespace py = pybind11;

PYBIND11_MODULE(_ast, m) {
    // 模块信息
    m.doc() = R"pbdoc(
        SpaceAST - Space Astrodynamics Library for Python
        
        SpaceAST provides Python bindings for the C++ astrodynamics library,
        including:
        - Time systems (TimePoint, JulianDate, DateTime)
        - Mathematics (Vector3d, Matrix3d, Quaternion, Euler)
        - Orbit mechanics (OrbElem, CartState, TwoBody)
        - Coordinate frames (Frame, Axes, Transform)
        - Celestial bodies (SolarSystem, Planet)
        
        Example:
            >>> import astpy
            >>> v = astpy.Vector3d(1.0, 2.0, 3.0)
            >>> print(v.norm())
            >>> tp = astpy.TimePoint.FromUTC(2024, 1, 1, 12, 0, 0)
    )pbdoc";

    // 版本信息
    m.attr("__version__") = "1.0.0";
    m.attr("__author__") = "SpaceAST Development Team";
    m.attr("__license__") = "Apache License 2.0";

    // ============================================
    // 异常处理
    // ============================================
    py::register_exception_translator([](std::exception_ptr p) {
        if (p) {
            try {
                std::rethrow_exception(p);
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, e.what());
            }
        }
    });

    // ============================================
    // 注册子模块
    // ============================================
    
    // 时间系统
    astpy::bind_time(m);
    
    // 数学类型
    astpy::bind_math(m);
    
    // 轨道力学
    astpy::bind_orbit(m);
    
    // 坐标系
    astpy::bind_frame(m);
    
    // 天体
    astpy::bind_body(m);
    
    // 物理量（带单位）
    astpy::bind_quantity(m);

    // ============================================
    // 全局常量
    // ============================================
    
    // 物理常量
    m.attr("SPEED_OF_LIGHT") = py::cast(299792.458);  // km/s
    m.attr("PI") = py::cast(3.14159265358979323846);
    m.attr("TWO_PI") = py::cast(6.28318530717958647692);
    m.attr("DEG_TO_RAD") = py::cast(0.017453292519943295);
    m.attr("RAD_TO_DEG") = py::cast(57.295779513082323);
    m.attr("ARC_SEC_TO_RAD") = py::cast(4.848136811095360e-6);
    
    // J2000 历元
    m.attr("J2000_JD") = py::cast(2451545.0);  // TT
    m.attr("J2000_MJD") = py::cast(51544.5);    // Modified JD
    
    // 地球参数
    m.attr("GM_EARTH") = py::cast(3.986004418e5);  // km^3/s^2
    m.attr("R_EARTH_EQ") = py::cast(6378.137);  // km (equatorial)
    m.attr("R_EARTH_POL") = py::cast(6356.752);  // km (polar)
    m.attr("W_EARTH") = py::cast(7.292115e-5);  // rad/s
    
    // 太阳参数
    m.attr("GM_SUN") = py::cast(1.32712440018e11);  // km^3/s^2
    m.attr("R_SUN") = py::cast(696340.0);  // km
    m.attr("AU") = py::cast(149597870.7);  // km
    
    // 月球参数
    m.attr("GM_MOON") = py::cast(4902.799);  // km^3/s^2
    m.attr("R_MOON") = py::cast(1738.0);  // km
    
    // ============================================
    // 全局辅助函数
    // ============================================
    m.def("deg", [](double deg) {
        return deg * 0.017453292519943295;
    }, py::arg("degrees"),
        "Convert degrees to radians");
    
    m.def("rad", [](double rad) {
        return rad * 57.295779513082323;
    }, py::arg("radians"),
        "Convert radians to degrees");
    
    m.def("arcsec", [](double arcsec) {
        return arcsec * 4.848136811095360e-6;
    }, py::arg("arcseconds"),
        "Convert arcseconds to radians");

    // ============================================
    // 便捷创建函数
    // ============================================
    m.def("vector3d", [](double x, double y, double z) {
        return ast::Vector3d(x, y, z);
    }, py::arg("x"), py::arg("y"), py::arg("z"),
        "Create a Vector3d from components");
    
    m.def("vector3d", [](py::list lst) {
        if (py::len(lst) != 3) {
            throw std::invalid_argument("Vector3d requires 3 elements");
        }
        return ast::Vector3d(
            py::cast<double>(lst[0]),
            py::cast<double>(lst[1]),
            py::cast<double>(lst[2])
        );
    }, py::arg("values"),
        "Create a Vector3d from list");
    
    m.def("quaternion", [](double w, double x, double y, double z) {
        return ast::Quaternion(w, x, y, z);
    }, py::arg("w"), py::arg("x"), py::arg("y"), py::arg("z"),
        "Create a Quaternion from w, x, y, z components");
    
    m.def("quaternion", [](py::list lst) {
        if (py::len(lst) != 4) {
            throw std::invalid_argument("Quaternion requires 4 elements");
        }
        return ast::Quaternion(
            py::cast<double>(lst[0]),
            py::cast<double>(lst[1]),
            py::cast<double>(lst[2]),
            py::cast<double>(lst[3])
        );
    }, py::arg("values"),
        "Create a Quaternion from list [w, x, y, z]");
    
    m.def("timepoint", [](int year, int month, int day, int hour, int minute, double second) {
        return ast::TimePoint::FromUTC(year, month, day, hour, minute, second);
    }, py::arg("year"), py::arg("month"), py::arg("day"),
        py::arg("hour"), py::arg("minute"), py::arg("second"),
        "Create a TimePoint from UTC date/time");
    
    m.def("orb_elem", [](double a, double e, double i, double raan, double argper, double trueA) {
        return ast::OrbElem(a, e, i, raan, argper, trueA);
    }, py::arg("a"), py::arg("e"), py::arg("i"),
        py::arg("raan"), py::arg("argper"), py::arg("trueA"),
        "Create orbital elements from six parameters");
    
    m.def("cart_state", [](double x, double y, double z, double vx, double vy, double vz) {
        return ast::CartState(ast::Vector3d(x, y, z), ast::Vector3d(vx, vy, vz));
    }, py::arg("x"), py::arg("y"), py::arg("z"),
        py::arg("vx"), py::arg("vy"), py::arg("vz"),
        "Create Cartesian state from position and velocity");
}

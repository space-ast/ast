///
/// @file      py_orbit.cpp
/// @brief     轨道力学Python绑定
/// @details   轨道根数、轨道参数的pybind11绑定
/// @author    axel
/// @date      2026-04-09
/// @copyright 版权所有 (C) 2026-present, space-axel/astpy
///

#include "pybind_ast.h"
#include "AstCore/Orbit/OrbitElement.hpp"
#include "AstCore/Orbit/OrbitParam.hpp"
#include "AstUtil/Constants.h"

using namespace ast;

void bind_orbit(py::module& m)
{
    // CartState class (直角坐标状态)
    py::class_<CartState>(m, "CartState", "笛卡尔轨道状态(位置速度)")
        .def(py::init<>(), "默认构造函数")
        .def(py::init<const Vector3d&, const Vector3d&>(),
             "根据位置和速度创建",
             py::arg("pos"), py::arg("vel"))
        .def_static("Zero", &CartState::Zero, "创建零状态")
        .def_property("pos", &CartState::pos, &CartState::pos, "位置向量")
        .def_property("vel", &CartState::vel, &CartState::vel, "速度向量")
        .def_property_readonly("x", &CartState::x)
        .def_property_readonly("y", &CartState::y)
        .def_property_readonly("z", &CartState::z)
        .def_property_readonly("vx", &CartState::vx)
        .def_property_readonly("vy", &CartState::vy)
        .def_property_readonly("vz", &CartState::vz)
        .def("__repr__", [](const CartState& s) {
            return "CartState(pos=" + std::to_string(s.x()) + ", " + 
                              std::to_string(s.y()) + ", " + 
                              std::to_string(s.z()) + ", vel=" +
                              std::to_string(s.vx()) + ", " +
                              std::to_string(s.vy()) + ", " +
                              std::to_string(s.vz()) + ")";
        });

    // OrbElem class (经典轨道根数)
    py::class_<OrbElem>(m, "OrbElem", "经典轨道根数(六根数)")
        .def(py::init<>(), "默认构造函数")
        .def(py::init<double, double, double, double, double, double>(),
             "根据六根数创建",
             py::arg("a"), py::arg("e"), py::arg("i"),
             py::arg("raan"), py::arg("argper"), py::arg("trueA"))
        .def_property("a", &OrbElem::getA, &OrbElem::a, "长半轴 [m]")
        .def_property("e", &OrbElem::getE, &OrbElem::e, "偏心率")
        .def_property("i", &OrbElem::getI, &OrbElem::i, "轨道倾角 [rad]")
        .def_property("raan", &OrbElem::getRAAN, &OrbElem::raan, "升交点赤经 [rad]")
        .def_property("argper", &OrbElem::getArgPer, &OrbElem::argper, "近地点幅角 [rad]")
        .def_property("trueA", &OrbElem::getTrueA, &OrbElem::trueA, "真近点角 [rad]")
        .def("getSMA", &OrbElem::getSMA, "获取长半轴")
        .def("getMeanMotion", &OrbElem::getMeanMotion, "获取平均角速度",
             py::arg("gm"))
        .def("__repr__", &OrbElem::toString);

    // ModOrbElem class (修正轨道根数)
    py::class_<ModOrbElem>(m, "ModOrbElem", "修正轨道根数(近地点半径形式)")
        .def(py::init<>(), "默认构造函数")
        .def(py::init<double, double, double, double, double, double>(),
             "根据六根数创建",
             py::arg("rp"), py::arg("e"), py::arg("i"),
             py::arg("raan"), py::arg("argper"), py::arg("trueA"))
        .def_property("rp", &ModOrbElem::getPeriRad, &ModOrbElem::rp, "近地点半径 [m]")
        .def_property("e", &ModOrbElem::getEcc, &ModOrbElem::e, "偏心率")
        .def_property("i", &ModOrbElem::getInc, &ModOrbElem::i, "轨道倾角 [rad]")
        .def_property("raan", &ModOrbElem::getRAAN, &ModOrbElem::raan, "升交点赤经 [rad]")
        .def_property("argper", &ModOrbElem::getArgPeri, &ModOrbElem::argper, "近地点幅角 [rad]")
        .def_property("trueA", &ModOrbElem::getTrueAnomaly, &ModOrbElem::trueA, "真近点角 [rad]")
        .def("getSMA", &ModOrbElem::getSMA, "获取长半轴")
        .def("getApoRad", &ModOrbElem::getApoRad, "获取远地点半径 [m]")
        .def("getApoAlt", &ModOrbElem::getApoAlt, "获取远地点高度 [m]",
             py::arg("bodyRadius"))
        .def("getPeriAlt", &ModOrbElem::getPeriAlt, "获取近地点高度 [m]",
             py::arg("bodyRadius"))
        .def("getPeriod", &ModOrbElem::getPeriod, "获取轨道周期 [s]",
             py::arg("gm"))
        .def("getMeanMotion", &ModOrbElem::getMeanMotion, "获取平均角速度 [rad/s]",
             py::arg("gm"))
        .def("getMeanAnomaly", &ModOrbElem::getMeanAnomaly, "获取平近点角 [rad]")
        .def("getEccAnomaly", &ModOrbElem::getEccAnomaly, "获取偏近点角 [rad]")
        .def("getArgLat", &ModOrbElem::getArgLat, "获取纬度幅角 [rad]")
        .def("__repr__", &ModOrbElem::toString);

    // 轨道参数转换函数
    m.def("aApoAltToPeriAlt", &aApoAltToPeriAlt,
          "远地点高度转近地点高度",
          py::arg("apogeeAlt"), py::arg("eccentricity"), py::arg("bodyRadius"));
    m.def("aPeriAltToApoAlt", &aPeriAltToApoAlt,
          "近地点高度转远地点高度",
          py::arg("perigeeAlt"), py::arg("eccentricity"), py::arg("bodyRadius"));
    m.def("aPeriAltToSMA", &aPeriAltToSMA,
          "近地点高度转长半轴",
          py::arg("perigeeAlt"), py::arg("eccentricity"), py::arg("bodyRadius"));
    m.def("aApoAltToSMA", &aApoAltToSMA,
          "远地点高度转长半轴",
          py::arg("apogeeAlt"), py::arg("eccentricity"), py::arg("bodyRadius"));
    m.def("aSMAToPeriAlt", &aSMAToPeriAlt,
          "长半轴转近地点高度",
          py::arg("sma"), py::arg("eccentricity"), py::arg("bodyRadius"));
    m.def("aSMAToApoAlt", &aSMAToApoAlt,
          "长半轴转远地点高度",
          py::arg("sma"), py::arg("eccentricity"), py::arg("bodyRadius"));
    m.def("aSMAToPeriod", &aSMAToPeriod,
          "长半轴转轨道周期",
          py::arg("sma"), py::arg("gm"));
    m.def("aPeriodToSMA", &aPeriodToSMA,
          "轨道周期转长半轴",
          py::arg("period"), py::arg("gm"));
    m.def("aSMAToMeanMotion", &aSMAToMeanMotion,
          "长半轴转平均角速度",
          py::arg("sma"), py::arg("gm"));
    m.def("aMeanMotionToSMA", &aMeanMotionToSMA,
          "平均角速度转长半轴",
          py::arg("meanMotion"), py::arg("gm"));
    m.def("aEccToMean", &aEccToMean,
          "偏近点角转平近点角",
          py::arg("eccAnomaly"), py::arg("eccentricity"));
    m.def("aEccToTrue", &aEccToTrue,
          "偏近点角转真近点角",
          py::arg("eccAnomaly"), py::arg("eccentricity"));
    m.def("aMeanToEcc", &aMeanToEcc,
          "平近点角转偏近点角",
          py::arg("meanAnomaly"), py::arg("eccentricity"));
    m.def("aMeanToTrue", &aMeanToTrue,
          "平近点角转真近点角",
          py::arg("meanAnomaly"), py::arg("eccentricity"));
    m.def("aTrueToMean", &aTrueToMean,
          "真近点角转平近点角",
          py::arg("trueAnomaly"), py::arg("eccentricity"));
    m.def("aTrueToEcc", &aTrueToEcc,
          "真近点角转偏近点角",
          py::arg("trueAnomaly"), py::arg("eccentricity"));
    m.def("aTrueToArgLat", &aTrueToArgLat,
          "真近点角转纬度幅角",
          py::arg("trueAnomaly"), py::arg("argPeri"));

    // 天文常数
    m.attr("GM_EARTH") = _AST kGM_Earth;
    m.attr("GM_SUN") = _AST kGM_Sun;
    m.attr("GM_MOON") = _AST kGM_Moon;
    m.attr("R_EARTH") = _AST kEarthRadius;
    m.attr("R_SUN") = _AST kSunRadius;
    m.attr("R_MOON") = _AST kMoonRadius;
    m.attr("J2_EARTH") = _AST kJ2_Earth;
}

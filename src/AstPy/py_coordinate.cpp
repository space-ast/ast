///
/// @file      py_coordinate.cpp
/// @brief     坐标系Python绑定
/// @details   坐标系转换、惯性系与地球固连系转换的pybind11绑定
/// @author    axel
/// @date      2026-04-09
/// @copyright 版权所有 (C) 2026-present, space-axel/astpy
///

#include "pybind_ast.h"
#include "AstCore/Coordinate/EarthFrame.hpp"
#include "AstCore/Coordinate/Frame/Frame.hpp"

using namespace ast;

void bind_coordinate(py::module& m)
{
    // ECI <-> ECF 转换函数
    m.def("aECIToECF", [](const Vector3d& vecECI, const TimePoint& tp) {
        Vector3d vecECF;
        aECIToECF(tp, vecECI, vecECF);
        return vecECF;
    }, "ECI转ECF坐标(位置)",
        py::arg("vecECI"), py::arg("tp"));

    m.def("aECFToECI", [](const Vector3d& vecECF, const TimePoint& tp) {
        Vector3d vecECI;
        aECFToECI(tp, vecECF, vecECI);
        return vecECI;
    }, "ECF转ECI坐标(位置)",
        py::arg("vecECF"), py::arg("tp"));

    m.def("aECIToECF", [](const Vector3d& posECI, const Vector3d& velECI, const TimePoint& tp) {
        Vector3d posECF, velECF;
        aECIToECF(tp, posECI, velECI, posECF, velECF);
        return std::make_tuple(posECF, velECF);
    }, "ECI转ECF坐标(位置速度)",
        py::arg("posECI"), py::arg("velECI"), py::arg("tp"));

    m.def("aECFToECI", [](const Vector3d& posECF, const Vector3d& velECF, const TimePoint& tp) {
        Vector3d posECI, velECI;
        aECFToECI(tp, posECF, velECF, posECI, velECI);
        return std::make_tuple(posECI, velECI);
    }, "ECF转ECI坐标(位置速度)",
        py::arg("posECF"), py::arg("velECF"), py::arg("tp"));

    // J2000 <-> ECF 转换函数
    m.def("aJ2000ToECF", [](const Vector3d& vecJ2000, const TimePoint& tp) {
        Vector3d vecECF;
        aJ2000ToECF(tp, vecJ2000, vecECF);
        return vecECF;
    }, "J2000转ECF坐标(位置)",
        py::arg("vecJ2000"), py::arg("tp"));

    m.def("aECFToJ2000", [](const Vector3d& vecECF, const TimePoint& tp) {
        Vector3d vecJ2000;
        aECFToJ2000(tp, vecECF, vecJ2000);
        return vecJ2000;
    }, "ECF转J2000坐标(位置)",
        py::arg("vecECF"), py::arg("tp"));

    m.def("aJ2000ToECF", [](const Vector3d& posJ2000, const Vector3d& velJ2000, const TimePoint& tp) {
        Vector3d posECF, velECF;
        aJ2000ToECF(tp, posJ2000, velJ2000, posECF, velECF);
        return std::make_tuple(posECF, velECF);
    }, "J2000转ECF坐标(位置速度)",
        py::arg("posJ2000"), py::arg("velJ2000"), py::arg("tp"));

    m.def("aECFToJ2000", [](const Vector3d& posECF, const Vector3d& velECF, const TimePoint& tp) {
        Vector3d posJ2000, velJ2000;
        aECFToJ2000(tp, posECF, velECF, posJ2000, velJ2000);
        return std::make_tuple(posJ2000, velJ2000);
    }, "ECF转J2000坐标(位置速度)",
        py::arg("posECF"), py::arg("velECF"), py::arg("tp"));

    // 获取旋转矩阵
    m.def("aECIToECFMatrix", [](const TimePoint& tp) {
        Matrix3d mat;
        aECIToECFMatrix(tp, mat);
        return mat;
    }, "获取ECI到ECF的旋转矩阵",
        py::arg("tp"));

    m.def("aJ2000ToECFMatrix", [](const TimePoint& tp) {
        Matrix3d mat;
        aJ2000ToECFMatrix(tp, mat);
        return mat;
    }, "获取J2000到ECF的旋转矩阵",
        py::arg("tp"));

    // 地心惯性系到地心地固系描述
    m.def("lonLatAltFromECF", [](const Vector3d& vecECF) {
        double lon, lat, alt;
        aECFToLonLatAlt(vecECF, lon, lat, alt);
        return py::make_tuple(lon, lat, alt);
    }, "ECF坐标转经纬高(大地坐标系)",
        py::arg("vecECF"));

    m.def("ECFFromLonLatAlt", [](double lon, double lat, double alt) {
        Vector3d vecECF;
        aLonLatAltToECF(lon, lat, alt, vecECF);
        return vecECF;
    }, "经纬高转ECF坐标(大地坐标系)",
        py::arg("lon"), py::arg("lat"), py::arg("alt"));

    // 角度单位转换
    m.def("aDegToRad", [](double deg) { return deg * kDegToRad; },
          "角度转弧度", py::arg("deg"));
    m.def("aRadToDeg", [](double rad) { return rad * kRadToDeg; },
          "弧度转角度", py::arg("rad"));
    m.def("aArcsecToRad", [](double arcsec) { return arcsec * kArcsecToRad; },
          "角秒转弧度", py::arg("arcsec"));
    m.def("aRadToArcsec", [](double rad) { return rad * kRadToArcsec; },
          "弧度转角秒", py::arg("rad"));
    m.def("aArcminToRad", [](double arcmin) { return arcmin * kArcminToRad; },
          "角分转弧度", py::arg("arcmin"));
    m.def("aRadToArcmin", [](double rad) { return rad * kRadToArcmin; },
          "弧度转角分", py::arg("rad"));

    // 长度单位转换
    m.def("aKmToM", [](double km) { return km * 1000.0; },
          "千米转米", py::arg("km"));
    m.def("aMToKm", [](double m) { return m / 1000.0; },
          "米转千米", py::arg("m"));
    m.def("aAUToM", [](double au) { return au * kAU; },
          "天文单位转米", py::arg("au"));
    m.def("aMToAU", [](double m) { return m / kAU; },
          "米转天文单位", py::arg("m"));

    // 角度常量
    m.attr("DEG_TO_RAD") = kDegToRad;
    m.attr("RAD_TO_DEG") = kRadToDeg;
    m.attr("ARCSEC_TO_RAD") = kArcsecToRad;
    m.attr("RAD_TO_ARCSEC") = kRadToArcsec;
}

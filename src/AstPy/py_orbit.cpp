/**
 * @file py_orbit.cpp
 * @brief 轨道力学 Python 绑定实现
 * @author SpaceAST Development Team
 * @version 1.0.0
 * @date 2024
 */

#include "py_orbit.h"
#include <AstCore/Orbit/OrbitDesigner.hpp>

namespace py = pybind11;
using namespace ast;

namespace astpy {

void bind_orbit(py::module& m) {
    // ============================================
    // OrbElem 类 - 经典轨道根数
    // ============================================
    py::class_<OrbElem>(m, "OrbElem", R"pbdoc(
        Classical Orbital Elements (Keplerian Elements).
        
        Orbital elements that define the shape and orientation of an orbit:
        - a: Semi-major axis [km]
        - e: Eccentricity [-]
        - i: Inclination [rad]
        - raan: Right Ascension of Ascending Node [rad]
        - argper: Argument of Perigee [rad]
        - trueA: True anomaly [rad]
    )pbdoc")
        // 构造函数
        .def(py::init<>(), "Create zero orbital elements")
        .def(py::init<double, double, double, double, double, double>(),
             py::arg("a"), py::arg("e"), py::arg("i"),
             py::arg("raan"), py::arg("argper"), py::arg("trueA"),
             "Create orbital elements from six parameters")
        
        // 属性 (直接访问)
        .def_readwrite("a", &OrbElem::a_,
            "Semi-major axis [km]")
        .def_readwrite("e", &OrbElem::e_,
            "Eccentricity [-]")
        .def_readwrite("i", &OrbElem::i_,
            "Inclination [rad]")
        .def_readwrite("raan", &OrbElem::raan_,
            "Right Ascension of Ascending Node [rad]")
        .def_readwrite("argper", &OrbElem::argper_,
            "Argument of Perigee [rad]")
        .def_readwrite("trueA", &OrbElem::trueA_,
            "True anomaly [rad]")
        
        // 访问器方法
        .def("getSMA", &OrbElem::getSMA, "Get semi-major axis")
        .def("getA", &OrbElem::getA, "Get semi-major axis (alias)")
        .def("getE", &OrbElem::getE, "Get eccentricity")
        .def("getI", &OrbElem::getI, "Get inclination")
        .def("getRAAN", &OrbElem::getRAAN, "Get RAAN")
        .def("getArgPer", &OrbElem::getArgPer, "Get argument of perigee")
        .def("getTrueA", &OrbElem::getTrueA, "Get true anomaly")
        .def("getMeanMotion", &OrbElem::getMeanMotion, py::arg("gm"),
             "Get mean motion [rad/s]")
        
        // 转换方法
        .def("toString", &OrbElem::toString,
             "Get string representation")
        .def("__repr__", [](const OrbElem& oe) {
            return "<OrbElem(a=" + std::to_string(oe.a_) +
                   ", e=" + std::to_string(oe.e_) +
                   ", i=" + std::to_string(oe.i_) +
                   ", raan=" + std::to_string(oe.raan_) +
                   ", argper=" + std::to_string(oe.argper_) +
                   ", trueA=" + std::to_string(oe.trueA_) + ")>";
        });

    // ============================================
    // CartState 类 - 笛卡尔状态
    // ============================================
    py::class_<CartState>(m, "CartState", R"pbdoc(
        Cartesian State (position and velocity).
        
        Represents the orbital state in Cartesian coordinates:
        position = [x, y, z] and velocity = [vx, vy, vz].
        Units are typically km and km/s.
    )pbdoc")
        // 构造函数
        .def(py::init<>(), "Create zero state")
        .def(py::init<const Vector3d&, const Vector3d&>(),
             py::arg("pos"), py::arg("vel"),
             "Create from position and velocity vectors")
        .def(py::init<double, double, double, double, double, double>(),
             py::arg("x"), py::arg("y"), py::arg("z"),
             py::arg("vx"), py::arg("vy"), py::arg("vz"),
             "Create from six scalar components")
        
        // 属性
        .def_property("x", &CartState::x, &CartState::x,
            "X position [km]")
        .def_property("y", &CartState::y, &CartState::y,
            "Y position [km]")
        .def_property("z", &CartState::z, &CartState::z,
            "Z position [km]")
        .def_property("vx", &CartState::vx, &CartState::vx,
            "X velocity [km/s]")
        .def_property("vy", &CartState::vy, &CartState::vy,
            "Y velocity [km/s]")
        .def_property("vz", &CartState::vz, &CartState::vz,
            "Z velocity [km/s]")
        .def_property_readonly("pos", &CartState::pos,
            "Position as Vector3d")
        .def_property_readonly("vel", &CartState::vel,
            "Velocity as Vector3d")
        
        // 静态方法
        .def_static("Zero", &CartState::Zero,
            "Create zero state")
        
        .def("__repr__", [](const CartState& cs) {
            return "<CartState(pos=(" + std::to_string(cs.x()) + ", " +
                   std::to_string(cs.y()) + ", " +
                   std::to_string(cs.z()) + "), vel=(" +
                   std::to_string(cs.vx()) + ", " +
                   std::to_string(cs.vy()) + ", " +
                   std::to_string(cs.vz()) + "))>";
        });

    // ============================================
    // TwoBody 类 - 二体问题
    // ============================================
    py::class_<TwoBody, SPtr<TwoBody>>(m, "TwoBody", R"pbdoc(
        Two-body orbital propagator.
        
        Propagates spacecraft state using the two-body (Kepler) equation.
        This is an idealized propagator that only considers central body gravity.
    )pbdoc")
        .def(py::init<double>(), py::arg("gm"),
            "Create two-body propagator with gravitational parameter [km^3/s^2]")
        
        .def_property_readonly("gm", &TwoBody::GetGM,
            "Gravitational parameter")
        
        // 静态方法
        .def_static("propagate", &aTwoBodyProp,
            py::arg("state"), py::arg("dt"), py::arg("gm"),
            R"pbdoc(
            Propagate state using two-body dynamics.
            
            Args:
                state: Initial Cartesian state
                dt: Time step in seconds
                gm: Gravitational parameter [km^3/s^2]
            
            Returns:
                Propagated Cartesian state
            )pbdoc");

    // ============================================
    // 轨道转换函数
    // ============================================
    m.def("rv2moe", &rv2moe,
        py::arg("rv"), py::arg("gm"),
        R"pbdoc(
        Convert position/velocity to mean orbital elements.
        
        Args:
            rv: Cartesian state [position, velocity]
            gm: Gravitational parameter [km^3/s^2]
        
        Returns:
            Orbital elements
        )pbdoc",
        py::return_value_policy::reference_internal);

    m.def("moe2rv", &moe2rv,
        py::arg("moe"), py::arg("gm"),
        R"pbdoc(
        Convert mean orbital elements to position/velocity.
        
        Args:
            moe: Orbital elements
            gm: Gravitational parameter [km^3/s^2]
        
        Returns:
            Cartesian state
        )pbdoc");

    m.def("rv2coe", &rv2coe,
        py::arg("rv"), py::arg("gm"),
        R"pbdoc(
        Convert position/velocity to classical orbital elements.
        
        Args:
            rv: Cartesian state [position, velocity]
            gm: Gravitational parameter [km^3/s^2]
        
        Returns:
            Classical orbital elements
        )pbdoc");

    m.def("coe2rv", &coe2rv,
        py::arg("coe"), py::arg("gm"),
        R"pbdoc(
        Convert classical orbital elements to position/velocity.
        
        Args:
            coe: Classical orbital elements
            gm: Gravitational parameter [km^3/s^2]
        
        Returns:
            Cartesian state
        )pbdoc");

    // ============================================
    // 轨道设计器
    // ============================================
    py::class_<SimpleOrbitDesigner, SPtr<SimpleOrbitDesigner>>(m, "SimpleOrbitDesigner",
        R"pbdoc(
        Simple orbit designer for basic orbital element calculation.
    )pbdoc")
        .def(py::init<double, double, double>(),
             py::arg("a"), py::arg("e"), py::arg("i"),
             "Create with semi-major axis, eccentricity, inclination")
        .def("getOrbElem", &SimpleOrbitDesigner::GetOrbElem,
             "Get orbital elements")
        .def("getCartState", &SimpleOrbitDesigner::GetCartState,
             py::arg("trueAnomaly"), py::arg("gm"),
             "Get Cartesian state at given true anomaly");

    py::class_<CircularOrbitDesigner, SPtr<CircularOrbitDesigner>>(m, "CircularOrbitDesigner",
        R"pbdoc(
        Circular orbit designer.
        
        Creates orbits with zero eccentricity using radius and inclination.
    )pbdoc")
        .def(py::init<double, double>(),
             py::arg("radius"), py::arg("i"),
             "Create with orbital radius and inclination");

    py::class_<StationaryOrbitDesigner, SPtr<StationaryOrbitDesigner>>(m, "StationaryOrbitDesigner",
        R"pbdoc(
        Geostationary orbit designer.
        
        Creates geostationary orbits (period = 86164 seconds).
    )pbdoc")
        .def(py::init<double>(),
             py::arg("altitude"),
             "Create geostationary orbit at given altitude");
}

}  // namespace astpy

/**
 * @file py_body.cpp
 * @brief 天体 Python 绑定实现
 * @author SpaceAST Development Team
 * @version 1.0.0
 * @date 2024
 */

#include "py_body.h"
#include <AstCore/SolarSystem/CelestialBody.hpp>
#include <AstCore/SolarSystem/SolarSystem.hpp>
#include <AstCore/SolarSystem/Planet.hpp>

namespace py = pybind11;
using namespace ast;

namespace astpy {

void bind_body(py::module& m) {
    // ============================================
    // SolarSystem 类
    // ============================================
    py::class_<SolarSystem, SPtr<SolarSystem>>(m, "SolarSystem", R"pbdoc(
        Solar System class for accessing planetary ephemerides.
        
        Provides access to Sun and planet data based on JPL ephemerides.
    )pbdoc")
        .def_static("Get", &SolarSystem::Get,
            R"pbdoc(
            Get the global SolarSystem instance.
            
            Returns:
                Shared pointer to SolarSystem
            )pbdoc")
        .def("getSun", &SolarSystem::GetSun,
            "Get the Sun celestial body")
        .def("getPlanet", &SolarSystem::GetPlanet, py::arg("id"),
            "Get a planet by ID")
        .def("getEarth", &SolarSystem::GetEarth,
            "Get the Earth celestial body")
        .def("getMoon", &SolarSystem::GetMoon,
            "Get the Moon celestial body")
        .def("getMercury", &SolarSystem::GetMercury,
            "Get Mercury")
        .def("getVenus", &SolarSystem::GetVenus,
            "Get Venus")
        .def("getMars", &SolarSystem::GetMars,
            "Get Mars")
        .def("getJupiter", &SolarSystem::GetJupiter,
            "Get Jupiter")
        .def("getSaturn", &SolarSystem::GetSaturn,
            "Get Saturn")
        .def("getUranus", &SolarSystem::GetUranus,
            "Get Uranus")
        .def("getNeptune", &SolarSystem::GetNeptune,
            "Get Neptune");

    // ============================================
    // CelestialBody 类
    // ============================================
    py::class_<CelestialBody, SPtr<CelestialBody>>(m, "CelestialBody", R"pbdoc(
        Celestial body class representing planets, moons, etc.
        
        Contains physical properties and ephemeris data for celestial bodies.
    )pbdoc")
        .def("getName", &CelestialBody::GetName,
            "Get the body name")
        .def("getID", &CelestialBody::GetID,
            "Get the body ID")
        .def("getGM", &CelestialBody::GetGM,
            "Get gravitational parameter [km^3/s^2]")
        .def("getRadius", &CelestialBody::GetRadius,
            "Get mean radius [km]")
        .def("getMass", &CelestialBody::GetMass,
            "Get mass [kg]")
        .def("getShape", &CelestialBody::GetShape,
            "Get the body shape")
        .def("getOrientation", &CelestialBody::GetOrientation,
            "Get the body orientation/rotation model")
        .def("getFrame", &CelestialBody::GetFrame,
            "Get the body-fixed frame")
        .def_property_readonly("id", &CelestialBody::GetID,
            "Body ID")
        .def_property_readonly("name", &CelestialBody::GetName,
            "Body name")
        .def_property_readonly("gm", &CelestialBody::GetGM,
            "Gravitational parameter [km^3/s^2]")
        .def_property_readonly("radius", &CelestialBody::GetRadius,
            "Mean radius [km]")
        .def_property_readonly("mass", &CelestialBody::GetMass,
            "Mass [kg]")
        
        .def("__repr__", [](CelestialBody& body) {
            return "<CelestialBody: " + body.GetName() + ">";
        });

    // ============================================
    // Planet 类
    // ============================================
    py::class_<Planet, CelestialBody, SPtr<Planet>>(m, "Planet", R"pbdoc(
        Planet class for major solar system planets.
    )pbdoc");

    // ============================================
    // BodyShape 类
    // ============================================
    py::class_<BodyShape, SPtr<BodyShape>>(m, "BodyShape", R"pbdoc(
        Body shape base class.
    )pbdoc");

    py::class_<SphereShape, BodyShape, SPtr<SphereShape>>(m, "SphereShape", R"pbdoc(
        Spherical body shape.
    )pbdoc")
        .def(py::init<double>(), py::arg("radius"),
            "Create spherical shape with given radius");

    py::class_<EllipsoidShape, BodyShape, SPtr<EllipsoidShape>>(m, "EllipsoidShape", R"pbdoc(
        Ellipsoidal body shape.
    )pbdoc")
        .def(py::init<double, double, double>(),
             py::arg("a"), py::arg("b"), py::arg("c"),
            "Create ellipsoid with semi-axes a, b, c");

    // ============================================
    // BodyOrientation 类
    // ============================================
    py::class_<BodyOrientation, SPtr<BodyOrientation>>(m, "BodyOrientation", R"pbdoc(
        Body orientation/rotation model.
    )pbdoc");

    py::class_<EarthOrientation, BodyOrientation, SPtr<EarthOrientation>>(m, "EarthOrientation",
        R"pbdoc(
        Earth orientation model including precession, nutation, and pole motion.
    )pbdoc");

    // ============================================
    // 行星ID枚举
    // ============================================
    m.attr("BODY_SUN") = py::cast(static_cast<int>(SolarSystem::BodiesID::SUN));
    m.attr("BODY_MERCURY") = py::cast(static_cast<int>(SolarSystem::BodiesID::MERCURY));
    m.attr("BODY_VENUS") = py::cast(static_cast<int>(SolarSystem::BodiesID::VENUS));
    m.attr("BODY_EARTH") = py::cast(static_cast<int>(SolarSystem::BodiesID::EARTH));
    m.attr("BODY_MARS") = py::cast(static_cast<int>(SolarSystem::BodiesID::MARS));
    m.attr("BODY_JUPITER") = py::cast(static_cast<int>(SolarSystem::BodiesID::JUPITER));
    m.attr("BODY_SATURN") = py::cast(static_cast<int>(SolarSystem::BodiesID::SATURN));
    m.attr("BODY_URANUS") = py::cast(static_cast<int>(SolarSystem::BodiesID::URANUS));
    m.attr("BODY_NEPTUNE") = py::cast(static_cast<int>(SolarSystem::BodiesID::NEPTUNE));
    m.attr("BODY_MOON") = py::cast(static_cast<int>(SolarSystem::BodiesID::MOON));
    m.attr("BODY_PLUTO") = py::cast(static_cast<int>(SolarSystem::BodiesID::PLUTO));

    // ============================================
    // 物理常量
    // ============================================
    m.attr("GM_EARTH") = py::cast(3.986004418e5);  // km^3/s^2
    m.attr("GM_SUN") = py::cast(1.32712440018e11);  // km^3/s^2
    m.attr("GM_MOON") = py::cast(4902.799);  // km^3/s^2
    m.attr("R_EARTH") = py::cast(6378.137);  // km (equatorial)
    m.attr("R_SUN") = py::cast(696340.0);  // km
    m.attr("AU") = py::cast(149597870.7);  // km
}

}  // namespace astpy

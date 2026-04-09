/**
 * @file py_quantity.cpp
 * @brief 数量值（带单位）Python 绑定实现
 * @author SpaceAST Development Team
 * @version 1.0.0
 * @date 2024
 */

#include "py_quantity.h"
#include <AstUtil/Quantity.hpp>

namespace py = pybind11;
using namespace ast;

namespace astpy {

void bind_quantity(py::module& m) {
    // ============================================
    // Unit 类
    // ============================================
    py::class_<Unit>(m, "Unit", R"pbdoc(
        Unit class for physical quantities.
        
        Units are used to represent physical quantities with their
        associated units for type safety.
    )pbdoc")
        .def(py::init<>(), "Create a dimensionless unit")
        .def(py::init<const std::string&>(), py::arg("name"),
            "Create unit from name")
        .def("getName", &Unit::GetName,
            "Get the unit name")
        .def("__repr__", [](const Unit& u) {
            return "<Unit: " + u.GetName() + ">";
        });

    // ============================================
    // Quantity 类
    // ============================================
    py::class_<Quantity>(m, "Quantity", R"pbdoc(
        Quantity class representing a value with units.
        
        A Quantity combines a numerical value with its associated unit,
        providing type safety for physical calculations.
    )pbdoc")
        .def(py::init<>(), "Create a zero quantity")
        .def(py::init<double>(), py::arg("value"),
            "Create from value with default unit")
        .def(py::init<double, const Unit&>(), py::arg("value"), py::arg("unit"),
            "Create from value and unit")
        
        .def_property("value", &Quantity::Value, &Quantity::SetValue,
            "Get/set the numerical value")
        .def_property("unit", &Quantity::GetUnit,
            "Get the unit")
        
        .def("inUnit", &Quantity::InUnit, py::arg("unit"),
            "Get value in specified unit")
        .def("inSI", &Quantity::InSI,
            "Get value in SI units")
        
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self * double())
        .def(double() * py::self)
        .def(py::self / double())
        
        .def("__repr__", [](const Quantity& q) {
            return "<Quantity(" + std::to_string(q.Value()) + " " +
                   q.GetUnit().GetName() + ")>";
        });

    // 常用单位
    m.attr("UNIT_ONE") = py::cast(Unit::One());
    m.attr("UNIT_METER") = py::cast(Unit::Meter());
    m.attr("UNIT_KILOMETER") = py::cast(Unit::Kilometer());
    m.attr("UNIT_SECOND") = py::cast(Unit::Second());
    m.attr("UNIT_MINUTE") = py::cast(Unit::Minute());
    m.attr("UNIT_HOUR") = py::cast(Unit::Hour());
    m.attr("UNIT_DAY") = py::cast(Unit::Day());
    m.attr("UNIT_KILOGRAM") = py::cast(Unit::Kilogram());
    m.attr("UNIT_RADIAN") = py::cast(Unit::Radian());
    m.attr("UNIT_DEGREE") = py::cast(Unit::Degree());
    m.attr("UNIT_ARCSEC") = py::cast(Unit::Arcsec());
    m.attr("UNIT_KM_S") = py::cast(Unit::KmS());
    m.attr("UNIT_KM3_S2") = py::cast(Unit::Km3S2());
}

}  // namespace astpy

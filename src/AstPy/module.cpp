///
/// @file      module.cpp
/// @brief     AstPy Python绑定模块入口
/// @details   使用pybind11实现C++到Python的绑定
/// @author    axel
/// @date      2026-04-09
/// @copyright 版权所有 (C) 2026-present, space-axel/astpy
///
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 许可证全文请见：http://www.apache.org/licenses/LICENSE-2.0
///

#include <pybind11/pybind11.h>

#include "pybind_ast.h"

namespace py = pybind11;

PYBIND11_MODULE(ast, m) {
    m.doc() = R"pbdoc(
        AstPy - SpaceAST Python Bindings
        --------------------------------
        
        AstPy是SpaceAST航天任务分析库的Python接口，
        提供了时间系统、轨道力学、坐标系转换等航天计算功能。
        
        主要模块：
            - time: 时间系统（Date, DateTime, JulianDate, TimePoint等）
            - math: 数学类型（Vector3d, Matrix3d等）
            - orbit: 轨道力学（六根数、轨道参数等）
            - coord: 坐标系转换（ECI/ECF, J2000等）
        
        使用示例：
            >>> import ast
            >>> from ast import Date, Vector3d
            >>> date = Date(2024, 1, 1)
            >>> vec = Vector3d(1.0, 2.0, 3.0)
            >>> print(vec.norm())
        
        更多信息请访问：https://gitee.com/space-axel/astpy
    )pbdoc";

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "0.0.1";
#endif

    // 子模块: time
    py::module time_mod = m.def_submodule("time", "时间系统模块");
    bind_time(time_mod);

    // 子模块: math
    py::module math_mod = m.def_submodule("math", "数学类型模块");
    bind_math(math_mod);

    // 子模块: orbit
    py::module orbit_mod = m.def_submodule("orbit", "轨道力学模块");
    bind_orbit(orbit_mod);

    // 子模块: coord
    py::module coord_mod = m.def_submodule("coord", "坐标系转换模块");
    bind_coordinate(coord_mod);

    // 常用导入快捷方式
    m.def("Date", []() { return py::type::of<ast::Date>(); }, "Date类");
    m.def("DateTime", []() { return py::type::of<ast::DateTime>(); }, "DateTime类");
    m.def("JulianDate", []() { return py::type::of<ast::JulianDate>(); }, "JulianDate类");
    m.def("TimePoint", []() { return py::type::of<ast::TimePoint>(); }, "TimePoint类");
    m.def("Vector3d", []() { return py::type::of<ast::Vector3d>(); }, "Vector3d类");
    m.def("Matrix3d", []() { return py::type::of<ast::Matrix3d>(); }, "Matrix3d类");
    m.def("OrbElem", []() { return py::type::of<ast::OrbElem>(); }, "OrbElem类");
    m.def("ModOrbElem", []() { return py::type::of<ast::ModOrbElem>(); }, "ModOrbElem类");
    m.def("CartState", []() { return py::type::of<ast::CartState>(); }, "CartState类");
}

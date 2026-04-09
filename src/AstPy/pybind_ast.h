///
/// @file      pybind_ast.h
/// @brief     pybind11 Python绑定声明
/// @details   定义pybind11绑定的模块、子模块结构
/// @author    axel
/// @date      2026-04-09
/// @copyright 版权所有 (C) 2026-present, space-axel/astpy
///

#pragma once

#include <pybind11/pybind11.h>

namespace py = pybind11;

// 子模块声明
void bind_time(py::module& m);
void bind_math(py::module& m);
void bind_orbit(py::module& m);
void bind_coordinate(py::module& m);

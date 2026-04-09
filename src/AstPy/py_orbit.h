/**
 * @file py_orbit.h
 * @brief 轨道力学 Python 绑定声明
 * @author SpaceAST Development Team
 * @version 1.0.0
 * @date 2024
 */

#ifndef ASTPY_ORBIT_H
#define ASTPY_ORBIT_H

#include "pybind11.h"

namespace astpy {

/**
 * @brief 注册轨道力学相关的 Python 绑定
 * @param m pybind11 模块
 */
void bind_orbit(py::module& m);

}  // namespace astpy

#endif  // ASTPY_ORBIT_H

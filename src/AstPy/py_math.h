/**
 * @file py_math.h
 * @brief 数学类型 Python 绑定声明
 * @author SpaceAST Development Team
 * @version 1.0.0
 * @date 2024
 */

#ifndef ASTPY_MATH_H
#define ASTPY_MATH_H

#include "pybind11.h"

namespace astpy {

/**
 * @brief 注册数学类型相关的 Python 绑定
 * @param m pybind11 模块
 */
void bind_math(py::module& m);

}  // namespace astpy

#endif  // ASTPY_MATH_H

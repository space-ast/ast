/**
 * @file py_time.h
 * @brief 时间系统 Python 绑定声明
 * @author SpaceAST Development Team
 * @version 1.0.0
 * @date 2024
 */

#ifndef ASTPY_TIME_H
#define ASTPY_TIME_H

#include "pybind11.h"

namespace astpy {

/**
 * @brief 注册时间系统相关的 Python 绑定
 * @param m pybind11 模块
 */
void bind_time(py::module& m);

}  // namespace astpy

#endif  // ASTPY_TIME_H

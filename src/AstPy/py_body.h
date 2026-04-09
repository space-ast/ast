/**
 * @file py_body.h
 * @brief 天体 Python 绑定声明
 * @author SpaceAST Development Team
 * @version 1.0.0
 * @date 2024
 */

#ifndef ASTPY_BODY_H
#define ASTPY_BODY_H

#include "pybind11.h"

namespace astpy {

/**
 * @brief 注册天体相关的 Python 绑定
 * @param m pybind11 模块
 */
void bind_body(py::module& m);

}  // namespace astpy

#endif  // ASTPY_BODY_H

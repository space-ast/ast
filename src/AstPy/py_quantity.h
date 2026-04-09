/**
 * @file py_quantity.h
 * @brief 数量值（带单位）Python 绑定声明
 * @author SpaceAST Development Team
 * @version 1.0.0
 * @date 2024
 */

#ifndef ASTPY_QUANTITY_H
#define ASTPY_QUANTITY_H

#include "pybind11.h"

namespace astpy {

/**
 * @brief 注册数量值相关的 Python 绑定
 * @param m pybind11 模块
 */
void bind_quantity(py::module& m);

}  // namespace astpy

#endif  // ASTPY_QUANTITY_H

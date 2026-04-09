/**
 * @file py_frame.h
 * @brief 坐标系 Python 绑定声明
 * @author SpaceAST Development Team
 * @version 1.0.0
 * @date 2024
 */

#ifndef ASTPY_FRAME_H
#define ASTPY_FRAME_H

#include "pybind11.h"

namespace astpy {

/**
 * @brief 注册坐标系相关的 Python 绑定
 * @param m pybind11 模块
 */
void bind_frame(py::module& m);

}  // namespace astpy

#endif  // ASTPY_FRAME_H

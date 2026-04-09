/**
 * @file pybind11.h
 * @brief pybind11 Python 绑定基础头文件
 * @author SpaceAST Development Team
 * @version 1.0.0
 * @date 2024
 *
 * 本文件提供 pybind11 绑定的辅助宏定义和工具函数
 */

#ifndef ASTPY_PYBIND11_H
#define ASTPY_PYBIND11_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/eigen.h>
#include <pybind11/chrono.h>

#include <exception>
#include <string>
#include <sstream>

namespace py = pybind11;

#include <AstCore.hpp>
#include <AstMath.hpp>

/**
 * @brief 命名空间别名
 */
namespace astpy {

/**
 * @brief 异常处理辅助宏
 */
#define ASTPY_TRY try {

#define ASTPY_CATCH \
    } catch (const std::exception& e) { \
        PyErr_SetString(PyExc_RuntimeError, e.what()); \
        return nullptr; \
    }

/**
 * @brief 创建只读属性
 * @param name 属性名
 * @param getter getter函数
 */
#define ASTPY_RO_PROPERTY(name, getter) \
    .def_property_readonly(#name, getter, #name)

/**
 * @brief 创建读写属性
 * @param name 属性名
 * @param getter getter函数
 * @param setter setter函数
 */
#define ASTPY_RW_PROPERTY(name, getter, setter) \
    .def_property(#name, getter, setter, #name)

/**
 * @brief 注册智能指针类型
 * @param cls pybind11 class_
 * @param Type C++类型名
 */
template<typename T>
void register_smart_ptr(pybind11::class_<T>& cls) {
    // 默认实现为空，特殊类型需要特化
}

/**
 * @brief Vector3d 辅助函数
 */
inline py::list vector3d_to_list(const ast::Vector3d& v) {
    py::list lst;
    lst.append(v.x());
    lst.append(v.y());
    lst.append(v.z());
    return lst;
}

/**
 * @brief list 转 Vector3d
 */
inline ast::Vector3d list_to_vector3d(const py::list& lst) {
    if (py::len(lst) != 3) {
        throw std::invalid_argument("Vector3d requires exactly 3 elements");
    }
    return ast::Vector3d(
        py::cast<double>(lst[0]),
        py::cast<double>(lst[1]),
        py::cast<double>(lst[2])
    );
}

/**
 * @brief Matrix3d 辅助函数 - 转换为 Python 列表
 */
inline py::list matrix3d_to_list(const ast::Matrix3d& m) {
    py::list result;
    for (int i = 0; i < 3; ++i) {
        py::list row;
        for (int j = 0; j < 3; ++j) {
            row.append(m(i, j));
        }
        result.append(row);
    }
    return result;
}

/**
 * @brief Quaternion 辅助函数 - 转换为 Python 列表
 */
inline py::list quaternion_to_list(const ast::Quaternion& q) {
    py::list lst;
    lst.append(q.w());  // scalar part first (Hamilton convention)
    lst.append(q.x());
    lst.append(q.y());
    lst.append(q.z());
    return lst;
}

/**
 * @brief 格式化时间为字符串
 */
inline std::string format_time(const std::string& time_str, int precision = 6) {
    std::ostringstream oss;
    oss << time_str;
    return oss.str();
}

}  // namespace astpy

#endif  // ASTPY_PYBIND11_H

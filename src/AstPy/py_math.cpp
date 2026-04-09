///
/// @file      py_math.cpp
/// @brief     数学类型Python绑定
/// @details   Vector, Matrix等数学类型的pybind11绑定
/// @author    axel
/// @date      2026-04-09
/// @copyright 版权所有 (C) 2026-present, space-axel/astpy
///

#include "pybind_ast.h"
#include "AstMath/Array/Vector.hpp"
#include "AstMath/Array/Matrix.hpp"

using namespace ast;

void bind_math(py::module& m)
{
    // Vector3d class
    py::class_<Vector3d>(m, "Vector3d", "3维向量")
        .def(py::init<>(), "默认构造函数，创建零向量")
        .def(py::init<double, double, double>(), 
             "根据xyz分量创建向量",
             py::arg("x"), py::arg("y"), py::arg("z"))
        .def_static("Zero", &Vector3d::Zero, "创建零向量")
        .def_static("UnitX", &Vector3d::UnitX, "创建X轴单位向量")
        .def_static("UnitY", &Vector3d::UnitY, "创建Y轴单位向量")
        .def_static("UnitZ", &Vector3d::UnitZ, "创建Z轴单位向量")
        .def_property("x", &Vector3d::x, &Vector3d::x, "X分量")
        .def_property("y", &Vector3d::y, &Vector3d::y, "Y分量")
        .def_property("z", &Vector3d::z, &Vector3d::z, "Z分量")
        .def("norm", &Vector3d::norm, "向量的欧几里得范数(长度)")
        .def("squaredNorm", &Vector3d::squaredNorm, "向量的平方范数")
        .def("normalize", &Vector3d::normalize, "归一化向量，返回原长度")
        .def("normalized", &Vector3d::normalized, "返回归一化后的向量副本")
        .def("cross", &Vector3d::cross, "叉乘", py::arg("other"))
        .def("dot", &Vector3d::dot, "点乘", py::arg("other"))
        .def("setZero", &Vector3d::setZero, "设置为零向量")
        .def("__getitem__", [](const Vector3d& v, size_t i) {
            if (i >= 3) throw py::index_error();
            return v(i);
        })
        .def("__setitem__", [](Vector3d& v, size_t i, double val) {
            if (i >= 3) throw py::index_error();
            v(i) = val;
        })
        .def("__len__", [](const Vector3d&) { return 3; })
        .def("__iter__", [](const Vector3d& v) {
            return py::make_iterator(&v(0), &v(0) + 3);
        }, py::keep_alive<0, 1>())
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self * double())
        .def(double() * py::self)
        .def(-py::self)
        .def("__str__", [](const Vector3d& v) {
            return "(" + std::to_string(v.x()) + ", " + 
                         std::to_string(v.y()) + ", " + 
                         std::to_string(v.z()) + ")";
        })
        .def("__repr__", [](const Vector3d& v) {
            return "Vector3d(" + std::to_string(v.x()) + ", " + 
                              std::to_string(v.y()) + ", " + 
                              std::to_string(v.z()) + ")";
        });

    // VectorXd class
    py::class_<VectorXd>(m, "VectorXd", "可变维度向量")
        .def(py::init<>(), "默认构造函数")
        .def(py::init<size_t>(), "创建指定大小的向量", py::arg("size"))
        .def("resize", &VectorXd::resize, "调整大小", py::arg("size"))
        .def("setZero", &VectorXd::setZero, "设置为零向量")
        .def_property_readonly("size", &VectorXd::size, "向量大小")
        .def("__getitem__", [](const VectorXd& v, size_t i) {
            if (i >= v.size()) throw py::index_error();
            return v(i);
        })
        .def("__setitem__", [](VectorXd& v, size_t i, double val) {
            if (i >= v.size()) throw py::index_error();
            v(i) = val;
        })
        .def("__len__", &VectorXd::size)
        .def("__iter__", [](const VectorXd& v) {
            return py::make_iterator(v.begin(), v.end());
        }, py::keep_alive<0, 1>());

    // Matrix3d class
    py::class_<Matrix3d>(m, "Matrix3d", "3x3矩阵")
        .def(py::init<>(), "默认构造函数")
        .def(py::init<double, double, double,
              double, double, double,
              double, double, double>(),
             "按行创建矩阵",
             py::arg("m00"), py::arg("m01"), py::arg("m02"),
             py::arg("m10"), py::arg("m11"), py::arg("m12"),
             py::arg("m20"), py::arg("m21"), py::arg("m22"))
        .def_static("Identity", []() { 
            Matrix3d m; m.setIdentity(); return m; 
        }, "创建单位矩阵")
        .def_static("Zero", []() { 
            Matrix3d m; m.setZero(); return m; 
        }, "创建零矩阵")
        .def("transpose", &Matrix3d::transpose, "转置")
        .def("inverse", &Matrix3d::inverse, "求逆")
        .def("determinant", &Matrix3d::determinant, "行列式")
        .def("trace", &Matrix3d::trace, "迹")
        .def("__getitem__", [](const Matrix3d& m, std::pair<size_t, size_t> idx) {
            if (idx.first >= 3 || idx.second >= 3) throw py::index_error();
            return m(idx.first, idx.second);
        })
        .def("__setitem__", [](Matrix3d& m, std::pair<size_t, size_t> idx, double val) {
            if (idx.first >= 3 || idx.second >= 3) throw py::index_error();
            m(idx.first, idx.second) = val;
        })
        .def("row", [](const Matrix3d& m, size_t i) { 
            if (i >= 3) throw py::index_error();
            return py::make_tuple(m(i,0), m(i,1), m(i,2)); 
        })
        .def("col", [](const Matrix3d& m, size_t j) { 
            if (j >= 3) throw py::index_error();
            return py::make_tuple(m(0,j), m(1,j), m(2,j)); 
        });

    // MatrixXd class
    py::class_<MatrixXd>(m, "MatrixXd", "可变维度矩阵")
        .def(py::init<>(), "默认构造函数")
        .def(py::init<size_t, size_t>(), "创建指定大小的矩阵", 
             py::arg("row"), py::arg("col"))
        .def("resize", &MatrixXd::resize, "调整大小", py::arg("row"), py::arg("col"))
        .def("setZero", &MatrixXd::setZero, "设置为零矩阵")
        .def_property_readonly("row", &MatrixXd::row, "行数")
        .def_property_readonly("col", &MatrixXd::col, "列数")
        .def_property_readonly("size", &MatrixXd::size, "元素总数")
        .def("__getitem__", [](const MatrixXd& m, std::pair<size_t, size_t> idx) {
            if (idx.first >= m.row() || idx.second >= m.col()) throw py::index_error();
            return m(idx.first, idx.second);
        })
        .def("__setitem__", [](MatrixXd& m, std::pair<size_t, size_t> idx, double val) {
            if (idx.first >= m.row() || idx.second >= m.col()) throw py::index_error();
            m(idx.first, idx.second) = val;
        });

    // Vector3d math operators
    m.def("dot", [](const Vector3d& a, const Vector3d& b) { return a.dot(b); },
          "向量点乘", py::arg("a"), py::arg("b"));
    m.def("cross", [](const Vector3d& a, const Vector3d& b) { return a.cross(b); },
          "向量叉乘", py::arg("a"), py::arg("b"));
    m.def("norm", [](const Vector3d& v) { return v.norm(); },
          "向量模长", py::arg("v"));
    m.def("normalize", [](const Vector3d& v) { return v.normalized(); },
          "向量归一化", py::arg("v"));
}

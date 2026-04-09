/**
 * @file py_math.cpp
 * @brief 数学类型 Python 绑定实现
 * @author SpaceAST Development Team
 * @version 1.0.0
 * @date 2024
 */

#include "py_math.h"

namespace py = pybind11;
using namespace ast;

namespace astpy {

void bind_math(py::module& m) {
    // ============================================
    // Vector3d 类
    // ============================================
    py::class_<Vector3d>(m, "Vector3d", R"pbdoc(
        3D Vector class for position, velocity, etc.
        
        A Vector3d represents a three-dimensional vector with x, y, z components.
        It supports various mathematical operations like addition, subtraction,
        dot product, cross product, etc.
    )pbdoc")
        // 构造函数
        .def(py::init<>(), "Create a zero vector")
        .def(py::init<double, double, double>(),
             py::arg("x"), py::arg("y"), py::arg("z"),
             "Create vector from x, y, z components")
        .def(py::init([](py::list lst) {
            if (py::len(lst) != 3) {
                throw std::invalid_argument("Vector3d requires exactly 3 elements");
            }
            return Vector3d(
                py::cast<double>(lst[0]),
                py::cast<double>(lst[1]),
                py::cast<double>(lst[2])
            );
        }), py::arg("values"), "Create vector from list/tuple of 3 values")
        
        // 访问器
        .def_property("x", &Vector3d::x, &Vector3d::setX,
            "X component")
        .def_property("y", &Vector3d::y, &Vector3d::setY,
            "Y component")
        .def_property("z", &Vector3d::z, &Vector3d::setZ,
            "Z component")
        .def("__getitem__", [](const Vector3d& v, int idx) {
            if (idx < 0) idx += 3;
            if (idx < 0 || idx >= 3)
                throw py::index_error("Vector3d index out of range");
            return v[idx];
        })
        .def("__setitem__", [](Vector3d& v, int idx, double val) {
            if (idx < 0) idx += 3;
            if (idx < 0 || idx >= 3)
                throw py::index_error("Vector3d index out of range");
            v[idx] = val;
        })
        
        // 属性
        .def_property_readonly("norm", &Vector3d::norm,
            "Euclidean norm (length) of the vector")
        .def_property_readonly("squaredNorm", &Vector3d::squaredNorm,
            "Squared norm of the vector")
        
        // 数学运算
        .def("normalized", &Vector3d::normalized,
             "Return a normalized copy of the vector")
        .def("normalize", &Vector3d::normalize,
             "Normalize this vector in-place")
        .def("dot", &Vector3d::dot, py::arg("other"),
             "Dot product with another vector")
        .def("cross", &Vector3d::cross, py::arg("other"),
             "Cross product with another vector")
        .def("angleTo", &Vector3d::angleTo, py::arg("other"),
             "Angle to another vector in radians")
        .def("distanceTo", &Vector3d::distanceTo, py::arg("other"),
             "Distance to another vector")
        .def("isZero", &Vector3d::IsZero,
             "Check if vector is zero")
        .def("isUnit", &Vector3d::IsUnit,
             "Check if vector is unit length")
        
        // 运算符
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self * double())
        .def(double() * py::self)
        .def(py::self / double())
        .def(py::self += py::self)
        .def(py::self -= py::self)
        .def(py::self *= double())
        .def(py::self /= double())
        .def(py::self * py::self, "Component-wise multiplication")
        .def(-py::self, "Negation")
        
        .def("__iter__", [](const Vector3d& v) {
            return py::make_iterator(v.begin(), v.end());
        }, py::keep_alive<0, 1>())
        .def("__len__", [](const Vector3d&) { return 3; })
        
        // 静态方法
        .def_static("Zero", &Vector3d::Zero, "Create a zero vector")
        .def_static("UnitX", &Vector3d::UnitX, "Create unit vector along X")
        .def_static("UnitY", &Vector3d::UnitY, "Create unit vector along Y")
        .def_static("UnitZ", &Vector3d::UnitZ, "Create unit vector along Z")
        
        .def("__repr__", [](const Vector3d& v) {
            return "<Vector3d(" + std::to_string(v.x()) + ", " +
                   std::to_string(v.y()) + ", " + std::to_string(v.z()) + ")>";
        });

    // ============================================
    // Matrix3d 类
    // ============================================
    py::class_<Matrix3d>(m, "Matrix3d", R"pbdoc(
        3x3 Matrix class for rotations and transformations.
        
        A Matrix3d represents a 3x3 matrix, commonly used for rotation matrices
        and 3D transformations.
    )pbdoc")
        // 构造函数
        .def(py::init<>(), "Create an identity matrix")
        .def(py::init<double, double, double, double, double, double,
             double, double, double>(),
             py::arg("m00"), py::arg("m01"), py::arg("m02"),
             py::arg("m10"), py::arg("m11"), py::arg("m12"),
             py::arg("m20"), py::arg("m21"), py::arg("m22"),
             "Create matrix from 9 elements (row-major)")
        .def(py::init([](py::list lst) {
            if (py::len(lst) != 3) {
                throw std::invalid_argument("Matrix3d requires 3 rows");
            }
            Matrix3d m;
            for (int i = 0; i < 3; ++i) {
                py::list row = py::cast<py::list>(lst[i]);
                if (py::len(row) != 3) {
                    throw std::invalid_argument("Each row requires 3 elements");
                }
                for (int j = 0; j < 3; ++j) {
                    m(i, j) = py::cast<double>(row[j]);
                }
            }
            return m;
        }), py::arg("values"), "Create matrix from 3x3 list")
        
        // 访问器
        .def("__getitem__", [](const Matrix3d& m, std::pair<int, int> idx) {
            return m(idx.first, idx.second);
        })
        .def("__setitem__", [](Matrix3d& m, std::pair<int, int> idx, double val) {
            m(idx.first, idx.second) = val;
        })
        .def("row", &Matrix3d::Row, py::arg("i"),
             "Get row as Vector3d")
        .def("col", &Matrix3d::Col, py::arg("j"),
             "Get column as Vector3d")
        
        // 属性
        .def_property_readonly("determinant", &Matrix3d::Determinant,
            "Matrix determinant")
        .def_property_readonly("trace", &Matrix3d::Trace,
            "Matrix trace")
        
        // 方法
        .def("transpose", &Matrix3d::Transpose,
             "Return transposed matrix")
        .def("inverse", &Matrix3d::Inverse,
             "Return inverse matrix")
        .def("isIdentity", &Matrix3d::IsIdentity,
             "Check if matrix is identity")
        .def("isOrthogonal", &Matrix3d::IsOrthogonal,
             "Check if matrix is orthogonal")
        
        // 运算符
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self * py::self)
        .def(py::self * Vector3d())
        .def(py::self * double())
        .def(double() * py::self)
        .def(py::self / double())
        
        // 静态方法
        .def_static("Identity", &Matrix3d::Identity, "Create identity matrix")
        .def_static("Zero", &Matrix3d::Zero, "Create zero matrix")
        .def_static("FromRowMajor", [](double m00, double m01, double m02,
                                        double m10, double m11, double m12,
                                        double m20, double m21, double m22) {
            Matrix3d m;
            m << m00, m01, m02,
                 m10, m11, m12,
                 m20, m21, m22;
            return m;
        }, "Create matrix from row-major values")
        
        .def("__repr__", [](const Matrix3d& m) {
            std::ostringstream oss;
            oss << "<Matrix3d:\n";
            for (int i = 0; i < 3; ++i) {
                oss << "  [";
                for (int j = 0; j < 3; ++j) {
                    oss << std::setw(10) << std::setprecision(4) << m(i, j);
                    if (j < 2) oss << ", ";
                }
                oss << "]\n";
            }
            oss << ")>";
            return oss.str();
        });

    // ============================================
    // Quaternion 类
    // ============================================
    py::class_<Quaternion>(m, "Quaternion", R"pbdoc(
        Quaternion class for representing 3D rotations.
        
        Quaternions are a mathematical way to represent rotations that avoids
        gimbal lock and provides smooth interpolation. The quaternion uses
        Hamilton convention: q = w + xi + yj + zk.
    )pbdoc")
        // 构造函数
        .def(py::init<>(), "Create identity quaternion")
        .def(py::init<double, double, double, double>(),
             py::arg("w"), py::arg("x"), py::arg("y"), py::arg("z"),
             "Create quaternion from components (w=real, x,y,z=imaginary)")
        .def(py::init([](double scalar, py::list vec) {
            if (py::len(vec) != 3) {
                throw std::invalid_argument("Quaternion imaginary part requires 3 elements");
            }
            return Quaternion(scalar,
                py::cast<double>(vec[0]),
                py::cast<double>(vec[1]),
                py::cast<double>(vec[2])
            );
        }), py::arg("scalar"), py::arg("imaginary"),
            "Create quaternion from scalar and imaginary vector")
        .def(py::init([](py::list lst) {
            if (py::len(lst) == 4) {
                return Quaternion(
                    py::cast<double>(lst[0]),
                    py::cast<double>(lst[1]),
                    py::cast<double>(lst[2]),
                    py::cast<double>(lst[3])
                );
            } else if (py::len(lst) == 3) {
                // Assume [x, y, z] with w = sqrt(1 - |v|^2)
                double x = py::cast<double>(lst[0]);
                double y = py::cast<double>(lst[1]);
                double z = py::cast<double>(lst[2]);
                double w = std::sqrt(1.0 - x*x - y*y - z*z);
                return Quaternion(w, x, y, z);
            }
            throw std::invalid_argument("Quaternion requires 3 or 4 elements");
        }), py::arg("values"), "Create quaternion from list")
        
        // 属性
        .def_property("w", &Quaternion::w, &Quaternion::setW,
            "W (scalar) component")
        .def_property("x", &Quaternion::x, &Quaternion::setX,
            "X component")
        .def_property("y", &Quaternion::y, &Quaternion::setY,
            "Y component")
        .def_property("z", &Quaternion::z, &Quaternion::setZ,
            "Z component")
        .def_property_readonly("vec", &Quaternion::vec,
            "Imaginary part as Vector3d")
        
        // 访问器
        .def_property_readonly("norm", &Quaternion::norm,
            "Quaternion norm")
        .def_property_readonly("squaredNorm", &Quaternion::squaredNorm,
            "Squared norm")
        
        // 方法
        .def("normalized", &Quaternion::normalized,
             "Return normalized copy")
        .def("normalize", &Quaternion::normalize,
             "Normalize this quaternion in-place")
        .def("conjugate", &Quaternion::conjugate,
             "Return conjugate (q* = w - xi - yj - zk)")
        .def("inverse", &Quaternion::inverse,
             "Return inverse quaternion")
        .def("toRotationMatrix", &Quaternion::toRotationMatrix,
             "Convert to 3x3 rotation matrix")
        .def("setIdentity", &Quaternion::setIdentity,
             "Set to identity quaternion")
        
        // 运算符
        .def(py::self * py::self)
        .def(py::self * double())
        .def(double() * py::self)
        .def(py::self / double())
        .def(-py::self)
        
        // 静态方法
        .def_static("Identity", &Quaternion::Identity, "Create identity quaternion")
        .def_static("FromRotationMatrix", &Quaternion::FromRotationMatrix,
            py::arg("matrix"), "Create quaternion from rotation matrix")
        .def_static("FromAxisAngle", [](double angle, const Vector3d& axis) {
            double half_angle = angle / 2.0;
            double sin_ha = std::sin(half_angle);
            double cos_ha = std::cos(half_angle);
            return Quaternion(cos_ha,
                             axis.x() * sin_ha,
                             axis.y() * sin_ha,
                             axis.z() * sin_ha);
        }, py::arg("angle"), py::arg("axis"),
            "Create quaternion from axis-angle representation")
        
        .def("__repr__", [](const Quaternion& q) {
            return "<Quaternion(" + std::to_string(q.w()) + ", " +
                   std::to_string(q.x()) + ", " +
                   std::to_string(q.y()) + ", " +
                   std::to_string(q.z()) + ")>";
        });

    // ============================================
    // Euler 类
    // ============================================
    py::class_<Euler>(m, "Euler", R"pbdoc(
        Euler angles class for representing 3D rotations.
        
        Euler angles represent a rotation through a sequence of three
        rotations about specific axes. Common rotation orders include
        XYZ, ZYX, etc.
        
        Rotation orders:
            eX=1, eY=2, eZ=3
            eXYZ=123, eXZY=132, eYXZ=213, eYZX=231, eZXY=312, eZYX=321
            eXYX=121, eXZX=131, eYXY=212, eYZY=232, eZXZ=313, eZYZ=323
    )pbdoc")
        // 旋转顺序枚举
        py::enum_<Euler::ERotationOrder>(m, "EulerOrder", R"pbdoc(
            Euler angle rotation order enumeration
        )pbdoc")
            .value("X", Euler::ERotationOrder::eX)
            .value("Y", Euler::ERotationOrder::eY)
            .value("Z", Euler::ERotationOrder::eZ)
            .value("XYZ", Euler::ERotationOrder::eXYZ)
            .value("XZY", Euler::ERotationOrder::eXZY)
            .value("YXZ", Euler::ERotationOrder::eYXZ)
            .value("YZX", Euler::ERotationOrder::eYZX)
            .value("ZXY", Euler::ERotationOrder::eZXY)
            .value("ZYX", Euler::ERotationOrder::eZYX)
            .value("XYX", Euler::ERotationOrder::eXYX)
            .value("XZX", Euler::ERotationOrder::eXZX)
            .value("YXY", Euler::ERotationOrder::eYXY)
            .value("YZY", Euler::ERotationOrder::eYZY)
            .value("ZXZ", Euler::ERotationOrder::eZXZ)
            .value("ZYZ", Euler::ERotationOrder::eZYZ)
            .export_values()
        
        // 构造函数
        .def(py::init<>(), "Create zero Euler angles")
        .def(py::init<double, double, double>(),
             py::arg("angle1"), py::arg("angle2"), py::arg("angle3"),
             "Create from three angles")
        
        // 属性
        .def_property("angle1", &Euler::angle1, &Euler::angle1,
            "First rotation angle (radians)")
        .def_property("angle2", &Euler::angle2, &Euler::angle2,
            "Second rotation angle (radians)")
        .def_property("angle3", &Euler::angle3, &Euler::angle3,
            "Third rotation angle (radians)")
        
        // 方法
        .def("toMatrix", [](const Euler& self, int seq) {
            Matrix3d mtx;
            self.toMatrix(seq, mtx);
            return mtx;
        }, py::arg("seq"),
            "Convert to rotation matrix with given sequence")
        .def("toQuat", [](const Euler& self, int seq) {
            Quaternion quat;
            self.toQuat(seq, quat);
            return quat;
        }, py::arg("seq"),
            "Convert to quaternion with given sequence")
        .def("fromMatrix", [](Euler& self, const Matrix3d& mtx, int seq) {
            return self.fromMatrix(mtx, seq);
        }, py::arg("matrix"), py::arg("seq"),
            "Create from rotation matrix")
        .def("fromQuat", [](Euler& self, const Quaternion& quat, int seq) {
            return self.fromQuat(quat, seq);
        }, py::arg("quaternion"), py::arg("seq"),
            "Create from quaternion")
        
        .def("__repr__", [](const Euler& e) {
            return "<Euler(" + std::to_string(e.angle1()) + ", " +
                   std::to_string(e.angle2()) + ", " +
                   std::to_string(e.angle3()) + ")>";
        });

    // ============================================
    // AngleAxis 类
    // ============================================
    py::class_<AngleAxis>(m, "AngleAxis", R"pbdoc(
        Angle-Axis representation of 3D rotations.
        
        Represents a rotation by an angle about a unit axis vector.
        Also known as rotation vector representation.
    )pbdoc")
        // 构造函数
        .def(py::init<>(), "Create identity (zero rotation)")
        .def(py::init<double, const Vector3d&>(),
             py::arg("angle"), py::arg("axis"),
             "Create from angle (radians) and axis vector")
        
        // 属性
        .def_property("angle", &AngleAxis::angle, &AngleAxis::angle,
            "Rotation angle in radians")
        .def_property("axis", &AngleAxis::axis, &AngleAxis::axis,
            "Rotation axis as Vector3d")
        
        // 方法
        .def("inverse", &AngleAxis::inverse,
             "Return inverse rotation")
        .def("fromRotationMatrix", &AngleAxis::fromRotationMatrix,
             py::arg("matrix"), "Initialize from rotation matrix")
        .def("toRotationMatrix", &AngleAxis::toRotationMatrix,
             "Convert to rotation matrix")
        
        // 静态方法
        .def_static("Identity", []() { return AngleAxis(); },
            "Create identity rotation")
        
        .def("__repr__", [](const AngleAxis& aa) {
            return "<AngleAxis(angle=" + std::to_string(aa.angle()) +
                   ", axis=" + std::to_string(aa.axis().x()) + ", " +
                   std::to_string(aa.axis().y()) + ", " +
                   std::to_string(aa.axis().z()) + ")>";
        });
}

}  // namespace astpy

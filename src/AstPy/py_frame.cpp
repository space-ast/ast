/**
 * @file py_frame.cpp
 * @brief 坐标系 Python 绑定实现
 * @author SpaceAST Development Team
 * @version 1.0.0
 * @date 2024
 */

#include "py_frame.h"

namespace py = pybind11;
using namespace ast;

namespace astpy {

void bind_frame(py::module& m) {
    // ============================================
    // Transform 类
    // ============================================
    py::class_<Transform>(m, "Transform", R"pbdoc(
        Coordinate transformation class.
        
        Represents a complete coordinate transformation including
        rotation and translation.
    )pbdoc")
        .def(py::init<>(), "Create identity transform")
        .def(py::init<const Rotation&, const Vector3d&>(),
             py::arg("rotation"), py::arg("translation"),
             "Create transform from rotation and translation")
        .def(py::init<const Rotation&, const Vector3d&, const Rotation&, const Vector3d&>(),
             py::arg("attitude"), py::arg("position"),
             py::arg("angularVelocity"), py::arg("velocity"),
             "Create full kinematic transform")
        
        .def_property_readonly("rotation", &Transform::GetRotation,
            "Get rotation component")
        .def_property_readonly("translation", &Transform::GetTranslation,
            "Get translation component")
        .def_property_readonly("attitude", &Transform::GetAttitude,
            "Get attitude rotation")
        .def_property_readonly("position", &Transform::GetPosition,
            "Get position vector")
        
        .def("applyTo", &Transform::ApplyTo, py::arg("point"),
             "Apply transform to a point")
        .def("applyInverseTo", &Transform::ApplyInverseTo, py::arg("point"),
             "Apply inverse transform to a point")
        .def("getInverse", &Transform::GetInverse,
             "Get inverse transform");

    // ============================================
    // KinematicTransform 类
    // ============================================
    py::class_<KinematicTransform>(m, "KinematicTransform", R"pbdoc(
        Kinematic transformation including velocities.
        
        Represents a complete transformation including position, velocity,
        attitude, and angular velocity.
    )pbdoc")
        .def(py::init<>(), "Create zero transform")
        .def(py::init<const Rotation&, const Vector3d&>(),
             py::arg("attitude"), py::arg("position"),
             "Create from attitude and position")
        .def(py::init<const Rotation&, const Vector3d&, const Rotation&, const Vector3d&>(),
             py::arg("attitude"), py::arg("position"),
             py::arg("angularVelocity"), py::arg("velocity"),
             "Create full kinematic transform")
        
        .def_property_readonly("rotation", &KinematicTransform::GetRotation,
            "Get rotation")
        .def_property_readonly("translation", &KinematicTransform::GetTranslation,
            "Get translation")
        .def_property_readonly("attitude", &KinematicTransform::GetAttitude,
            "Get attitude rotation")
        .def_property_readonly("position", &KinematicTransform::GetPosition,
            "Get position")
        .def_property_readonly("angularVelocity", &KinematicTransform::GetAngularVelocity,
            "Get angular velocity")
        .def_property_readonly("velocity", &KinematicTransform::GetVelocity,
            "Get velocity");

    // ============================================
    // Axes 类 (轴系)
    // ============================================
    py::class_<Axes, SPtr<Axes>>(m, "Axes", R"pbdoc(
        Axes class representing a coordinate axes system.
        
        Axes define the orientation of a coordinate system.
        This is an abstract base class.
    )pbdoc")
        .def("getName", &Axes::getName,
             "Get the axes name")
        .def("getTransform", &Axes::getTransform,
             py::arg("tp"), py::arg("transform"),
             "Get transformation at given time point");

    // ============================================
    // Frame 类 (坐标系)
    // ============================================
    py::class_<Frame, SPtr<Frame>>(m, "Frame", R"pbdoc(
        Frame class representing a coordinate frame.
        
        A Frame defines both the origin (Point) and orientation (Axes)
        of a coordinate system.
    )pbdoc")
        .def("getName", &Frame::getName,
             "Get the frame name")
        .def("getBody", &Frame::getBody,
             "Get associated celestial body")
        .def("getGM", &Frame::getGM,
             "Get gravitational parameter of central body")
        .def("getParent", &Frame::getParent,
             "Get parent frame")
        .def("getAxes", &Frame::getAxes,
             "Get axes of this frame")
        .def("getOrigin", &Frame::getOrigin,
             "Get origin point of this frame")
        .def("getTransform", [](Frame& self, const TimePoint& tp, Transform& transform) {
            return self.getTransform(tp, transform);
        }, py::arg("tp"), py::arg("transform"),
             "Get transformation to parent at time point")
        .def("getTransformTo", [](Frame& self, Frame& target, const TimePoint& tp, Transform& transform) {
            return self.getTransformTo(&target, tp, transform);
        }, py::arg("target"), py::arg("tp"), py::arg("transform"),
             "Get transformation to target frame")
        .def("getTransformFrom", [](Frame& self, Frame& source, const TimePoint& tp, Transform& transform) {
            return self.getTransformFrom(&source, tp, transform);
        }, py::arg("source"), py::arg("tp"), py::arg("transform"),
             "Get transformation from source frame");

    // ============================================
    // Rotation 类
    // ============================================
    py::class_<Rotation>(m, "Rotation", R"pbdoc(
        Rotation class representing a pure rotation.
        
        A Rotation defines how to rotate vectors between coordinate systems.
    )pbdoc")
        .def(py::init<>(), "Create identity rotation")
        .def(py::init<const Matrix3d&>(),
             py::arg("matrix"), "Create from rotation matrix")
        .def(py::init<const Quaternion&>(),
             py::arg("quaternion"), "Create from quaternion")
        
        .def_property_readonly("matrix", &Rotation::GetMatrix,
             "Get rotation matrix")
        .def_property_readonly("quaternion", &Rotation::GetQuaternion,
             "Get quaternion")
        
        .def("applyTo", &Rotation::ApplyTo, py::arg("vec"),
             "Apply rotation to a vector")
        .def("applyInverseTo", &Rotation::ApplyInverseTo, py::arg("vec"),
             "Apply inverse rotation to a vector")
        .def("getInverse", &Rotation::GetInverse,
             "Get inverse rotation")
        .def("compose", &Rotation::Compose, py::arg("other"),
             "Compose with another rotation")
        .def("between", &Rotation::Between, py::arg("other"),
             "Get rotation between this and another");

    // ============================================
    // FrameRoot 类
    // ============================================
    py::class_<FrameRoot, Frame, SPtr<FrameRoot>>(m, "FrameRoot", R"pbdoc(
        Root frame - the base inertial frame.
        
        FrameRoot represents the top-level inertial reference frame.
    )pbdoc");

    // ============================================
    // FrameICRF 类
    // ============================================
    py::class_<FrameICRF, Frame, SPtr<FrameICRF>>(m, "FrameICRF", R"pbdoc(
        ICRF (International Celestial Reference Frame) frame.
    )pbdoc");

    // ============================================
    // AxesRoot 类
    // ============================================
    py::class_<AxesRoot, Axes, SPtr<AxesRoot>>(m, "AxesRoot", R"pbdoc(
        Root axes - base axes system.
    )pbdoc");

    // ============================================
    // AxesICRF 类
    // ============================================
    py::class_<AxesICRF, Axes, SPtr<AxesICRF>>(m, "AxesICRF", R"pbdoc(
        ICRF axes system.
    )pbdoc");

    // ============================================
    // 坐标转换函数
    // ============================================
    m.def("aFrameTransform", static_cast<Transform(*)(Frame&, Frame&, const TimePoint&)>(
        &aFrameTransform),
        py::arg("fromFrame"), py::arg("toFrame"), py::arg("timePoint"),
        R"pbdoc(
        Get transformation between two frames at a given time.
        
        Args:
            fromFrame: Source frame
            toFrame: Target frame
            timePoint: Time of transformation
        
        Returns:
            Transformation from source to target
        )pbdoc");

    m.def("aAxesTransform", static_cast<Rotation(*)(Axes&, Axes&, const TimePoint&)>(
        &aAxesTransform),
        py::arg("fromAxes"), py::arg("toAxes"), py::arg("timePoint"),
        R"pbdoc(
        Get rotation between two axes at a given time.
        
        Args:
            fromAxes: Source axes
            toAxes: Target axes
            timePoint: Time of transformation
        
        Returns:
            Rotation from source to target
        )pbdoc");

    m.def("aQuatToMatrix", &aQuatToMatrix, py::arg("quat"),
        R"pbdoc(
        Convert quaternion to rotation matrix.
    )pbdoc");

    m.def("aMatrixToQuat", &aMatrixToQuat, py::arg("matrix"),
        R"pbdoc(
        Convert rotation matrix to quaternion.
    )pbdoc");

    m.def("aEulerToQuat", &aEulerToQuat, py::arg("angle1"),
        py::arg("angle2"), py::arg("angle3"), py::arg("seq"),
        R"pbdoc(
        Convert Euler angles to quaternion.
        
        Args:
            angle1: First Euler angle [rad]
            angle2: Second Euler angle [rad]
            angle3: Third Euler angle [rad]
            seq: Rotation sequence (e.g., 321 for ZYX)
        
        Returns:
            Quaternion
        )pbdoc");

    m.def("aQuatToEuler", &aQuatToEuler, py::arg("quat"), py::arg("seq"),
        R"pbdoc(
        Convert quaternion to Euler angles.
        
        Args:
            quat: Input quaternion
            seq: Rotation sequence
        
        Returns:
            Tuple of (angle1, angle2, angle3) in radians
        )pbdoc");

    m.def("aRotationXMatrix", &aRotationXMatrix, py::arg("angle"),
        R"pbdoc(
        Create rotation matrix about X axis.
    )pbdoc");

    m.def("aRotationYMatrix", &aRotationYMatrix, py::arg("angle"),
        R"pbdoc(
        Create rotation matrix about Y axis.
    )pbdoc");

    m.def("aRotationZMatrix", &aRotationZMatrix, py::arg("angle"),
        R"pbdoc(
        Create rotation matrix about Z axis.
    )pbdoc");

    m.def("aRotationMatrix", &aRotationMatrix, py::arg("axis"), py::arg("angle"),
        R"pbdoc(
        Create rotation matrix about arbitrary axis.
    )pbdoc");
}

}  // namespace astpy

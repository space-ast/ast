#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
attitude_examples.py - Attitude Representation Examples

This script demonstrates the use of quaternions, Euler angles,
and rotation matrices for attitude representation.

Author: SpaceAST Development Team
Version: 1.0.0
"""

import astpy
import math

def print_section(title):
    """Print a section header."""
    print("\n" + "=" * 60)
    print(f" {title}")
    print("=" * 60)


def example_quaternion_basics():
    """Example: Basic quaternion operations."""
    print_section("Quaternion Basics")
    
    # Identity quaternion
    q_id = astpy.Quaternion.Identity()
    print(f"Identity quaternion: {q_id}")
    print(f"  w={q_id.w:.4f}, x={q_id.x:.4f}, y={q_id.y:.4f}, z={q_id.z:.4f}")
    
    # Quaternion from axis-angle
    angle_deg = 90
    angle_rad = math.radians(angle_deg)
    axis = astpy.Vector3d(0, 0, 1)  # Z-axis
    
    q = astpy.Quaternion.FromAxisAngle(angle_rad, axis)
    print(f"\n{angle_deg} degree rotation around Z-axis:")
    print(f"Quaternion: {q}")
    print(f"  w={q.w:.6f}, x={q.x:.6f}, y={q.y:.6f}, z={q.z:.6f}")
    print(f"  Norm: {q.norm():.6f}")
    
    # Verify rotation
    v = astpy.Vector3d(1, 0, 0)
    rot_matrix = q.toRotationMatrix()
    v_rotated = rot_matrix * v
    print(f"\nRotating {v} by 90 deg around Z:")
    print(f"  Result: ({v_rotated.x:.6f}, {v_rotated.y:.6f}, {v_rotated.z:.6f})")
    print(f"  Expected: (0, 1, 0)")


def example_quaternion_composition():
    """Example: Composing rotations with quaternions."""
    print_section("Quaternion Composition")
    
    # Rotation of 45 degrees around X
    q_x = astpy.Quaternion.FromAxisAngle(math.pi/4, astpy.Vector3d(1, 0, 0))
    
    # Rotation of 45 degrees around Y
    q_y = astpy.Quaternion.FromAxisAngle(math.pi/4, astpy.Vector3d(0, 1, 0))
    
    # Composition: X then Y (Q = Q_Y * Q_X)
    q_combined = q_y * q_x
    
    print(f"Rotation 45 deg around X: {q_x}")
    print(f"Rotation 45 deg around Y: {q_y}")
    print(f"Combined (Y after X): {q_combined}")
    
    # Direct rotation around combined axis
    v = astpy.Vector3d(1, 0, 0)
    v_final = q_combined * v
    
    print(f"\nRotating (1,0,0) by combined quaternion:")
    print(f"  Result: ({v_final.x:.6f}, {v_final.y:.6f}, {v_final.z:.6f})")
    
    # Convert combined quaternion to axis-angle
    combined_matrix = q_combined.toRotationMatrix()
    print(f"\nCombined rotation matrix:\n{combined_matrix}")


def example_euler_angles():
    """Example: Euler angle representation."""
    print_section("Euler Angles")
    
    # Create Euler angles (roll, pitch, yaw in XYZ convention)
    roll = math.radians(30)
    pitch = math.radians(20)
    yaw = math.radians(10)
    
    euler = astpy.Euler(roll, pitch, yaw)
    print(f"Euler angles (deg): ({math.degrees(roll):.1f}, {math.degrees(pitch):.1f}, {math.degrees(yaw):.1f})")
    print(f"Euler object: {euler}")
    
    # Convert to rotation matrix with XYZ sequence
    R_xyz = euler.toMatrix(astpy.Euler.eXYZ)
    print(f"\nXYZ rotation matrix:\n{R_xyz}")
    
    # Convert to quaternion
    q = euler.toQuat(astpy.Euler.eXYZ)
    print(f"\nEquivalent quaternion: {q}")
    
    # Now try ZYX sequence
    R_zyx = euler.toMatrix(astpy.Euler.eZYX)
    print(f"\nZYX rotation matrix (different!):\n{R_zyx}")
    
    # Create same orientation using ZYX sequence
    euler_zyx = astpy.Euler()
    euler_zyx.fromMatrix(R_xyz, astpy.Euler.eZYX)
    print(f"\nZYX Euler from XYZ matrix: ({math.degrees(euler_zyx.angle1):.1f}, "
          f"{math.degrees(euler_zyx.angle2):.1f}, {math.degrees(euler_zyx.angle3):.1f}) deg")


def example_rotation_matrices():
    """Example: Direct rotation matrix operations."""
    print_section("Rotation Matrices")
    
    # Elementary rotations
    angle = math.pi / 6  # 30 degrees
    
    Rx = astpy.aRotationXMatrix(angle)
    Ry = astpy.aRotationYMatrix(angle)
    Rz = astpy.aRotationZMatrix(angle)
    
    print(f"30 degree rotations:")
    print(f"Rx:\n{Rx}")
    print(f"\nRy:\n{Ry}")
    print(f"\nRz:\n{Rz}")
    
    # Combined rotation: Rz * Ry * Rx (X then Y then Z)
    R_combined = Rz * Ry * Rx
    print(f"\nCombined (Z*Y*X):\n{R_combined}")
    
    # Verify it's orthogonal (R^T * R = I)
    R_transpose = R_combined.transpose()
    R_identity_check = R_transpose * R_combined
    print(f"\nOrthogonality check (R^T * R):\n{R_identity_check}")
    
    # Determinant should be +1
    print(f"\nDeterminant: {R_combined.determinant():.6f} (should be 1)")


def example_vector_rotations():
    """Example: Rotating vectors between frames."""
    print_section("Vector Rotations")
    
    # Create a vector
    v = astpy.Vector3d(1, 0, 0)
    print(f"Original vector: {v}")
    
    # Rotate by 90 degrees around Z
    angle = math.pi / 2
    Rz = astpy.aRotationZMatrix(angle)
    
    v_rotated = Rz * v
    print(f"\nAfter 90 deg rotation around Z:")
    print(f"  {v_rotated}")
    print(f"  (should be approximately 0, 1, 0)")
    
    # Rotate by 90 degrees around Y
    Ry = astpy.aRotationYMatrix(angle)
    v_rotated2 = Ry * v
    print(f"\nAfter 90 deg rotation around Y:")
    print(f"  {v_rotated2}")
    print(f"  (should be approximately 0, 0, -1)")
    
    # Combined rotation
    R = Ry * Rz
    v_final = R * v
    print(f"\nAfter Z then Y rotation:")
    print(f"  {v_final}")


def example_angle_axis():
    """Example: Angle-Axis representation."""
    print_section("Angle-Axis Representation")
    
    # Create rotation using AngleAxis
    angle = math.radians(60)
    axis = astpy.Vector3d(1, 1, 1).normalized()
    
    aa = astpy.AngleAxis(angle, axis)
    print(f"Angle-Axis representation:")
    print(f"  Angle: {math.degrees(aa.angle):.2f} degrees")
    print(f"  Axis: {aa.axis}")
    
    # Convert to rotation matrix
    R = aa.toRotationMatrix()
    print(f"\nRotation matrix:\n{R}")
    
    # Convert to quaternion
    q = astpy.Quaternion.FromAxisAngle(angle, axis)
    print(f"\nQuaternion: {q}")
    
    # Verify consistency
    R_from_q = q.toRotationMatrix()
    diff = R - R_from_q
    print(f"\nMatrix difference (should be zeros):\n{diff}")


def example_quaternion_interpolation():
    """Example: Quaternion interpolation (SLERP concept)."""
    print_section("Quaternion Interpolation")
    
    # Two orientations
    q1 = astpy.Quaternion.Identity()
    q2 = astpy.Quaternion.FromAxisAngle(math.pi, astpy.Vector3d(0, 0, 1))
    
    print(f"Orientation 1 (identity): {q1}")
    print(f"Orientation 2 (180 deg around Z): {q2}")
    
    # Linear interpolation (simplified - not true SLERP)
    t_values = [0.0, 0.25, 0.5, 0.75, 1.0]
    
    print(f"\nLinear interpolation (not true SLERP):")
    for t in t_values:
        # Simple linear interpolation of components (not correct!)
        w = q1.w * (1-t) + q2.w * t
        x = q1.x * (1-t) + q2.x * t
        y = q1.y * (1-t) + q2.y * t
        z = q1.z * (1-t) + q2.z * t
        
        # Normalize
        q_interp = astpy.Quaternion(w, x, y, z).normalized()
        
        # Get rotation angle around Z
        rot_matrix = q_interp.toRotationMatrix()
        trace = rot_matrix.trace()
        angle = math.acos((trace - 1) / 2)
        
        print(f"  t={t:.2f}: angle around Z = {math.degrees(angle):.1f} deg")


def main():
    """Run all attitude examples."""
    print("\n" + "#" * 60)
    print("# SpaceAST - Attitude Representation Examples")
    print("#" * 60)
    
    example_quaternion_basics()
    example_quaternion_composition()
    example_euler_angles()
    example_rotation_matrices()
    example_vector_rotations()
    example_angle_axis()
    example_quaternion_interpolation()
    
    print("\n" + "#" * 60)
    print("# All attitude examples completed!")
    print("#" * 60 + "\n")


if __name__ == "__main__":
    main()

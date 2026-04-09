#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
basic_usage.py - SpaceAST Python Bindings Basic Usage Examples

This script demonstrates the basic usage of the SpaceAST Python bindings
for common astrodynamics operations.

Author: SpaceAST Development Team
Version: 1.0.0
"""

import astpy

def print_section(title):
    """Print a section header."""
    print("\n" + "=" * 60)
    print(f" {title}")
    print("=" * 60)


def example_time_system():
    """Example: Time system operations."""
    print_section("Time System Examples")
    
    # Create time point from UTC
    tp1 = astpy.TimePoint.FromUTC(2024, 1, 1, 12, 0, 0)
    print(f"Created TimePoint: {tp1}")
    
    # Get J2000 epoch
    j2000 = astpy.TimePoint.J2000TT()
    print(f"J2000 epoch: {j2000}")
    
    # Time arithmetic
    tp2 = tp1 + 3600  # Add 1 hour
    print(f"After +3600s: {tp2}")
    
    # Time difference
    dt = tp2 - tp1
    print(f"Time difference: {dt} seconds")
    
    # Convert to different time scales
    jd_tt = tp1.toTT()
    jd_tdb = tp1.toTDB()
    print(f"TT Julian Date: {jd_tt}")
    print(f"TDB Julian Date: {jd_tdb}")
    
    # JulianDate operations
    jd = astpy.JulianDate.FromImpreciseDay(2460292.5)
    print(f"\nJulianDate: {jd}")
    print(f"Imprecise day: {jd.impreciseDay()}")
    
    # DateTime conversion
    dt_obj = astpy.DateTime(2024, 1, 15, 10, 30, 45.5)
    print(f"\nDateTime: {dt_obj}")


def example_vectors():
    """Example: Vector3d operations."""
    print_section("Vector3d Examples")
    
    # Create vectors
    v1 = astpy.Vector3d(1.0, 2.0, 3.0)
    v2 = astpy.Vector3d(4.0, 5.0, 6.0)
    
    print(f"v1 = {v1}")
    print(f"v2 = {v2}")
    
    # Basic operations
    print(f"\nv1 + v2 = {v1 + v2}")
    print(f"v1 - v2 = {v1 - v2}")
    print(f"v1 * 2 = {v1 * 2}")
    print(f"v1 / 2 = {v1 / 2}")
    
    # Vector operations
    print(f"\nv1.norm() = {v1.norm():.6f}")
    print(f"v1.squaredNorm() = {v1.squaredNorm():.6f}")
    print(f"v1.dot(v2) = {v1.dot(v2):.6f}")
    print(f"v1.cross(v2) = {v1.cross(v2)}")
    print(f"v1.angleTo(v2) = {astpy.deg(v1.angleTo(v2)):.2f} degrees")
    
    # Normalization
    v3 = astpy.Vector3d(3.0, 4.0, 0.0)
    v3_norm = v3.normalized()
    print(f"\nv3 = {v3}, |v3| = {v3.norm():.6f}")
    print(f"v3.normalized() = {v3_norm}, |v3_norm| = {v3_norm.norm():.6f}")
    
    # Create from list
    v_list = astpy.Vector3d([1.0, 2.0, 3.0])
    print(f"\nFrom list: {v_list}")
    
    # Indexing
    print(f"v1[0] = {v1[0]}, v1[1] = {v1[1]}, v1[2] = {v1[2]}")
    
    # Unit vectors
    print(f"\nUnit vectors:")
    print(f"UnitX = {astpy.Vector3d.UnitX()}")
    print(f"UnitY = {astpy.Vector3d.UnitY()}")
    print(f"UnitZ = {astpy.Vector3d.UnitZ()}")


def example_matrices():
    """Example: Matrix3d operations."""
    print_section("Matrix3d Examples")
    
    # Identity matrix
    I = astpy.Matrix3d.Identity()
    print(f"Identity matrix:\n{I}")
    
    # Create from list
    m = astpy.Matrix3d([
        [1, 2, 3],
        [4, 5, 6],
        [7, 8, 9]
    ])
    print(f"\nCustom matrix:\n{m}")
    
    # Matrix operations
    print(f"\nTrace: {m.trace():.2f}")
    print(f"Determinant: {m.determinant():.2f}")
    print(f"Transpose:\n{m.transpose()}")
    
    # Matrix-vector multiplication
    v = astpy.Vector3d(1, 2, 3)
    print(f"\nm * v = {m * v}")
    
    # Rotation matrices
    rot_x = astpy.aRotationXMatrix(astpy.PI / 4)
    rot_y = astpy.aRotationYMatrix(astpy.PI / 4)
    rot_z = astpy.aRotationZMatrix(astpy.PI / 4)
    print(f"\nRotation X (45 deg):\n{rot_x}")


def example_quaternions():
    """Example: Quaternion operations."""
    print_section("Quaternion Examples")
    
    # Identity quaternion
    q_id = astpy.Quaternion.Identity()
    print(f"Identity quaternion: {q_id}")
    
    # Create quaternion
    q = astpy.Quaternion(0.87758, 0.23971, 0.0, 0.0)  # 30 deg rotation around Z
    print(f"Quaternion: {q}")
    
    # Quaternion from axis-angle
    angle = astpy.PI / 6  # 30 degrees
    axis = astpy.Vector3d(0, 0, 1)
    q_aa = astpy.Quaternion.FromAxisAngle(angle, axis)
    print(f"From axis-angle ({astpy.deg(angle):.1f} deg around Z): {q_aa}")
    
    # Quaternion properties
    print(f"\nQuaternion norm: {q.norm():.6f}")
    print(f"Quaternion components: w={q.w:.6f}, x={q.x:.6f}, y={q.y:.6f}, z={q.z:.6f}")
    
    # Quaternion multiplication
    q1 = astpy.Quaternion(0.96593, 0.25882, 0, 0)  # 20 deg
    q2 = astpy.Quaternion(0.96593, 0, 0.25882, 0)  # 20 deg around Y
    q_combined = q1 * q2
    print(f"\nq1 * q2 = {q_combined}")
    
    # Convert to rotation matrix
    rot_matrix = q.toRotationMatrix()
    print(f"\nTo rotation matrix:\n{rot_matrix}")
    
    # Conjugate and inverse
    print(f"\nConjugate: {q.conjugate()}")
    print(f"Inverse: {q.inverse()}")
    
    # Normalization
    q_unnorm = q * 2
    print(f"\nUnnormalized: {q_unnorm}, norm: {q_unnorm.norm():.6f}")
    print(f"Normalized: {q_unnorm.normalized()}, norm: {q_unnorm.normalized().norm():.6f}")


def example_euler_angles():
    """Example: Euler angles operations."""
    print_section("Euler Angles Examples")
    
    # Create Euler angles
    euler = astpy.Euler(0.1, 0.2, 0.3)
    print(f"Euler angles: {euler}")
    print(f"angle1 = {euler.angle1():.6f}")
    print(f"angle2 = {euler.angle2():.6f}")
    print(f"angle3 = {euler.angle3():.6f}")
    
    # Convert to rotation matrix with different sequences
    rot_xyz = euler.toMatrix(astpy.Euler.eXYZ)
    rot_zyx = euler.toMatrix(astpy.Euler.eZYX)
    print(f"\nXYZ rotation matrix:\n{rot_xyz}")
    
    # Convert to quaternion
    quat = euler.toQuat(astpy.Euler.eXYZ)
    print(f"\nTo quaternion: {quat}")
    
    # Create from quaternion
    q = astpy.Quaternion(0.96593, 0.25882, 0, 0)
    euler_from_q = astpy.Euler()
    euler_from_q.fromQuat(q, astpy.Euler.eXYZ)
    print(f"Euler from quaternion: {euler_from_q}")


def example_orbital_mechanics():
    """Example: Orbital mechanics operations."""
    print_section("Orbital Mechanics Examples")
    
    # Physical constants
    GM = astpy.GM_EARTH
    R_EARTH = astpy.R_EARTH
    print(f"Earth GM: {GM:.3f} km^3/s^2")
    print(f"Earth radius: {R_EARTH:.3f} km")
    
    # Create orbital elements (ISS-like orbit)
    a = R_EARTH + 400  # Semi-major axis [km]
    e = 0.001          # Eccentricity
    i = 0.9           # Inclination [rad] ~51.6 deg
    
    oe = astpy.OrbElem(a=a, e=e, i=i, 
                       raan=0.5, argper=0.3, trueA=0.0)
    print(f"\nOrbital elements:")
    print(f"  Semi-major axis: {oe.a:.3f} km")
    print(f"  Eccentricity: {oe.e:.6f}")
    print(f"  Inclination: {astpy.deg(oe.i):.2f} degrees")
    print(f"  RAAN: {astpy.deg(oe.raan):.2f} degrees")
    print(f"  Arg. Perigee: {astpy.deg(oe.argper):.2f} degrees")
    print(f"  True Anomaly: {astpy.deg(oe.trueA):.2f} degrees")
    
    # Mean motion and period
    n = oe.getMeanMotion(GM)
    T = 2 * astpy.PI / n
    print(f"\nMean motion: {n:.8f} rad/s")
    print(f"Orbital period: {T:.2f} seconds = {T/3600:.2f} hours")
    
    # Create Cartesian state at perigee
    cs = astpy.CartState(
        x=a*(1-e), y=0, z=0,
        vx=0, vy=7.5, vz=0
    )
    print(f"\nCartesian state at perigee:")
    print(f"  Position: ({cs.x:.3f}, {cs.y:.3f}, {cs.z:.3f}) km")
    print(f"  Velocity: ({cs.vx:.3f}, {cs.vy:.3f}, {cs.vz:.3f}) km/s")
    
    # Two-body propagation
    dt = 3600  # Propagate 1 hour
    cs_propagated = astpy.TwoBody.propagate(cs, dt, GM)
    print(f"\nAfter propagating {dt} seconds:")
    print(f"  Position: ({cs_propagated.x:.3f}, {cs_propagated.y:.3f}, {cs_propagated.z:.3f}) km")
    print(f"  Velocity: ({cs_propagated.vx:.3f}, {cs_propagated.vy:.3f}, {cs_propagated.vz:.3f}) km/s")
    
    # Orbit conversion
    oe_converted = astpy.rv2coe(cs, GM)
    print(f"\nConverted back to orbital elements:")
    print(f"  SMA: {oe_converted.a:.3f} km")
    print(f"  ECC: {oe_converted.e:.6f}")


def example_coordinate_transforms():
    """Example: Coordinate frame transformations."""
    print_section("Coordinate Transformation Examples")
    
    # Create rotation matrix
    angle = astpy.PI / 4  # 45 degrees
    rot = astpy.aRotationZMatrix(angle)
    print(f"Rotation matrix (45 deg around Z):\n{rot}")
    
    # Convert between representations
    q = astpy.aMatrixToQuat(rot)
    print(f"\nQuaternion: {q}")
    
    rot_back = astpy.aQuatToMatrix(q)
    print(f"Matrix from quaternion:\n{rot_back}")
    
    # Compose rotations
    rot1 = astpy.aRotationXMatrix(astpy.PI / 6)
    rot2 = astpy.aRotationYMatrix(astpy.PI / 6)
    rot_combined = rot1 * rot2
    print(f"\nCombined rotation (X then Y by 30 deg):\n{rot_combined}")
    
    # Apply rotation to vector
    v = astpy.Vector3d(1, 0, 0)
    v_rotated = rot * v
    print(f"\nRotate {v} by 45 deg around Z: {v_rotated}")


def example_celestial_bodies():
    """Example: Celestial body access."""
    print_section("Celestial Bodies Examples")
    
    # Get solar system
    ss = astpy.SolarSystem.Get()
    print(f"Solar system instance: {ss}")
    
    # Get Earth
    earth = ss.getEarth()
    print(f"\nEarth:")
    print(f"  Name: {earth.name}")
    print(f"  GM: {earth.gm:.3f} km^3/s^2")
    print(f"  Radius: {earth.radius:.3f} km")
    print(f"  Mass: {earth.mass:.3e} kg")
    
    # Get Sun
    sun = ss.getSun()
    print(f"\nSun:")
    print(f"  Name: {sun.name}")
    print(f"  GM: {sun.gm:.3e} km^3/s^2")
    print(f"  Radius: {sun.radius:.3f} km")
    
    # Constants
    print(f"\nPhysical Constants:")
    print(f"  Astronomical Unit: {astpy.AU:.3f} km")
    print(f"  Speed of Light: {astpy.SPEED_OF_LIGHT:.3f} km/s")


def example_convenience_functions():
    """Example: Convenience functions."""
    print_section("Convenience Function Examples")
    
    # Unit conversion
    print("Unit conversion:")
    print(f"  180 deg = {astpy.deg(180):.6f} rad")
    print(f"  PI rad = {astpy.rad(astpy.PI):.2f} deg")
    
    # Quick creation
    print("\nQuick creation functions:")
    v = astpy.vector3d(1, 2, 3)
    print(f"  vector3d: {v}")
    
    q = astpy.quaternion(1, 0, 0, 0)
    print(f"  quaternion: {q}")
    
    tp = astpy.timepoint(2024, 6, 15, 12, 0, 0)
    print(f"  timepoint: {tp}")
    
    oe = astpy.orb_elem(6778, 0.001, 0.9, 0.5, 0.3, 0.0)
    print(f"  orb_elem: SMA={oe.a}")


def main():
    """Run all examples."""
    print("\n" + "#" * 60)
    print("# SpaceAST Python Bindings - Basic Usage Examples")
    print("#" * 60)
    
    # Run all examples
    example_time_system()
    example_vectors()
    example_matrices()
    example_quaternions()
    example_euler_angles()
    example_orbital_mechanics()
    example_coordinate_transforms()
    example_celestial_bodies()
    example_convenience_functions()
    
    print("\n" + "#" * 60)
    print("# All examples completed!")
    print("#" * 60 + "\n")


if __name__ == "__main__":
    main()

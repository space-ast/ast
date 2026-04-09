# AstPy Python bindings for SpaceAST

## Overview

AstPy provides Python bindings for the SpaceAST C++ astrodynamics library using pybind11.

## Installation

### Prerequisites

- Python 3.7+
- pybind11
- SpaceAST C++ library
- CMake or xmake build system

### Build from source

```bash
# Clone SpaceAST
git clone https://gitee.com/space-ast/ast.git
cd ast

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DAST_BUILD_PYTHON=ON -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . --config Release

# Install
cmake --install .
```

## Quick Start

```python
import astpy

# Create vectors
v1 = astpy.Vector3d(1.0, 2.0, 3.0)
v2 = astpy.Vector3d(4.0, 5.0, 6.0)

# Vector operations
print(v1 + v2)           # Addition
print(v1.dot(v2))        # Dot product
print(v1.cross(v2))      # Cross product
print(v1.norm())         # Norm/length

# Create time points
tp1 = astpy.TimePoint.FromUTC(2024, 1, 1, 12, 0, 0)
tp2 = tp1 + 3600  # Add 1 hour

# Create orbital elements
oe = astpy.OrbElem(
    a=6778.0,      # Semi-major axis [km]
    e=0.001,       # Eccentricity
    i=0.9,         # Inclination [rad]
    raan=0.5,      # RAAN [rad]
    argper=0.3,    # Argument of perigee [rad]
    trueA=0.0      # True anomaly [rad]
)

# Create Cartesian state
cs = astpy.CartState(
    x=6778.0, y=0.0, z=0.0,
    vx=0.0, vy=7.5, vz=0.0
)

# Work with quaternions
q = astpy.Quaternion(1.0, 0.0, 0.0, 0.0)  # Identity
q = astpy.Quaternion.FromAxisAngle(0.5, astpy.Vector3d(0, 0, 1))
rot_matrix = q.toRotationMatrix()
```

## Modules

### Time System

```python
# TimePoint - absolute time point
tp = astpy.TimePoint.FromUTC(2024, 1, 1, 12, 0, 0)
tp_tt = tp.toTT()

# JulianDate - continuous time
jd = astpy.JulianDate.FromImpreciseDay(2460292.5)
jd2 = jd + 86400  # Add 1 day

# DateTime - calendar date/time
dt = astpy.DateTime(2024, 1, 1, 12, 0, 0.0)
```

### Mathematics

```python
# Vector3d - 3D vectors
v = astpy.Vector3d(1, 2, 3)
v_normalized = v.normalized()
dot = v1.dot(v2)
cross = v1.cross(v2)

# Matrix3d - 3x3 matrices
m = astpy.Matrix3d.Identity()
m = astpy.Matrix3d([
    [1, 0, 0],
    [0, 1, 0],
    [0, 0, 1]
])

# Quaternion - rotation representation
q = astpy.Quaternion(w=1, x=0, y=0, z=0)
q2 = q * q  # Quaternion multiplication

# Euler - Euler angles
euler = astpy.Euler(angle1=0.1, angle2=0.2, angle3=0.3)
rot = euler.toMatrix(astpy.Euler.eXYZ)
```

### Orbit Mechanics

```python
# OrbElem - Keplerian orbital elements
oe = astpy.OrbElem(a=6778, e=0.001, i=0.9, 
                    raan=0.5, argper=0.3, trueA=0.0)

# CartState - position/velocity state
cs = astpy.CartState(x=6778, y=0, z=0, vx=0, vy=7.5, vz=0)

# TwoBody propagation
state = astpy.TwoBody.propagate(cs, dt=3600, gm=astpy.GM_EARTH)
```

### Coordinate Frames

```python
# Frames and transformations
frame = astpy.FrameRoot()
transform = frame.getTransform(time_point)

# Rotation
rot = astpy.aRotationZMatrix(0.5)
q = astpy.aMatrixToQuat(rot)
```

### Celestial Bodies

```python
# Solar system access
ss = astpy.SolarSystem.Get()
earth = ss.getEarth()
sun = ss.getSun()

# Physical constants
print(astpy.GM_EARTH)  # km^3/s^2
print(astpy.R_EARTH)   # km
print(astpy.AU)        # Astronomical Unit [km]
```

## API Reference

### Constants

| Name | Value | Description |
|------|-------|-------------|
| PI | 3.141592653589793 | Pi |
| TWO_PI | 6.283185307179586 | 2 * Pi |
| DEG_TO_RAD | 0.017453292519943295 | Degrees to radians |
| RAD_TO_DEG | 57.295779513082323 | Radians to degrees |
| GM_EARTH | 3.986004418e5 | Earth's gravitational parameter [km^3/s^2] |
| R_EARTH | 6378.137 | Earth's equatorial radius [km] |
| AU | 149597870.7 | Astronomical Unit [km] |
| J2000_JD | 2451545.0 | J2000 epoch (TT) |

### Helper Functions

```python
# Unit conversion
rad = astpy.deg(45)           # Degrees to radians
deg = astpy.rad(0.785)        # Radians to degrees

# Quick creation
v = astpy.vector3d(1, 2, 3)
q = astpy.quaternion(1, 0, 0, 0)
tp = astpy.timepoint(2024, 1, 1, 12, 0, 0)
oe = astpy.orb_elem(6778, 0.001, 0.9, 0.5, 0.3, 0.0)
```

## License

Apache License 2.0 - See LICENSE file for details.

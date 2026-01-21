# Two-Body Propagator

*Note: This document is AI-translated.*

## Overview

The two-body propagator provides orbit propagation functionality for ideal two-body problems based on the analytical solution of Kepler's equation. This method offers high computational efficiency and is suitable for fast orbit prediction scenarios where high precision is not required.

## Core Concepts

### Two-Body Problem

The two-body problem assumes a system with only two point masses moving under mutual gravitational attraction. This problem has an analytical solution, with orbital shapes being conic sections (ellipse, parabola, or hyperbola).

### Propagation Principle

The two-body propagator uses Kepler's equation to calculate orbital state after a given time interval:

- Calculate orbital elements from initial position and velocity
- Use Kepler's equation to compute true anomaly after given time
- Calculate position and velocity vectors based on the new true anomaly

## Main Interfaces

### Function Interface

```cpp
AST_CORE_CAPI err_t aTwoBodyProp(double duration, double gm, Vector3d& r, Vector3d& v);
```

**Parameter Description:**

- `duration`: Propagation time interval (seconds)
- `gm`: Gravitational constant (m³/s²)
- `r`: Input/output position vector (meters)
- `v`: Input/output velocity vector (m/s)

**Return Value:**

- Returns `AST_OK` on success
- Returns corresponding error code on failure

## Usage Examples

### 1. Basic Two-Body Propagation

@[code](/examples/propagator/two_body/twobody_basic_propagation.cpp)

### 2. Earth Orbit Propagation

@[code](/examples/propagator/two_body/twobody_earth_orbit.cpp)

## Accuracy and Limitations

### Accuracy Characteristics

- **Calculation Accuracy**: Analytical solution, theoretically no truncation error
- **Applicability**: Ideal two-body problems, ignoring all perturbation forces
- **Computational Efficiency**: Very high, suitable for real-time applications

### Usage Limitations

- Not suitable for scenarios requiring consideration of Earth oblateness, atmospheric drag, third-body gravity, etc.
- Errors accumulate during long-term propagation due to model simplification
- For high-precision orbit prediction requirements, recommend using numerical methods like HPOP

## Notes

- Input position and velocity vectors should be in the same inertial coordinate system
- Gravitational constant should match the central body
- Propagation time interval should not be too long to avoid numerical error accumulation
- Higher accuracy for near-circular orbits; numerical stability considerations needed for high-eccentricity orbits

## API Reference

@[code](/src/AstCore/Propagator/TwoBody.hpp)
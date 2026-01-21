# J2 Analytical Propagator

*Note: This document is AI-translated.*

## Overview

The J2 analytical propagator provides orbit propagation considering Earth's oblateness J2 term, offering higher precision orbit prediction than the two-body propagator. This method maintains high computational efficiency while accounting for the main perturbation effects of Earth's non-spherical gravitational field.

## Core Concepts

### J2 Perturbation Term

The J2 term is the primary term in Earth's gravitational field spherical harmonic expansion, representing Earth's oblateness effect. J2 perturbation mainly affects:

- **Orbital plane precession**: Orbital plane precession around Earth's rotation axis
- **Perigee precession**: Rotation of perigee within the orbital plane
- **Nodal regression**: Change in right ascension of ascending node

### Propagation Principle

The J2 analytical propagator is based on long-term variation rates of mean orbital elements:

- Calculate long-term variation rates of orbital elements caused by J2 perturbation
- Perform linear propagation of mean orbital elements
- Convert mean orbital elements to instantaneous orbital elements
- Calculate corresponding position and velocity vectors

## Main Interfaces

### Class Interface

```cpp
class J2Analytical : public J2J4Analytical
```

**Constructors:**

```cpp
J2Analytical(const ModOrbElem& modOrbElem, const TimePoint& epoch, double gm, double j2, double re);
J2Analytical(const Vector3d& pos, const Vector3d& vel, const TimePoint& epoch, double gm, double j2, double re);
```

### Function Interface

```cpp
AST_CORE_API err_t aJ2AnalyticalProp(double duration, double gm, double j2, double re, Vector3d& r, Vector3d& v);
AST_CORE_API err_t aJ2AnalyticalProp(double duration, double gm, double j2, double re, ModOrbElem& modOrbElem);
```

**Parameter Description:**

- `duration`: Propagation time interval (seconds)
- `gm`: Gravitational constant (m³/s²)
- `j2`: J2 coefficient (dimensionless)
- `re`: Body radius (meters)
- `r`: Input/output position vector (meters)
- `v`: Input/output velocity vector (m/s)
- `modOrbElem`: Input/output modified orbital elements

## Usage Examples

### 1. Position-Velocity Propagation

@[code](/examples/propagator/j2_analytical/j2_position_velocity_prop.cpp)

### 2. Orbital Elements Propagation

@[code](/examples/propagator/j2_analytical/j2_orbital_elements_prop.cpp)

### 3. Class Interface Usage

@[code](/examples/propagator/j2_analytical/j2_class_interface.cpp)

## Accuracy and Limitations

### Accuracy Characteristics

- **Calculation Accuracy**: Considers J2 term long-term perturbations, better than two-body propagator
- **Applicability**: Suitable for near-Earth orbits requiring Earth oblateness effects
- **Computational Efficiency**: High, suitable for real-time applications requiring higher accuracy

### Usage Limitations

- Only considers J2 term perturbations, ignores higher-order terms and other perturbation forces
- For high-precision orbit prediction requirements, recommend using numerical methods like HPOP
- Errors accumulate during long-term propagation due to model simplification

## Earth J2 Parameters

For Earth orbit calculations, recommended parameters:

- **J2 coefficient**: 1.08262668e-3
- **Earth radius**: 6378137.0 meters
- **Gravitational constant**: 3.986004418e14 m³/s²

## Notes

- Input position and velocity vectors should be in the same inertial coordinate system
- J2 coefficient and body radius should match the central body
- Propagation time interval should not be too long to avoid numerical error accumulation
- J2 perturbation effects are significant for polar orbits and critical inclination orbits

## API Reference

@[code](/src/AstCore/Propagator/J2Analytical.hpp)
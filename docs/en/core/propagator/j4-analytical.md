# J4 Analytical Propagator

*Note: This document is AI-translated.*

## Overview

The J4 analytical propagator provides orbit propagation considering Earth's J2 and J4 oblateness terms, offering higher precision orbit prediction than the J2 propagator. This method maintains high computational efficiency while accounting for more detailed Earth non-spherical gravitational field effects.

## Core Concepts

### J4 Perturbation Term

The J4 term is the second-order term in Earth's gravitational field spherical harmonic expansion, representing more detailed Earth shape characteristics. J4 perturbation further refines:

- **Orbital plane precession**: More accurate orbital plane precession
- **Perigee precession**: More precise perigee rotation
- **Higher-order effects**: Additional perturbation effects beyond J2

### Propagation Principle

The J4 analytical propagator is based on long-term variation rates of mean orbital elements considering both J2 and J4 terms:

- Calculate long-term variation rates of orbital elements caused by J2 and J4 perturbations
- Perform linear propagation of mean orbital elements
- Convert mean orbital elements to instantaneous orbital elements
- Calculate corresponding position and velocity vectors

## Main Interfaces

### Class Interface

```cpp
class J4Analytical : public J2J4Analytical
```

**Constructors:**

```cpp
J4Analytical(const ModOrbElem& modOrbElem, const TimePoint& epoch, double gm, double j2, double j4, double re);
J4Analytical(const Vector3d& pos, const Vector3d& vel, const TimePoint& epoch, double gm, double j2, double j4, double re);
```

### Function Interface

```cpp
AST_CORE_API err_t aJ4AnalyticalProp(double duration, double gm, double j2, double j4, double re, Vector3d& r, Vector3d& v);
AST_CORE_API err_t aJ4AnalyticalProp(double duration, double gm, double j2, double j4, double re, ModOrbElem& modOrbElem);
```

**Parameter Description:**

- `duration`: Propagation time interval (seconds)
- `gm`: Gravitational constant (m³/s²)
- `j2`: J2 coefficient (dimensionless)
- `j4`: J4 coefficient (dimensionless)
- `re`: Body radius (meters)
- `r`: Input/output position vector (meters)
- `v`: Input/output velocity vector (m/s)
- `modOrbElem`: Input/output modified orbital elements

## Usage Examples

### 1. Position-Velocity Propagation

@[code](/examples/propagator/j4_analytical/j4_position_velocity_prop.cpp)

### 2. Orbital Elements Propagation

@[code](/examples/propagator/j4_analytical/j4_orbital_elements_prop.cpp)

### 3. Class Interface Usage

@[code](/examples/propagator/j4_analytical/j4_class_interface.cpp)

## Accuracy and Limitations

### Accuracy Characteristics

- **Calculation Accuracy**: Considers J2 and J4 term long-term perturbations, better than J2 propagator
- **Applicability**: Suitable for near-Earth orbits requiring more detailed Earth shape effects
- **Computational Efficiency**: High, suitable for applications requiring higher accuracy than J2

### Usage Limitations

- Only considers J2 and J4 term perturbations, ignores higher-order terms and other perturbation forces
- For high-precision orbit prediction requirements, recommend using numerical methods like HPOP
- Errors accumulate during long-term propagation due to model simplification

## Earth J2/J4 Parameters

For Earth orbit calculations, recommended parameters:

- **J2 coefficient**: 1.08262668e-3
- **J4 coefficient**: -1.620e-6
- **Earth radius**: 6378137.0 meters
- **Gravitational constant**: 3.986004418e14 m³/s²

## Accuracy Comparison

Compared to J2 propagator, J4 propagator provides:

- More accurate perigee precession rate calculation
- Better orbital plane precession prediction
- Improved accuracy for orbits with specific inclinations

## Notes

- Input position and velocity vectors should be in the same inertial coordinate system
- J2/J4 coefficients and body radius should match the central body
- Propagation time interval should not be too long to avoid numerical error accumulation
- J4 effects are more significant for certain orbital inclinations and eccentricities

## API Reference

@[code](/src/AstCore/Propagator/J4Analytical.hpp)
# Orbit Elements Definition and Conversion

*Note: This document is AI-translated. Please refer to the source code for specific APIs and behaviors.*

## Module Overview

The OrbitElement module provides conversion functions between various orbit elements (Cartesian coordinates, classical orbit elements, modified orbit elements, equinoctial elements, and modified equinoctial elements). It is a fundamental component for astrodynamics and space dynamics calculations.

## Core Concepts

### Types of Orbit Elements

The module defines the following main types of orbit elements:

1. **Cartesian Coordinates (CartState)**
   - Contains position vector and velocity vector
   - Intuitive representation of spatial position and motion state

2. **Classical Orbit Elements (OrbElem)**
   - Semi-major axis (a)
   - Eccentricity (e)
   - Inclination (i)
   - Right ascension of ascending node (raan)
   - Argument of perigee (argper)
   - True anomaly (trueA)

3. **Modified Orbit Elements (ModOrbElem)**
   - Perigee radius (rp)
   - Eccentricity (e)
   - Inclination (i)
   - Right ascension of ascending node (raan)
   - Argument of perigee (argper)
   - True anomaly (trueA)

4. **Equinoctial Elements (EquinElem)**
   - Semi-major axis (a)
   - h = e*sin(argper + raan)
   - k = e*cos(argper + raan)
   - p = tan(i/2)*sin(raan)
   - q = tan(i/2)*cos(raan)
   - lambda = mean longitude = M + raan + argper

5. **Modified Equinoctial Elements (ModEquinElem)**
   - Semi-latus rectum (p = a(1-e²))
   - f = e*cos(argper+raan)
   - g = e*sin(argper+raan)
   - h = tan(i/2)cos(raan)
   - k = tan(i/2)sin(raan)
   - L = raan + argper + trueA

### Key Constants and Units

- Position unit: meter (m)
- Velocity unit: meter/second (m/s)
- Angle unit: radian (rad)
- Gravitational parameter unit: cubic meter per second squared (m³/s²)

## Usage Examples

### 1. Conversion Between Classical Orbit Elements and Cartesian Coordinates

@[code](/examples/core/orbitelement/convert_classical_scalar.cpp)

### 2. Using Class Reference Version Functions

@[code](/examples/core/orbitelement/convert_classical_class.cpp)

### 3. Conversion Between Different Orbit Element Types

@[code](/examples/core/orbitelement/convert_basic.cpp)

## Dependencies

- `AstGlobal.h`: Project global definitions
- `AstCore/Vector.hpp`: Vector class support

## Notes

- All angle parameters use radians
- Distance parameters default to meters (m)
- Time parameters default to seconds (s)
- Gravitational parameter unit is m³/s²
- Some conversion functions return error codes, please handle error cases appropriately
- Zero eccentricity orbits (circular orbits) may require special handling in some conversions
- Modified equinoctial elements may have singularity issues when the orbit inclination is close to 180 degrees

## API Reference

@[code](/src/AstCore/Orbit/OrbitElement.hpp)
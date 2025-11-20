---
title: OrbitElement - Orbital Element Definitions and Conversions
description: Comprehensive library for orbital element conversions including Cartesian coordinates, classical elements, modified elements, equinoctial elements, and modified equinoctial elements
---

# Orbit Element Conversion Module

## Overview

The OrbitElement module provides a comprehensive set of classes and functions for converting between various orbital element representations used in celestial mechanics and astrodynamics. This module supports transformations between Cartesian coordinates, classical orbital elements, modified orbital elements, equinoctial elements, and modified equinoctial elements, forming the foundation for complex orbital computations.

## Core Concepts

### Orbital Element Types

The module defines several key orbital element types:

1. **Cartesian State (CartState)**
   - Position vector
   - Velocity vector
   - Provides a direct spatial representation of an object's position and motion

2. **Classical Orbital Elements (OrbElem)**
   - Semi-major axis (a)
   - Eccentricity (e)
   - Inclination (i)
   - Right ascension of the ascending node (raan)
   - Argument of perigee (argper)
   - True anomaly (trueA)

3. **Modified Orbital Elements (ModOrbElem)**
   - Perigee radius (rp)
   - Eccentricity (e)
   - Inclination (i)
   - Right ascension of the ascending node (raan)
   - Argument of perigee (argper)
   - True anomaly (trueA)

4. **Equinoctial Elements (EquinElem)**
   - Semi-major axis (a)
   - h = e*sin(omega + raan)
   - k = e*cos(omega + raan)
   - p = tan(i/2)*sin(raan)
   - q = tan(i/2)*cos(raan)
   - lambda = mean longitude = M + raan + argper

5. **Modified Equinoctial Elements (ModEquinElem)**
   - Semi-parameter (p = a(1-e^2))
   - f = e*cos(argper+RAAN)
   - g = e*sin(argper+RAAN)
   - h = tan(i/2)cos(RAAN)
   - k = tan(i/2)sin(RAAN)
   - L = RAAN + argper + trueA

### Units and Constants

- Position: Meters (m)
- Velocity: Meters per second (m/s)
- Angles: Radians (rad)
- Gravitational parameter: Cubic meters per second squared (m³/s²)

## Usage Examples

### Example 1: Classical Orbital Elements to Cartesian Conversion

```cpp
#include "AstCore/Orbit/OrbitElement.hpp"
#include <iostream>

using namespace AST_NAMESPACE;

int main() {
    // Earth's gravitational parameter (m^3/s^2)
    double gmEarth = 3.986004418e14;
    
    // Define classical orbital elements [a, e, i, raan, argper, trueA]
    double coe[6] = {7000000.0, 0.01, 0.0, 0.0, 0.0, 0.0};
    
    // Convert to Cartesian coordinates
    double pos[3], vel[3];
    err_t err = coe2rv(coe, gmEarth, pos, vel);
    
    if (err == eNoError) {
        std::cout << "Position vector: [" << pos[0] << ", " << pos[1] << ", " << pos[2] << "]\n";
        std::cout << "Velocity vector: [" << vel[0] << ", " << vel[1] << ", " << vel[2] << "]\n";
        
        // Convert back from Cartesian to classical orbital elements
        double coeBack[6];
        err = rv2coe(pos, vel, gmEarth, coeBack);
        
        if (err == eNoError) {
            std::cout << "Semi-major axis: " << coeBack[0] << " m\n";
            std::cout << "Eccentricity: " << coeBack[1] << std::endl;
        }
    }
    
    return 0;
}
```

### Example 2: Using Class Reference Functions

```cpp
#include "AstCore/Orbit/OrbitElement.hpp"
#include <iostream>

using namespace AST_NAMESPACE;

int main() {
    // Earth's gravitational parameter (m^3/s^2)
    double gmEarth = 3.986004418e14;
    
    // Define position and velocity vectors
    Vector3d pos(6778137.0, 0.0, 0.0);  // GEO position
    Vector3d vel(0.0, 7726.0, 0.0);      // GEO velocity
    
    // Convert to classical orbital elements
    OrbElem elem;
    err_t err = aCartToOrbElem(pos, vel, gmEarth, elem);
    
    if (err == eNoError) {
        std::cout << "Semi-major axis: " << elem.a() << " m\n";
        std::cout << "Eccentricity: " << elem.e() << "\n";
        std::cout << "Inclination: " << elem.i() << " rad\n";
        
        // Convert back from classical elements to Cartesian
        Vector3d posBack, velBack;
        err = aOrbElemToCart(elem, gmEarth, posBack, velBack);
        
        if (err == eNoError) {
            std::cout << "Position vector: [" << posBack[0] << ", " << posBack[1] << ", " << posBack[2] << "]\n";
        }
    }
    
    return 0;
}
```

### Example 3: Converting Between Different Element Types

```cpp
#include "AstCore/Orbit/OrbitElement.hpp"
#include <iostream>

using namespace AST_NAMESPACE;

int main() {
    // Define classical orbital elements
    OrbElem coe;
    coe.a() = 7000000.0;  // 7000 km
    coe.e() = 0.1;
    coe.i() = 0.0;        // 0 radians
    coe.raan() = 0.0;
    coe.argper() = 0.0;
    coe.trueA() = 0.0;
    
    // Convert to equinoctial elements
    EquinElem ee;
    coe2ee(coe.data(), ee.data());
    
    // Convert to modified orbital elements
    ModOrbElem moe;
    coe2moe(coe.data(), moe.data());
    
    // Convert to modified equinoctial elements
    ModEquinElem mee;
    coe2mee(coe.data(), mee.data());
    
    std::cout << "Conversion complete\n";
    std::cout << "Equinoctial elements semi-major axis: " << ee.a() << " m\n";
    std::cout << "Modified orbital elements perigee radius: " << moe.rp() << " m\n";
    
    return 0;
}
```

## API Reference

### Orbital Element Classes

#### CartState - Cartesian State

```cpp
class CartState {
public:
    Vector3d pos_;   ///< Position
    Vector3d vel_;   ///< Velocity
public:
    A_DEF_POD_ITERABLE(double)
    const Vector3d& pos() const {return pos_;} 
    Vector3d& pos() {return pos_;}
    const Vector3d& vel() const {return vel_;} 
    Vector3d& vel() {return vel_;}
};
```

#### OrbElem - Classical Orbital Elements

```cpp
class OrbElem {
public:
    double	 a_;          ///< Semi-major axis 
    double	 e_;          ///< Eccentricity
    double	 i_;          ///< Inclination 
    double	 raan_;       ///< Right ascension of ascending node 
    double	 argper_;     ///< Argument of perigee
    double	 trueA_;      ///< True anomaly
public:
    A_DEF_POD_ITERABLE(double)
    AST_DEF_ACCESS_METHOD(double, a)
    AST_DEF_ACCESS_METHOD(double, e)
    AST_DEF_ACCESS_METHOD(double, i)
    AST_DEF_ACCESS_METHOD(double, raan)
    AST_DEF_ACCESS_METHOD(double, argper)
    AST_DEF_ACCESS_METHOD(double, trueA)
};
```

#### ModOrbElem - Modified Orbital Elements

```cpp
class ModOrbElem {
public:
    double p_;         ///< Perigee radius 
    double e_;          ///< Eccentricity 
    double i_;          ///< Inclination
    double raan_;       ///< Right ascension of ascending node
    double argper_;     ///< Argument of perigee
    double trueA_;      ///< True anomaly
public:
    A_DEF_POD_ITERABLE(double)
    AST_DEF_ACCESS_METHOD(double, p)
    AST_DEF_ACCESS_METHOD(double, e)
    AST_DEF_ACCESS_METHOD(double, i)
    AST_DEF_ACCESS_METHOD(double, raan)
    AST_DEF_ACCESS_METHOD(double, argper)
    AST_DEF_ACCESS_METHOD(double, trueA)
};
```

#### EquinElem - Equinoctial Elements

```cpp
class EquinElem {
public:
    double a_;          ///< Semimajor axis length
    double h_;          ///< e*sin(argper + raan)   omegabar=argper + raan
    double k_;          ///< e*cos(argper + raan)
    double p_;          ///< tan(i/2)*sin(raan)
    double q_;          ///< tan(i/2)*cos(raan)
    double lambda_;     ///< Mean longitude = M + raan + argper
public:
    A_DEF_POD_ITERABLE(double)
    AST_DEF_ACCESS_METHOD(double, a)
    AST_DEF_ACCESS_METHOD(double, h)
    AST_DEF_ACCESS_METHOD(double, k)
    AST_DEF_ACCESS_METHOD(double, p)
    AST_DEF_ACCESS_METHOD(double, q)
    AST_DEF_ACCESS_METHOD(double, lambda)
};
```

#### ModEquinElem - Modified Equinoctial Elements

```cpp
class ModEquinElem {
public:
    double rp_;       ///< p = a(1-e^2) Semi-parameter
    double f_;        ///< f = e*cos(argper+RAAN)
    double g_;        ///< g = e*sin(argper+RAAN)
    double h_;        ///< h = tan(i/2)cos(RAAN)
    double k_;        ///< k = tan(i/2)sin(RAAN)
    double L_;        ///< L = RAAN + argper + trueA
public:
    A_DEF_POD_ITERABLE(double)
    AST_DEF_ACCESS_METHOD(double, rp)
    AST_DEF_ACCESS_METHOD(double, f)
    AST_DEF_ACCESS_METHOD(double, g)
    AST_DEF_ACCESS_METHOD(double, h)
    AST_DEF_ACCESS_METHOD(double, k)
    AST_DEF_ACCESS_METHOD(double, L)
};
```

### Core Conversion Functions

#### Classical Orbital Elements Conversions

| Function | Description | Parameters | Return Value |
|----------|-------------|------------|--------------|
| `coe2rv` | Convert classical elements to Cartesian | coe: Classical elements array<br>gm: Gravitational parameter [m³/s²]<br>pos: Output position vector [m]<br>vel: Output velocity vector [m/s] | Error code, eNoError on success |
| `rv2coe` | Convert Cartesian to classical elements | pos: Position vector [m]<br>vel: Velocity vector [m/s]<br>gm: Gravitational parameter [m³/s²]<br>coe: Output classical elements | Error code, eNoError on success |
| `coe2moe` | Convert classical elements to modified elements | coe: Classical elements array<br>moe: Output modified elements | None |
| `moe2coe` | Convert modified elements to classical elements | moe: Modified elements array<br>coe: Output classical elements | None |
| `coe2ee` | Convert classical elements to equinoctial elements | coe: Classical elements array<br>ee: Output equinoctial elements | None |
| `ee2coe` | Convert equinoctial elements to classical elements | ee: Equinoctial elements array<br>coe: Output classical elements | None |
| `coe2mee` | Convert classical elements to modified equinoctial | coe: Classical elements array<br>mee: Output modified equinoctial | Error code, eNoError on success |

#### Modified Orbital Elements Conversions

| Function | Description | Parameters | Return Value |
|----------|-------------|------------|--------------|
| `moe2rv` | Convert modified elements to Cartesian | moe: Modified elements array<br>gm: Gravitational parameter [m³/s²]<br>pos: Output position vector [m]<br>vel: Output velocity vector [m/s] | None |
| `rv2moe` | Convert Cartesian to modified elements | pos: Position vector [m]<br>vel: Velocity vector [m/s]<br>gm: Gravitational parameter [m³/s²]<br>moe: Output modified elements | Error code, eNoError on success |
| `moe2ee` | Convert modified elements to equinoctial elements | moe: Modified elements array<br>ee: Output equinoctial elements | Error code, eNoError on success |
| `ee2moe` | Convert equinoctial elements to modified elements | ee: Equinoctial elements array<br>moe: Output modified elements | Error code, eNoError on success |
| `moe2mee` | Convert modified elements to modified equinoctial | moe: Modified elements array<br>mee: Output modified equinoctial | None |

#### Equinoctial Elements Conversions

| Function | Description | Parameters | Return Value |
|----------|-------------|------------|--------------|
| `ee2rv` | Convert equinoctial elements to Cartesian | ee: Equinoctial elements array<br>gm: Gravitational parameter [m³/s²]<br>pos: Output position vector [m]<br>vel: Output velocity vector [m/s] | None |
| `rv2ee` | Convert Cartesian to equinoctial elements | pos: Position vector [m]<br>vel: Velocity vector [m/s]<br>gm: Gravitational parameter [m³/s²]<br>ee: Output equinoctial elements | None |
| `ee2mee` | Convert equinoctial elements to modified equinoctial | ee: Equinoctial elements array<br>mee: Output modified equinoctial | None |
| `mee2ee` | Convert modified equinoctial to equinoctial elements | mee: Modified equinoctial array<br>ee: Output equinoctial elements | None |

#### Modified Equinoctial Elements Conversions

| Function | Description | Parameters | Return Value |
|----------|-------------|------------|--------------|
| `mee2rv` | Convert modified equinoctial to Cartesian | mee: Modified equinoctial array<br>gm: Gravitational parameter [m³/s²]<br>pos: Output position vector [m]<br>vel: Output velocity vector [m/s] | None |
| `rv2mee` | Convert Cartesian to modified equinoctial | pos: Position vector [m]<br>vel: Velocity vector [m/s]<br>gm: Gravitational parameter [m³/s²]<br>mee: Output modified equinoctial | Error code, eNoError on success |
| `mee2coe` | Convert modified equinoctial to classical elements | mee: Modified equinoctial array<br>coe: Output classical elements | Error code, eNoError on success |
| `mee2moe` | Convert modified equinoctial to modified elements | mee: Modified equinoctial array<br>moe: Output modified elements | None |

### Class Reference Conversion Functions

#### Modified Equinoctial Elements Conversion Functions

```cpp
// Convert modified equinoctial elements to Cartesian
void aModEquinElemToCart(const ModEquinElem& mee, double gm, Vector3d& pos, Vector3d& vel);

// Convert Cartesian to modified equinoctial elements
err_t aCartToModEquinElem(const Vector3d& pos, const Vector3d& vel, double gm, ModEquinElem& mee);

// Convert classical elements to modified equinoctial elements
err_t aOrbElemToModEquinElem(const OrbElem& elem, ModEquinElem& mee);

// Convert modified equinoctial elements to classical elements
err_t aModEquinElemToOrbElem(const ModEquinElem& mee, OrbElem& elem);
```

#### Classical Orbital Elements Conversion Functions

```cpp
// Convert Cartesian to classical orbital elements
err_t aCartToOrbElem(const Vector3d& pos, const Vector3d& vel, double gm, OrbElem& elem);

// Convert classical orbital elements to Cartesian
err_t aOrbElemToCart(const OrbElem& elem, double gm, Vector3d& pos, Vector3d& vel);
```

#### Modified Orbital Elements Conversion Functions

```cpp
// Convert Cartesian to modified orbital elements
err_t aCartToModOrbElem(const Vector3d& pos, const Vector3d& vel, double gm, ModOrbElem& modOrb);

// Convert modified orbital elements to Cartesian
void aModOrbElemToCart(const ModOrbElem& modOrb, double gm, Vector3d& pos, Vector3d& vel);

// Convert equinoctial elements to modified orbital elements
err_t aEquinElemToModOrb(const EquinElem& equinElem, ModOrbElem& modOrb);

// Convert modified orbital elements to equinoctial elements
err_t aModOrbToEquinElem(const ModOrbElem& modOrb, EquinElem& equinElem);
```

#### Equinoctial Elements Conversion Functions

```cpp
// Convert Cartesian to equinoctial elements
void aCartToEquinElem(const Vector3d& pos, const Vector3d& vel, double gm, EquinElem& equinElem);

// Convert equinoctial elements to Cartesian
void aEquinElemToCart(const EquinElem& equinElem, double gm, Vector3d& pos, Vector3d& vel);
```

## Dependencies

- `AstGlobal.hpp`: Project global definitions
- `AstCore/Vector.hpp`: Vector class support

## Notes

- All angle parameters use radians
- Distance parameters default to meters (m)
- Time parameters default to seconds (s)
- Gravitational parameter is in m³/s²
- Some conversion functions return error codes - handle these appropriately
- Circular orbits (zero eccentricity) may require special handling in certain conversions
- Modified equinoctial elements can experience singularities near 180° inclination
- Ensure proper error handling for conversions that return error codes

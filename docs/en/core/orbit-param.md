---
title: Orbit Parameter Conversion
description: Comprehensive set of functions for converting between different orbital parameters including radii, altitudes, periods, mean motion, and anomalies
---

# Orbit Parameter Conversion Module

## Overview

The Orbit Parameter Conversion module provides a comprehensive set of functions for converting between various orbital parameters used in celestial mechanics and astrodynamics. This module enables seamless transformations between different representations of orbital elements, making it essential for satellite mission design, orbit determination, and space mission planning applications.

## Core Concepts

### Orbital Elements Types

The module supports conversions between multiple orbital parameter types:

1. **Radius and Altitude Parameters**
   - Perigee/Apogee Radius
   - Perigee/Apogee Altitude
   - Semi-Major Axis
   - Semi-Minor Axis
   - Semi-Parameter (Semi-Latus Rectum)

2. **Time and Motion Parameters**
   - Orbital Period
   - Mean Motion
   - Time Since Periapsis Passage (TPP)
   - Time of Periapsis Angle (TPAN)

3. **Anomaly Representations**
   - True Anomaly
   - Mean Anomaly
   - Eccentric Anomaly

4. **Angular Relationships**
   - Argument of Latitude
   - True Longitude of Periapsis
   - Longitude of Periapsis

5. **Shape Parameters**
   - Eccentricity
   - Flattening Factor

### Key Constants

- **Central Body Radius (cbRadius)**: The radius of the central celestial body [m]
- **Gravitational Parameter (gm)**: The gravitational constant multiplied by the mass of the central body [m³/s²]
- **Central Body Rotation Rate (cbRotRate)**: The angular velocity of the central body's rotation [rad/s]

## Usage Examples

### Example 1: Basic Radius and Altitude Conversions

```cpp
#include "AstCore/OrbitParam.hpp"
#include <iostream>

using namespace AST_NAMESPACE;

int main() {
    // Constants for Earth
    const double earth_radius = 6378137.0; // [m]
    const double earth_gm = 398600.4418e9; // [m^3/s^2]
    
    // Define orbital parameters
    double perigee_alt = 200000.0; // 200 km [m]
    double eccentricity = 0.05;
    
    // Convert perigee altitude to radius
    double perigee_rad = aPeriAltToPeriRad(perigee_alt, earth_radius);
    std::cout << "Perigee Radius: " << perigee_rad << " m" << std::endl;
    
    // Convert perigee radius to apogee radius
    double apogee_rad = aPeriRadToApoRad(perigee_rad, eccentricity);
    std::cout << "Apogee Radius: " << apogee_rad << " m" << std::endl;
    
    // Convert apogee radius to apogee altitude
    double apogee_alt = aApoRadToApoAlt(apogee_rad, earth_radius);
    std::cout << "Apogee Altitude: " << apogee_alt << " m" << std::endl;
    
    return 0;
}
```

### Example 2: Period and Mean Motion Calculations

```cpp
#include "AstCore/OrbitParam.hpp"
#include <iostream>

using namespace AST_NAMESPACE;

int main() {
    // Constants for Earth
    const double earth_radius = 6378137.0; // [m]
    const double earth_gm = 398600.4418e9; // [m^3/s^2]
    
    // Semi-major axis for a near-circular Earth orbit (700 km altitude)
    double semi_major_axis = earth_radius + 700000.0; // [m]
    
    // Calculate orbital period
    double period = aSMajAxToPeriod(semi_major_axis, earth_gm);
    std::cout << "Orbital Period: " << period << " seconds" << std::endl;
    std::cout << "Orbital Period: " << period / 60.0 << " minutes" << std::endl;
    
    // Calculate mean motion
    double mean_motion = aSMajAxToMeanMotn(semi_major_axis, earth_gm);
    std::cout << "Mean Motion: " << mean_motion << " rad/s" << std::endl;
    std::cout << "Mean Motion: " << mean_motion * 86400.0 / (2 * M_PI) << " revolutions/day" << std::endl;
    
    // Convert mean motion back to period
    double period_from_mm = aMeanMotnToPeriod(mean_motion);
    std::cout << "Period from Mean Motion: " << period_from_mm << " seconds" << std::endl;
    
    return 0;
}
```

### Example 3: Anomaly Conversions

```cpp
#include "AstCore/OrbitParam.hpp"
#include <iostream>

using namespace AST_NAMESPACE;

int main() {
    // Define anomalies
    double eccentricity = 0.1;
    double true_anomaly = M_PI / 4.0; // 45 degrees in radians
    
    // Convert true anomaly to eccentric anomaly
    double ecc_anomaly = aTrueToEcc(true_anomaly, eccentricity);
    std::cout << "Eccentric Anomaly: " << ecc_anomaly << " rad" << std::endl;
    
    // Convert eccentric anomaly to mean anomaly
    double mean_anomaly = aEccToMean(ecc_anomaly, eccentricity);
    std::cout << "Mean Anomaly: " << mean_anomaly << " rad" << std::endl;
    
    // Convert mean anomaly back to true anomaly (full cycle)
    double true_anomaly_back = aMeanToTrue(mean_anomaly, eccentricity);
    std::cout << "True Anomaly (back): " << true_anomaly_back << " rad" << std::endl;
    
    return 0;
}
```

### Example 4: Complete Orbital Analysis

```cpp
#include "AstCore/OrbitParam.hpp"
#include <iostream>

using namespace AST_NAMESPACE;

int main() {
    // Constants for Earth
    const double earth_radius = 6378137.0; // [m]
    const double earth_gm = 398600.4418e9; // [m^3/s^2]
    
    // Input parameters
    double perigee_alt = 300000.0; // 300 km [m]
    double apogee_alt = 35786000.0; // GEO altitude [m]
    
    // Calculate radii
    double perigee_rad = aPeriAltToPeriRad(perigee_alt, earth_radius);
    double apogee_rad = aApoAltToApoRad(apogee_alt, earth_radius);
    
    // Calculate eccentricity
    double eccentricity = aRadiiToEcc(perigee_rad, apogee_rad);
    std::cout << "Eccentricity: " << eccentricity << std::endl;
    
    // Calculate semi-major axis
    double semi_major_axis = aPeriRadToSMajAx(perigee_rad, eccentricity);
    std::cout << "Semi-Major Axis: " << semi_major_axis << " m" << std::endl;
    
    // Calculate orbital period
    double period = aSMajAxToPeriod(semi_major_axis, earth_gm);
    std::cout << "Orbital Period: " << period / 3600.0 << " hours" << std::endl;
    
    // Calculate mean motion
    double mean_motion = aPeriodToMeanMotn(period);
    std::cout << "Mean Motion: " << mean_motion * 86400.0 / (2 * M_PI) << " revolutions/day" << std::endl;
    
    return 0;
}
```

## API Reference

### Radius and Altitude Conversions

```cpp
// Apogee Altitude to other parameters
double aApoAltToApoRad(double apogeeAlt, double cbRadius);
double aApoAltToMeanMotn(double apogeeAlt, double eccentricity, double cbRadius, double gm);
double aApoAltToPeriAlt(double apogeeAlt, double eccentricity, double cbRadius);
double aApoAltToPeriRad(double apogeeAlt, double eccentricity, double cbRadius);
double aApoAltToPeriod(double apogeeAlt, double eccentricity, double cbRadius, double gm);
double aApoAltToSMajAx(double apogeeAlt, double eccentricity, double cbRadius);

// Apogee Radius to other parameters
double aApoRadToApoAlt(double apogeeRad, double cbRadius);
double aApoRadToMeanMotn(double apogeeRad, double eccentricity, double gm);
double aApoRadToPeriAlt(double apogeeRad, double eccentricity, double cbRadius);
double aApoRadToPeriod(double apogeeRad, double eccentricity, double gm);
double aApoRadToPeriRad(double apogeeRad, double eccentricity);
double aApoRadToSMajAx(double apogeeRad, double eccentricity);

// Perigee Altitude to other parameters
double aPeriAltToApoAlt(double perigeeAlt, double eccentricity, double cbRadius);
double aPeriAltToApoRad(double perigeeAlt, double eccentricity, double cbRadius);
double aPeriAltToMeanMotn(double perigeeAlt, double eccentricity, double cbRadius, double gm);
double aPeriAltToPeriRad(double perigeeAlt, double cbRadius);
double aPeriAltToPeriod(double perigeeAlt, double eccentricity, double cbRadius, double gm);
double aPeriAltToSMajAx(double perigeeAlt, double eccentricity, double cbRadius);

// Perigee Radius to other parameters
double aPeriRadToApoAlt(double perigeeRad, double eccentricity, double cbRadius);
double aPeriRadToApoRad(double perigeeRad, double eccentricity);
double aPeriRadToMeanMotn(double perigeeRad, double eccentricity, double gm);
double aPeriRadToPeriAlt(double perigeeRad, double cbRadius);
double aPeriRadToPeriod(double perigeeRad, double eccentricity, double gm);
double aPeriRadToSMajAx(double perigeeRad, double eccentricity);

// Semi-Major Axis to other parameters
double aSMajAxToApoAlt(double semiMajorAxis, double eccentricity, double cbRadius);
double aSMajAxToApoRad(double semiMajorAxis, double eccentricity);
double aSMajAxToMeanMotn(double semiMajorAxis, double gm);
double aSMajAxToPeriAlt(double semiMajorAxis, double eccentricity, double cbRadius);
double aSMajAxToPeriRad(double semiMajorAxis, double eccentricity);
double aSMajAxToPeriod(double semiMajorAxis, double gm);
double aSMajAxToSMinAx(double semiMajorAxis, double eccentricity);
double aSMajAxToSParam(double semiMajorAxis, double eccentricity);
double aSMinAxToSMajAx(double semiminorAxis, double eccentricity);
```

### Period and Mean Motion Conversions

```cpp
// Mean Motion conversions
double aMeanMotnToApoAlt(double meanMotion, double eccentricity, double cbRadius, double gm);
double aMeanMotnToApoRad(double meanMotion, double eccentricity, double gm);
double aMeanMotnToPeriAlt(double meanMotion, double eccentricity, double cbRadius, double gm);
double aMeanMotnToPeriRad(double meanMotion, double eccentricity, double gm);
double aMeanMotnToPeriod(double meanMotn);
double aMeanMotnToSMajAx(double meanMotn, double gm);

// Period conversions
double aPeriodToApoAlt(double period, double eccentricity, double cbRadius, double gm);
double aPeriodToApoRad(double period, double eccentricity, double gm);
double aPeriodToMeanMotn(double period);
double aPeriodToPeriAlt(double period, double eccentricity, double cbRadius, double gm);
double aPeriodToPeriRad(double period, double eccentricity, double gm);
double aPeriodToSMajAx(double period, double gm);
```

### Anomaly Conversions

```cpp
// Eccentric Anomaly conversions
double aEccToMean(double eccAnomaly, double eccentricity);
double aEccToTrue(double eccAnomaly, double eccentricity);
double aEccToTPAN(double eccAnomaly, double argPeri, double semiMajorAxis, double eccentricity, double gm);
double aEccToTPP(double eccAnomaly, double semiMajorAxis, double eccentricity, double gm);

// Mean Anomaly conversions
double aMeanToEcc(double meanAnomaly, double eccentricity, double eps = 1e-14, int maxIter = 100);
double aMeanToTPAN(double meanAnomaly, double argPeri, double semiMajorAxis, double eccentricity, double gm);
double aMeanToTPP(double meanAnomaly, double semiMajorAxis, double gm);
double aMeanToTrue(double meanAnomaly, double eccentricity, double eps = 1e-14, int maxIter = 100);

// True Anomaly conversions
double aTrueToEcc(double trueAnomaly, double eccentricity);
double aTrueToMean(double trueAnomaly, double eccentricity);
double aTrueToTPAN(double trueAnomaly, double argPeri, double semiMajorAxis, double eccentricity, double gm);
double aTrueToTPP(double trueAnomaly, double semiMajorAxis, double eccentricity, double gm);
double aTrueToArgLat(double trueAnomaly, double argPeri);
double aTrueToTrueLong(double trueAnomaly, double argPeri, double raan);
```

### Time of Periapsis Conversions

```cpp
// Time of Periapsis Angle (TPAN) conversions
double aTPANToEcc(double TPAN, double argPeri, double semiMajorAxis, double eccentricity, double gm, double eps = 1e-14, int maxIter = 100);
double aTPANToMean(double TPAN, double argPeri, double semiMajorAxis, double eccentricity, double gm);
double aTPANToTPP(double TPAN, double argPeri, double semiMajorAxis, double eccentricity, double gm);
double aTPANToTrue(double TPAN, double argPeri, double semiMajorAxis, double eccentricity, double gm, double eps = 1e-14, int maxIter = 100);

// Time of Periapsis Passage (TPP) conversions
double aTPPToEcc(double TPP, double semiMajorAxis, double eccentricity, double gm, double eps = 1e-14, int maxIter = 100);
double aTPPToMean(double TPP, double semiMajorAxis, double gm);
double aTPPToTrue(double TPP, double semiMajorAxis, double eccentricity, double gm, double eps = 1e-14, int maxIter = 100);
double aTPPToTPAN(double TPP, double argPeri, double semiMajorAxis, double eccentricity, double gm);
```

### Specialized Conversions

```cpp
// Eccentricity calculations
double aRadiiToEcc(double perigeeRad, double apogeeRad);
double aEccToFlat(double eccentricity);
double aFlatToEcc(double flatFactor);

// Angular relationships
double aArgPeriToLongPeri(double argPeri, double raan);

// Ground track repeat calculations
double aRepeatGrndTrk(int daysToRepeat, int revsToRepeat, double gm, double cbRotRate);
```

## Precision and Performance Considerations

### Iterative Methods

Several functions in this module use iterative methods (Newton-Raphson) for conversions, particularly:

- `aMeanToEcc`
- `aMeanToTrue`
- `aTPANToEcc`
- `aTPANToTrue`
- `aTPPToEcc`
- `aTPPToTrue`

These functions accept optional parameters for controlling convergence:
- `eps`: Convergence tolerance (default: 1e-14)
- `maxIter`: Maximum number of iterations (default: 100)

### Units

All functions use consistent units:
- Distances: Meters [m]
- Angles: Radians [rad]
- Time: Seconds [s]
- Angular velocity: Radians per second [rad/s]
- Gravitational parameter: Cubic meters per second squared [m³/s²]

### Error Handling

The module assumes valid input ranges for orbital parameters:
- Eccentricity must be in [0, 1) for elliptical orbits
- Radii must be greater than the central body radius
- Input values should be physically meaningful for astrodynamic calculations

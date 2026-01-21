# Orbit Parameter Calculation

*Note: This document is AI-translated. Please refer to the source code for specific APIs and behaviors.*

## Module Overview

The OrbitParam module provides conversion functions between various orbit parameters, supporting conversions between perigee/apogee altitude/radius, period, mean angular velocity, eccentricity, and conversions between anomalies (true anomaly, eccentric anomaly, mean anomaly).

## Core Concepts

### Types of Orbit Elements

The module supports calculation and conversion of the following main orbit elements:

1. **Perigee/Apogee Elements**
   - Perigee altitude/radius
   - Apogee altitude/radius
   - Perigee argument time/passing time

2. **Anomalies**
   - True Anomaly
   - Eccentric Anomaly
   - Mean Anomaly

3. **Orbit Geometry Parameters**
   - Semi-major axis
   - Eccentricity
   - Flattening
   - Orbit period
   - Mean angular velocity

4. **Angle Relationships**
   - Argument of perigee
   - Longitude of perigee
   - Argument
   - True longitude of perigee

### Key Constants

- All length units default to meters (m)
- All angle units default to radians (rad)
- All time units default to seconds (s)
- Iterative functions default to 1e-14 precision with maximum 100 iterations

## Usage Examples

### 1. Radius and Height Conversion

The following example shows how to convert between perigee/apogee radius and altitude:

@[code](/examples/core/orbitparam/radius_and_height.cpp)

### 2. Orbit Period and Angular Velocity Calculation

The following example shows how to calculate orbit period and mean angular velocity:

@[code](/examples/core/orbitparam/period_and_angvel.cpp)

### 3. Anomaly Conversion

@[code](/examples/core/orbitparam/anomaly_convert.cpp)

### 4. Complete Orbit Analysis

@[code](/examples/core/orbitparam/orbitparam_example.cpp)

## Dependencies

- `AstGlobal.h`: Project global definitions
- `math.h`: Mathematical function support
- Related mathematical operation modules

## Notes

- All angle parameters use radians
- Distance parameters default to meters (m)
- Time parameters default to seconds (s)
- Iterative calculation functions include precision and maximum iteration parameters, which can be adjusted according to actual needs
- For high eccentricity orbits, it may be necessary to increase the maximum number of iterations to ensure convergence
- Zero eccentricity orbits (circular orbits) have special handling logic to avoid division by zero errors

## API Reference

@[code](/src/AstCore/Orbit/OrbitParam.hpp)
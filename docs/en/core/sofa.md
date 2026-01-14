# SOFA Astronomical Calculation Module

## Module Overview

The SOFA module is the core astronomical calculation module in the ast project, implemented based on the SOFA (Standards of Fundamental Astronomy) library from the International Astronomical Union (IAU). This module provides a series of high-precision astronomical calculation functions for processing astronomical phenomena such as precession, nutation, and sidereal time.

The SOFA module has been adapted from the original SOFA library to seamlessly integrate with custom types in the ast project (such as TimePoint) and provides a more flexible extension mechanism.

## Core Concepts

### 1. SOFA Library Introduction

The SOFA library is an authoritative astronomical algorithm library published by the International Astronomical Union (IAU), containing algorithms and programs that implement various astronomical standard models. The SOFA module is implemented based on SOFA library version 2023-10-11.

### 2. Time Systems

The SOFA module involves multiple time systems:
- **TT (Terrestrial Time)**: Used for calculating long-term astronomical phenomena such as precession and nutation
- **TDB (Barycentric Dynamical Time)**: Theoretically used for high-precision astronomical calculations
- **UT1 (Universal Time)**: Used for calculating Earth rotation angle

### 3. Astronomical Standards

The SOFA module implements various IAU astronomical standards:
- **IAU1976**: Precession model
- **IAU1980**: Nutation model
- **IERS1996**: Nutation correction model
- **IAU1982**: Greenwich Mean Sidereal Time model
- **IAU1994**: Greenwich Apparent Sidereal Time model
- **IAU2000**: Earth Rotation Angle model

## Main Features

### 1. Precession Calculation

- `aPrecession_IAU1976`: Calculate precession angles according to IAU1976 specification

### 2. Nutation Calculation

- `aNutation_IAU1980`: Calculate nutation angles according to IAU1980 specification
- `aNutation_IERS1996`: Calculate nutation angles according to IERS1996 specification
- `aNutationFuncSet`: Set custom nutation angle calculation function
- `aNutationMethodSet`: Set nutation angle calculation method

### 3. Obliquity Calculation

- `aMeanObliquity_IAU1980`: Calculate mean obliquity of the ecliptic

### 4. Sidereal Time Calculation

- `aGMST_IAU1982`: Calculate Greenwich Mean Sidereal Time
- `aGAST_IAU1994`: Calculate Greenwich Apparent Sidereal Time
- `aEquationOfEquinoxes_IAU1994`: Calculate equation of equinoxes

### 5. Earth Rotation Calculation

- `aEarthRotationAngle_IAU2000`: Calculate Earth Rotation Angle

## Usage Examples

### 1. Precession Angle Calculation

@[code](/examples/core/sofa/precession.cpp)

### 2. Nutation Angle Calculation

@[code](/examples/core/sofa/nutation.cpp)

### 3. Greenwich Sidereal Time Calculation

@[code](/examples/core/sofa/gmst_gast.cpp)

## Dependencies

- **AstCore/TimePoint.hpp**: Time point type definition
- **AstUtil/Constants.h**: Physical constant definitions
- **AstUtil/Logger.hpp**: Logging functionality

## Notes

1. **Time System Differences**: Pay attention to distinguishing between different time systems such as TT, TDB, and UT1
2. **Precision Considerations**: The SOFA module provides high-precision calculations, but certain approximate calculations (such as directly using TT instead of TDB) may affect precision in extreme cases
3. **Custom Implementation**: You can set a custom nutation angle calculation function through `aNutationFuncSet`, but need to ensure interface consistency
4. **SOFA License**: The SOFA module is derived from the SOFA library and must comply with SOFA software license terms

## API Reference

### Precession Calculation

#### `aPrecession_IAU1976`

```cpp
void aPrecession_IAU1976(const TimePoint& tp, double& zeta, double& z, double& theta);
```

Calculate precession angles for a given time point according to IAU1976 specification.

**Parameters**:
- `tp`: Time point
- `zeta`: Output zeta precession angle (radians)
- `z`: Output z precession angle (radians)
- `theta`: Output theta precession angle (radians)

### Nutation Calculation

#### `aNutation`

```cpp
NutationFunc aNutation;
```

Nutation angle calculation function pointer, default pointing to `aNutation_IAU1980`.

#### `aNutationFuncSet`

```cpp
void aNutationFuncSet(NutationFunc func);
```

Set nutation angle calculation function pointer.

**Parameters**:
- `func`: Custom nutation angle calculation function pointer

#### `aNutationMethodSet`

```cpp
err_t aNutationMethodSet(ENutationMethod method);
```

Set nutation angle calculation method according to enum value.

**Parameters**:
- `method`: Nutation calculation method enum value

**Return Value**:
- Error code, 0 for success

### Obliquity Calculation

#### `aMeanObliquity_IAU1980`

```cpp
double aMeanObliquity_IAU1980(const TimePoint& tp);
```

Calculate mean obliquity of the ecliptic for a given time point according to IAU1980 specification.

**Parameters**:
- `tp`: Time point

**Return Value**:
- Mean obliquity of the ecliptic (radians)

### Sidereal Time Calculation

#### `aGMST_IAU1982`

```cpp
double aGMST_IAU1982(const TimePoint& tp);
```

Calculate Greenwich Mean Sidereal Time for a given time point according to IAU1982 specification.

**Parameters**:
- `tp`: Time point

**Return Value**:
- Greenwich Mean Sidereal Time (radians)

#### `aGAST_IAU1994`

```cpp
double aGAST_IAU1994(const TimePoint& tp);
```

Calculate Greenwich Apparent Sidereal Time for a given time point according to IAU1994 specification.

**Parameters**:
- `tp`: Time point

**Return Value**:
- Greenwich Apparent Sidereal Time (radians)

### Earth Rotation Calculation

#### `aEarthRotationAngle_IAU2000`

```cpp
double aEarthRotationAngle_IAU2000(const TimePoint& tp);
```

Calculate Earth Rotation Angle for a given time point according to IAU2000 specification.

**Parameters**:
- `tp`: Time point

**Return Value**:
- Earth Rotation Angle (radians)

## Version History

- **v1.0.0**: Implemented based on SOFA library version 2023-10-11
- Initial version, including core functions for precession, nutation, sidereal time, etc.

## License Statement

The SOFA module is derived from the SOFA library of the International Astronomical Union (IAU) and must comply with the SOFA software license terms. For details, please refer to the license statement in the source code files.
---
title: Custom Literals Support
description: Comprehensive support for unit conversions using custom literals
---

# Custom Literals Support

## Overview

The `Literals.hpp` module provides a comprehensive set of custom literal operators for unit conversions in the ast project. These operators make unit conversions in code more intuitive and readable. All conversions are based on constants defined in `Constants.h`.

## Features

### Supported Unit Types

#### Angle Units
- **Radians**: `_rad` - Direct representation of radians
- **Degrees**: `_deg` - Conversion from degrees to radians

#### Length Units
- **Meters**: `_m` - Direct representation of meters
- **Kilometers**: `_km` - Conversion from kilometers to meters
- **Centimeters**: `_cm` - Conversion from centimeters to meters
- **Millimeters**: `_mm` - Conversion from millimeters to meters
- **Micrometers**: `_um` - Conversion from micrometers to meters
- **Nanometers**: `_nm` - Conversion from nanometers to meters
- **Astronomical Units**: `_au` - Conversion from astronomical units to meters

#### Time Units
- **Seconds**: `_s` - Direct representation of seconds
- **Minutes**: `_min` - Conversion from minutes to seconds
- **Hours**: `_h` - Conversion from hours to seconds
- **Days**: `_day` - Conversion from days to seconds
- **Milliseconds**: `_ms` - Conversion from milliseconds to seconds
- **Microseconds**: `_us` - Conversion from microseconds to seconds
- **Nanoseconds**: `_ns` - Conversion from nanoseconds to seconds
- **Picoseconds**: `_ps` - Conversion from picoseconds to seconds

#### Velocity Units
- **Meters per Second**: `_m_s` - Direct representation of m/s
- **Kilometers per Second**: `_km_s` - Conversion from km/s to m/s
- **Kilometers per Hour**: `_km_h` - Conversion from km/h to m/s

#### Angular Velocity Units
- **Radians per Second**: `_rad_s` - Direct representation of rad/s
- **Degrees per Second**: `_deg_s` - Conversion from deg/s to rad/s

### Type Support

All literal operators are provided in two overloads:
- Long double precision (`long double`)
- Unsigned long integer (`unsigned long long`)

## Usage Examples

### Angle Unit Examples

```cpp
#include "AstUtil/Literals.hpp"

using namespace AST_NAMESPACE;

// Angle conversion examples
double angle1 = 90.0_deg;      // 90 degrees converted to radians (approximately 1.5708 rad)
double angle2 = 3.14159_rad;   // Direct use of radians
double quarter = 0.25 * 360_deg; // 90 degrees
```

### Length Unit Examples

```cpp
// Length conversion examples
double distance1 = 5.5_km;     // 5.5 kilometers converted to meters (5500 meters)
double distance2 = 1000_cm;    // 1000 centimeters converted to meters (10 meters)
double distance3 = 1.0_au;     // 1 astronomical unit converted to meters
double thickness = 0.5_mm;     // 0.5 millimeters converted to meters
```

### Time Unit Examples

```cpp
// Time conversion examples
double time1 = 24_h;           // 24 hours converted to seconds (86400 seconds)
double time2 = 30_min;         // 30 minutes converted to seconds (1800 seconds)
double time3 = 500_ms;         // 500 milliseconds converted to seconds (0.5 seconds)
double shortDelay = 10_us;     // 10 microseconds converted to seconds
```

### Velocity Unit Examples

```cpp
// Velocity conversion examples
double speed1 = 300000_km_s;   // 300000 kilometers per second converted to m/s
double speed2 = 100_km_h;      // 100 kilometers per hour converted to m/s (approximately 27.78 m/s)
double speed3 = 25_m_s;        // Direct use of m/s
```

### Angular Velocity Unit Examples

```cpp
// Angular velocity conversion examples
double angSpeed1 = 30_deg_s;   // 30 degrees per second converted to rad/s
double angSpeed2 = 0.1_rad_s;  // Direct use of rad/s
```

### Practical Calculation Examples

```cpp
// Calculation examples
double speed = 100_km / 1_h;   // Calculate speed (approximately 27.78 m/s)
double frequency = 1.0 / 1_ms; // Calculate frequency (1000 Hz)
double circumference = 2.0 * kPI * 1.0_km; // Calculate circumference
```

## Design Notes

### Base Unit System

- **Angles**: Internal calculations use radians
- **Lengths**: Internal calculations use meters
- **Time**: Internal calculations use seconds
- **Velocity**: Internal calculations use meters per second
- **Angular Velocity**: Internal calculations use radians per second

### Namespace

All literal operators are defined in the `ast::literals` namespace and imported into the `ast` namespace via `using namespace ast::literals;` for convenient direct use.

### Compile-time Calculation

All literal operators use the `constexpr` keyword, supporting compile-time calculations for improved performance.

### Precision Considerations

The `double` type is used as the return value, providing sufficient precision for astronomical calculations.

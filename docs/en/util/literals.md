# Custom Literals Support

*Note: This document aims to help understand the module functionality. Please refer to the source code for specific APIs and behaviors.*

## Overview

`Literals.hpp` is the custom literals support module in the ast project, providing a series of custom literal operators for unit conversion, making unit conversions in code more intuitive and readable. All conversions are based on constants defined in `Constants.h`.

## Features

### Supported Unit Types

#### Angle Units
- **Radian**: `_rad` - Directly represents radian values
- **Degree**: `_deg` - Converts degrees to radians

#### Length Units
- **Meter**: `_m` - Directly represents meter values
- **Kilometer**: `_km` - Converts kilometers to meters
- **Centimeter**: `_cm` - Converts centimeters to meters
- **Millimeter**: `_mm` - Converts millimeters to meters
- **Micrometer**: `_um` - Converts micrometers to meters
- **Nanometer**: `_nm` - Converts nanometers to meters
- **Astronomical Unit**: `_au` - Converts astronomical units to meters

#### Time Units
- **Second**: `_s` - Directly represents second values
- **Minute**: `_min` - Converts minutes to seconds
- **Hour**: `_h` - Converts hours to seconds
- **Day**: `_day` - Converts days to seconds
- **Millisecond**: `_ms` - Converts milliseconds to seconds
- **Microsecond**: `_us` - Converts microseconds to seconds
- **Nanosecond**: `_ns` - Converts nanoseconds to seconds
- **Picosecond**: `_ps` - Converts picoseconds to seconds

#### Speed Units
- **Meter/Second**: `_m_s` - Directly represents meter/second values
- **Kilometer/Second**: `_km_s` - Converts kilometers/second to meters/second
- **Kilometer/Hour**: `_km_h` - Converts kilometers/hour to meters/second

#### Angular Velocity Units
- **Radian/Second**: `_rad_s` - Directly represents radian/second values
- **Degree/Second**: `_deg_s` - Converts degrees/second to radians/second

### Type Support

All literal operators provide two overloads:
- Long double (`long double`)
- Unsigned long long (`unsigned long long`)

## Usage Examples

### Angle Unit Example

@[code](/examples/util/literals/angle.cpp)

### Length Unit Example

@[code](/examples/util/literals/length.cpp)

### Time Unit Example

@[code](/examples/util/literals/time.cpp)

### Speed Unit Example

@[code](/examples/util/literals/speed.cpp)

### Angular Velocity Unit Example

@[code](/examples/util/literals/angvel.cpp)

### Practical Calculation Example

@[code](/examples/util/literals/literals_ex1.cpp)

## Design Notes

### Unit System Foundation

- **Angle**: Internally uses radians
- **Length**: Internally uses meters
- **Time**: Internally uses seconds
- **Speed**: Internally uses meters/second
- **Angular Velocity**: Internally uses radians/second

### Namespace

All literal operators are defined in the `ast::literals` namespace and can be easily used by importing with `using namespace ast::literals;`.

### Compile-Time Calculation

All literal operators use the `constexpr` keyword, supporting compile-time calculations for improved performance.

### Precision Considerations

Uses `double` type as the return value, providing sufficient precision for astronomical calculations.
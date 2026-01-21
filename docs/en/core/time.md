# Time Module

*Note: This document is AI-translated. For specific APIs and behaviors, please refer to the source code.*

## Module Overview

The Time module provides comprehensive time handling functionality, including date, time, date-time, Julian Date, Modified Julian Date, duration, and time system conversions. This module is a fundamental component for astrodynamics and aerospace dynamics calculations, supporting conversions and operations between various time representations.

## Core Concepts

### Time Representation Types

The module defines the following main time representation types:

1. **Date**
   - Represents year, month, day information
   - Supports leap year checking, month day calculation, day of year calculation, etc.

2. **Time**
   - Represents hour, minute, second information
   - Supports time normalization, total second calculation, etc.

3. **DateTime**
   - Combines date and time information
   - Supports local time to UTC conversion, date-time normalization, etc.

4. **JulianDate**
   - Time representation method used in astronomy
   - Supports conversion between date-time and Julian Date

5. **ModJulianDate (Modified Julian Date)**
   - Simplified form of Julian Date
   - Based on JulianDate class, providing a more concise representation

6. **Duration Types**
   - **ShorDuration**: Short duration, using double to represent seconds
   - **LongDuration**: Long duration, using separate integer and fractional seconds
   - **DaySecDuration**: Integer days + fractional seconds, suitable for long time spans

### Time Systems

The module supports conversions between the following time systems:

- **TAI (International Atomic Time)**: International Atomic Time
- **TT (Terrestrial Time)**: Terrestrial Dynamical Time, used in astrodynamics calculations
- **GPS (GPS Time)**: Global Positioning System Time
- **UTC (Coordinated Universal Time)**: Coordinated Universal Time

## Usage Examples

### 1. Date-Time Creation and Normalization

@[code](/examples/core/time/date_time_normalize.cpp)

### 2. Julian Date Conversion

@[code](/examples/core/time/julian_date_convert.cpp)

### 3. Time System Conversion

@[code](/examples/core/time/time_system_convert.cpp)


## Dependencies

- `AstGlobal.h`: Project global definitions
- `AstUtil/StringView.hpp`: String processing support
- `AstCore/Constants.h`: Constant definitions

## Notes

- All time parameters default to seconds (s)
- Angle parameters default to radians (rad)
- Some conversion functions consider leap seconds, others do not - please check function documentation
- Time representation using a single double has limited precision, for long time spans, use DaySecDuration or LongDuration
- Date-time normalization functions have multiple versions (local time, UTC time, Beijing time), please choose according to needs
- Conversions between Julian Date and Modified Julian Date have both precise and imprecise versions

## API Reference

@[code](/src/AstCore/Time/Date.hpp)
@[code](/src/AstCore/Time/Time.hpp)
@[code](/src/AstCore/Time/DateTime.hpp)
@[code](/src/AstCore/Time/JulianDate.hpp)
@[code](/src/AstCore/Time/ModJulianDate.hpp)
@[code](/src/AstCore/Time/Duration.hpp)
@[code](/src/AstCore/Time/TimeSystem.hpp)
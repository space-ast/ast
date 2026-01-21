# Frame Transform Module

*Note: This document is AI-translated. For specific APIs and behaviors, please refer to the source code.*

## Module Overview

The Frame Transform module provides conversion functionality between various coordinate systems commonly used in astrodynamics and aerospace dynamics. This module supports precise transformations between inertial frames, fixed frames, and various intermediate reference frames, providing foundational support for orbit calculations, attitude control, and other applications.

## Core Concepts

### Coordinate System Definitions

The module supports the following main coordinate systems:

- **CBI (Central Body Inertial Frame)**：Central body inertial frame
- **CBF (Central Body Fixed Frame)**：Central body fixed frame
- **ECI (Earth Centered Inertial Frame)**：Earth-centered inertial frame, which can represent ICRF or J2000 depending on configuration
- **J2000**：Earth mean equatorial frame at TT 2000-01-01 12:00:00
- **MOD (Mean of Date)**：Mean equatorial frame, considering precession
- **TOD (True of Date)**：True equatorial frame, considering precession and nutation
- **GTOD (Greenwich True of Date)**：Greenwich true equatorial frame, with XY plane as equatorial plane, positive X-axis pointing to prime meridian, rotating with Earth
  - Also known as TDR, GCR, PEF, or EFG
- **ECF (Earth Centered Fixed Frame)**：Earth-centered fixed frame, considering polar motion on top of GTOD
- **ICRF (International Celestial Reference Frame)**：International Celestial Reference Frame
- **CIRF (Celestial Intermediate Reference Frame)**：Celestial Intermediate Reference Frame
  - The first pole (z-axis direction) is the Celestial Intermediate Pole (CIP) at a given time
  - The fundamental plane is the intermediate equator perpendicular to the CIP at that time
  - Longitude coordinate starts from the Celestial Intermediate Origin (CIO)
- **TIRF (Terrestrial Intermediate Reference Frame)**：Terrestrial Intermediate Reference Frame
  - Linked to the International Terrestrial Reference Frame (ITRF) by polar motion and the positioning angle of the terrestrial intermediate origin
  - Connected to CIRF by rotating around the CIP by the Earth Rotation Angle (ERA)

### Transform Types

The module provides three main types of coordinate transformation interfaces:

1. **Transform**：Returns Rotation or KinematicRotation objects
2. **Matrix**：Returns coordinate transformation matrices
3. **Direct Transformation**：Directly converts coordinates from one reference frame to another

Some transformations also support velocity conversion, considering the angular velocity effects of rotating coordinate systems.

## Main Functions

The module supports transformations between the following coordinate systems:

- **ECI ↔ ECF**：Earth-centered inertial frame and Earth-centered fixed frame
- **J2000 ↔ ECF**：J2000 inertial frame and Earth-centered fixed frame
- **J2000 ↔ MOD**：J2000 inertial frame and mean of date frame
- **MOD ↔ TOD**：Mean of date frame and true of date frame
- **TOD ↔ GTOD**：True of date frame and Greenwich true of date frame
- **GTOD ↔ ECF**：Greenwich true of date frame and Earth-centered fixed frame
- **ICRF ↔ ECF**：International Celestial Reference Frame and Earth-centered fixed frame
- **ICRF ↔ CIRF**：International Celestial Reference Frame and Celestial Intermediate Reference Frame
- **CIRF ↔ TIRF**：Celestial Intermediate Reference Frame and Terrestrial Intermediate Reference Frame
- **TIRF ↔ ECF**：Terrestrial Intermediate Reference Frame and Earth-centered fixed frame

## Usage Examples

### 1. J2000 to ECF Coordinate Conversion

@[code](/examples/core/frame_transform/j2000_to_ecf.cpp)

### 2. J2000 to ECF Conversion with Velocity

@[code](/examples/core/frame_transform/j2000_to_ecf_with_velocity.cpp)

### 3. Using Transformation Matrix

@[code](/examples/core/frame_transform/using_transform_matrix.cpp)

## Dependencies

- `AstGlobal.h`：Project global definitions
- `AstCore/TimePoint.hpp`：Time point definition
- `AstMath/Vector.hpp`：Vector definition
- `AstMath/Matrix.hpp`：Matrix definition
- `AstMath/Rotation.hpp`：Rotation definition
- `AstCore/SOFA.hpp`：Astronomical algorithm library interface
- `AstCore/EOP.hpp`：Earth Orientation Parameters interface

## Notes

- All angle parameters default to radians (rad)
- Transformation accuracy depends on the precession and nutation models used
- Polar motion correction requires EOP data support
- Some transformation functions provide both IAU1980 and JPL DE implementations, which can be selected via `aNutationMethodSet`
- More precise model selection may be required for long time span transformations

## API Reference

@[code](/src/AstCore/Coordinate/FrameTransform.hpp)
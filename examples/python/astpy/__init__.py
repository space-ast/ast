# AstPy - SpaceAST Python Bindings
"""
AstPy provides Python bindings for the SpaceAST C++ astrodynamics library.

This module provides access to:
- Time systems (TimePoint, JulianDate, DateTime)
- Mathematics (Vector3d, Matrix3d, Quaternion, Euler)
- Orbit mechanics (OrbElem, CartState, TwoBody)
- Coordinate frames (Frame, Axes, Transform)
- Celestial bodies (SolarSystem, Planet)

Example:
    >>> import astpy
    >>> v = astpy.Vector3d(1, 2, 3)
    >>> print(v.norm())
    >>> tp = astpy.TimePoint.FromUTC(2024, 1, 1, 12, 0, 0)
"""

# Import all public symbols from the C++ extension
from ._ast import *

# Version information
__version__ = "1.0.0"
__author__ = "SpaceAST Development Team"
__license__ = "Apache License 2.0"

# Public API
__all__ = [
    # Time system
    "TimePoint",
    "JulianDate",
    "ModJulianDate",
    "DateTime",
    "Date",
    "TimeInterval",
    "DaySecDuration",
    "TimeScale",
    
    # Mathematics
    "Vector3d",
    "Matrix3d",
    "Quaternion",
    "Euler",
    "AngleAxis",
    "EulerOrder",
    
    # Orbit mechanics
    "OrbElem",
    "CartState",
    "TwoBody",
    "SimpleOrbitDesigner",
    "CircularOrbitDesigner",
    "StationaryOrbitDesigner",
    
    # Coordinate frames
    "Frame",
    "FrameRoot",
    "FrameICRF",
    "Axes",
    "AxesRoot",
    "AxesICRF",
    "Transform",
    "KinematicTransform",
    "Rotation",
    
    # Celestial bodies
    "SolarSystem",
    "CelestialBody",
    "Planet",
    
    # Constants
    "GM_EARTH",
    "GM_SUN",
    "GM_MOON",
    "R_EARTH",
    "R_SUN",
    "R_MOON",
    "AU",
    "SPEED_OF_LIGHT",
    "PI",
    "TWO_PI",
    "DEG_TO_RAD",
    "RAD_TO_DEG",
    "J2000_JD",
    "J2000_MJD",
    
    # Helper functions
    "deg",
    "rad",
    "arcsec",
    "vector3d",
    "quaternion",
    "timepoint",
    "orb_elem",
    "cart_state",
    "rv2moe",
    "rv2coe",
    "moe2rv",
    "coe2rv",
    "aQuatToMatrix",
    "aMatrixToQuat",
    "aRotationXMatrix",
    "aRotationYMatrix",
    "aRotationZMatrix",
    "aRotationMatrix",
]

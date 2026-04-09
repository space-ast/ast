"""AstPy - Aerospace simulation Python bindings"""

from .Math import Vector3d, Matrix3d
from .Time import Date, DateTime, JulianDate, TimePoint
from .Orbit import OrbitalElements, OrbitParameters, MU_EARTH
from .Coord import CoordinateSystem, CoordinateConverter

__version__ = "0.1.0"

__all__ = [
    # Math
    'Vector3d',
    'Matrix3d',
    # Time
    'Date',
    'DateTime',
    'JulianDate',
    'TimePoint',
    # Orbit
    'OrbitalElements',
    'OrbitParameters',
    'MU_EARTH',
    # Coord
    'CoordinateSystem',
    'CoordinateConverter',
]

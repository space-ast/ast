"""Coord module for AstPy - Coordinate system conversions"""
import math
from enum import Enum
from typing import Tuple
from ..Math.vector import Vector3d, Matrix3d

class CoordinateSystem(Enum):
    """Coordinate system enumeration"""
    J2000 = "J2000"
    J2000_ECL = "J2000_ECLIPTIC"
    MOD = "MOD"  # Mean Equator of Date
    TOD = "TOD"  # True Equator of Date
    ECI = "ECI"  # Earth Centered Inertial
    ECEF = "ECEF"  # Earth Centered Earth Fixed
    GEOCENTRIC = "GEOCENTRIC"
    TOPOCENTRIC = "TOPOCENTRIC"

class CoordinateConverter:
    """Coordinate system converter"""
    
    # Earth's rotation rate (rad/s)
    OMEGA_EARTH = 7.292115e-5
    
    # WGS84 Earth ellipsoid parameters
    WGS84_A = 6378137.0  # Semi-major axis (m)
    WGS84_B = 6356752.314245  # Semi-minor axis (m)
    WGS84_F = 1/298.257223563  # Flattening
    WGS84_E2 = 0.00669437999014  # First eccentricity squared
    
    @staticmethod
    def eci_to_ecef(position: Vector3d, gmst: float) -> Vector3d:
        """Convert ECI to ECEF coordinates
        
        Args:
            position: Position in ECI frame
            gmst: Greenwich Mean Sidereal Time (radians)
        """
        cos_gmst = math.cos(gmst)
        sin_gmst = math.sin(gmst)
        
        return Vector3d(
            cos_gmst * position.x + sin_gmst * position.y,
            -sin_gmst * position.x + cos_gmst * position.y,
            position.z
        )
    
    @staticmethod
    def ecef_to_eci(position: Vector3d, gmst: float) -> Vector3d:
        """Convert ECEF to ECI coordinates
        
        Args:
            position: Position in ECEF frame
            gmst: Greenwich Mean Sidereal Time (radians)
        """
        cos_gmst = math.cos(gmst)
        sin_gmst = math.sin(gmst)
        
        return Vector3d(
            cos_gmst * position.x - sin_gmst * position.y,
            sin_gmst * position.x + cos_gmst * position.y,
            position.z
        )
    
    @staticmethod
    def ecef_to_lla(position: Vector3d) -> Tuple[float, float, float]:
        """Convert ECEF to geodetic coordinates (Latitude, Longitude, Altitude)
        
        Args:
            position: Position in ECEF (meters)
        
        Returns:
            Tuple of (latitude, longitude, altitude) in degrees and meters
        """
        x, y, z = position.x, position.y, position.z
        
        # Longitude
        lon = math.atan2(y, x)
        
        # Iterative solution for latitude and altitude
        p = math.sqrt(x**2 + y**2)
        lat = math.atan2(z, p * (1 - CoordinateConverter.WGS84_F))
        
        # Convergent iteration for latitude with maximum iteration protection
        convergence_threshold = 1e-12
        max_iterations = 100
        iteration_count = 0
        while iteration_count < max_iterations:
            iteration_count += 1
            sin_lat = math.sin(lat)
            N = CoordinateConverter.WGS84_A / math.sqrt(1 - CoordinateConverter.WGS84_E2 * sin_lat**2)
            new_lat = math.atan2(z + CoordinateConverter.WGS84_E2 * N * sin_lat, p)
            if abs(new_lat - lat) < convergence_threshold:
                lat = new_lat
                break
            lat = new_lat
        else:
            # If max iterations reached without convergence, use last value
            pass
        
        sin_lat = math.sin(lat)
        cos_lat = math.cos(lat)
        N = CoordinateConverter.WGS84_A / math.sqrt(1 - CoordinateConverter.WGS84_E2 * sin_lat**2)
        alt = p / cos_lat - N
        
        return math.degrees(lat), math.degrees(lon), alt
    
    @staticmethod
    def lla_to_ecef(lat: float, lon: float, alt: float) -> Vector3d:
        """Convert geodetic to ECEF coordinates
        
        Args:
            lat: Latitude (degrees)
            lon: Longitude (degrees)
            alt: Altitude above ellipsoid (meters)
        
        Returns:
            Position in ECEF (meters)
        """
        lat_rad = math.radians(lat)
        lon_rad = math.radians(lon)
        
        sin_lat = math.sin(lat_rad)
        cos_lat = math.cos(lat_rad)
        
        N = CoordinateConverter.WGS84_A / math.sqrt(1 - CoordinateConverter.WGS84_E2 * sin_lat**2)
        
        x = (N + alt) * cos_lat * math.cos(lon_rad)
        y = (N + alt) * cos_lat * math.sin(lon_rad)
        z = (N * (1 - CoordinateConverter.WGS84_E2) + alt) * sin_lat
        
        return Vector3d(x, y, z)
    
    @staticmethod
    def rotate_matrix(from_sys: CoordinateSystem, to_sys: CoordinateSystem, 
                      gmst: float = 0.0, jd: float = 0.0) -> Matrix3d:
        """Get rotation matrix between coordinate systems
        
        Args:
            from_sys: Source coordinate system
            to_sys: Target coordinate system
            gmst: Greenwich Mean Sidereal Time (radians)
            jd: Julian Date (for precession/nutation)
        
        Raises:
            NotImplementedError: If the coordinate conversion is not supported
        """
        # J2000 to ECEF rotation
        cos_gmst = math.cos(gmst)
        sin_gmst = math.sin(gmst)
        
        if from_sys == CoordinateSystem.J2000 and to_sys == CoordinateSystem.ECEF:
            return Matrix3d((cos_gmst, sin_gmst, 0, -sin_gmst, cos_gmst, 0, 0, 0, 1))
        elif from_sys == CoordinateSystem.ECEF and to_sys == CoordinateSystem.J2000:
            return Matrix3d((cos_gmst, -sin_gmst, 0, sin_gmst, cos_gmst, 0, 0, 0, 1))
        
        # Unsupported conversion - raise NotImplementedError
        raise NotImplementedError(
            f"Coordinate conversion from {from_sys.value} to {to_sys.value} is not implemented"
        )
    
    @staticmethod
    def convert(position: Vector3d, from_sys: CoordinateSystem, 
                to_sys: CoordinateSystem, gmst: float = 0.0) -> Vector3d:
        """Convert position between coordinate systems
        
        Args:
            position: Position vector in source frame
            from_sys: Source coordinate system
            to_sys: Target coordinate system
            gmst: Greenwich Mean Sidereal Time (radians)
        
        Raises:
            NotImplementedError: If the coordinate conversion is not supported
        """
        matrix = CoordinateConverter.rotate_matrix(from_sys, to_sys, gmst)
        return matrix * position
    
    @staticmethod
    def compute_gmst(jd: float) -> float:
        """Compute Greenwich Mean Sidereal Time
        
        Args:
            jd: Julian Date
        
        Returns:
            GMST in radians
        """
        T = (jd - 2451545.0) / 36525.0
        gmst = 280.46061837 + 360.98564736629 * (jd - 2451545.0) + 0.000387933 * T**2
        gmst = math.fmod(gmst, 360.0)
        if gmst < 0:
            gmst += 360.0
        return math.radians(gmst)

"""
Time module for AstPy
Provides Date, DateTime, JulianDate, and TimePoint classes
"""
from datetime import datetime, timedelta
from typing import Union

class Date:
    """Date class for handling calendar dates"""
    def __init__(self, year: int, month: int, day: int):
        self.year = year
        self.month = month
        self.day = day
    
    def __str__(self) -> str:
        return f"{self.year:04d}-{self.month:02d}-{self.day:02d}"
    
    def __repr__(self) -> str:
        return self.__str__()
    
    def to_julian_date(self) -> 'JulianDate':
        return self.to_datetime().to_julian_date()
    
    def to_datetime(self) -> 'DateTime':
        return DateTime(self.year, self.month, self.day, 0, 0, 0.0)

class DateTime:
    """DateTime class for handling date and time"""
    def __init__(self, year: int, month: int, day: int, hour: int = 0, minute: int = 0, second: float = 0.0):
        self.year = year
        self.month = month
        self.day = day
        self.hour = hour
        self.minute = minute
        self.second = second
    
    def __str__(self) -> str:
        return f"{self.year:04d}-{self.month:02d}-{self.day:02d} {self.hour:02d}:{self.minute:02d}:{self.second:08.5f}"
    
    def __repr__(self) -> str:
        return self.__str__()
    
    def to_julian_date(self) -> 'JulianDate':
        return JulianDate.from_datetime(self)
    
    def to_datetime(self) -> datetime:
        # Preserve fractional seconds by using timedelta for microseconds
        # Note: datetime only supports microsecond precision (6 decimal places)
        whole_seconds = int(self.second)
        fractional_seconds = self.second - whole_seconds
        # Round to nearest microsecond to avoid float accumulation errors
        microseconds = int(round(fractional_seconds * 1_000_000))
        return datetime(self.year, self.month, self.day, self.hour, self.minute, whole_seconds, microseconds)

class JulianDate:
    """Julian Date class for astronomical time calculations"""
    J2000 = 2451545.0
    
    def __init__(self, jd: float):
        self.jd = jd
        self.mjd = jd - 2400000.5
    
    def __str__(self) -> str:
        return f"JD {self.jd:.10f}"
    
    def __repr__(self) -> str:
        return self.__str__()
    
    @staticmethod
    def from_datetime(dt: Union[datetime, DateTime]) -> 'JulianDate':
        if isinstance(dt, DateTime):
            dt = dt.to_datetime()
        year, month = dt.year, dt.month
        day = dt.day + (dt.hour + dt.minute / 60.0 + dt.second / 3600.0) / 24.0
        if month <= 2:
            year -= 1
            month += 12
        A = year // 100
        B = 2 - A + A // 4
        jd = (36525 * (year + 4716)) // 100 + (306001 * (month + 1)) // 10000 + day + B - 1524.5
        return JulianDate(jd)
    
    def seconds_since_j2000(self) -> float:
        return (self.jd - self.J2000) * 86400.0
    
    def to_datetime(self) -> datetime:
        """Convert Julian Date to Python datetime.
        
        Returns:
            datetime: Python datetime object with microsecond precision
        """
        # Algorithm from Jean Meeus, Astronomical Algorithms
        jd = self.jd + 0.5
        Z = int(jd)
        F = jd - Z
        
        if Z < 2299161:
            A = Z
        else:
            alpha = int((Z - 1867216.25) / 36524.25)
            A = Z + 1 + alpha - alpha // 4
        
        B = A + 1524
        C = int((B - 122.1) / 365.25)
        D = int(365.25 * C)
        E = int((B - D) / 30.6001)
        
        day = B - D - int(30.6001 * E) + F
        
        if E < 14:
            month = E - 1
        else:
            month = E - 13
        
        if month > 2:
            year = C - 4716
        else:
            year = C - 4715
        
        # Extract time components
        hours = (day - int(day)) * 24
        minutes = (hours - int(hours)) * 60
        seconds = (minutes - int(minutes)) * 60
        
        day_int = int(day)
        hour_int = int(hours)
        minute_int = int(minutes)
        second_int = int(seconds)
        microsecond = int((seconds - second_int) * 1_000_000)
        
        return datetime(year, month, day_int, hour_int, minute_int, second_int, microsecond)

class TimePoint:
    """TimePoint class for precise time representation"""
    def __init__(self, seconds_since_j2000: float):
        self.seconds = seconds_since_j2000
    
    def __str__(self) -> str:
        return f"TimePoint(T={self.seconds:.3f}s since J2000)"
    
    def __repr__(self) -> str:
        return self.__str__()
    
    def to_julian_date(self) -> JulianDate:
        return JulianDate(JulianDate.J2000 + self.seconds / 86400.0)

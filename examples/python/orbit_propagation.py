#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
orbit_propagation.py - Orbital Propagation Examples

This script demonstrates orbital propagation using the SpaceAST library.

Author: SpaceAST Development Team
Version: 1.0.0
"""

import astpy
import math

def print_section(title):
    """Print a section header."""
    print("\n" + "=" * 60)
    print(f" {title}")
    print("=" * 60)


def example_two_body_propagation():
    """Example: Two-body orbital propagation."""
    print_section("Two-Body Propagation")
    
    # Earth parameters
    GM = astpy.GM_EARTH
    R_EARTH = astpy.R_EARTH
    
    # Create a circular orbit at 400 km altitude
    h = 400.0  # km
    a = R_EARTH + h  # Semi-major axis
    e = 0.0    # Circular orbit
    i = math.radians(51.6)  # ISS-like inclination
    
    # Create orbital elements
    oe = astpy.OrbElem(a=a, e=e, i=i, 
                       raan=0.0, argper=0.0, trueA=0.0)
    
    print(f"Initial orbit:")
    print(f"  Altitude: {h:.1f} km")
    print(f"  Semi-major axis: {a:.3f} km")
    print(f"  Eccentricity: {e:.6f}")
    print(f"  Inclination: {math.degrees(i):.2f} deg")
    
    # Calculate orbital period
    n = oe.getMeanMotion(GM)
    T = 2 * math.pi / n
    print(f"  Period: {T:.2f} seconds = {T/60:.2f} minutes")
    
    # Initial state at perigee (for circular orbit, perigee = any point)
    cs_initial = astpy.CartState(
        x=a, y=0, z=0,
        vx=0, vy=math.sqrt(GM/a), vz=0
    )
    
    print(f"\nInitial state:")
    print(f"  r = ({cs_initial.x:.3f}, {cs_initial.y:.3f}, {cs_initial.z:.3f}) km")
    print(f"  v = ({cs_initial.vx:.4f}, {cs_initial.vy:.4f}, {cs_initial.vz:.4f}) km/s")
    
    # Propagate for one orbit
    cs_final = astpy.TwoBody.propagate(cs_initial, T, GM)
    
    print(f"\nAfter one orbit ({T:.2f} seconds):")
    print(f"  r = ({cs_final.x:.3f}, {cs_final.y:.3f}, {cs_final.z:.3f}) km")
    print(f"  v = ({cs_final.vx:.4f}, {cs_final.vy:.4f}, {cs_final.vz:.4f}) km/s")
    
    # Check that we're back to the starting position
    dr = math.sqrt((cs_final.x - cs_initial.x)**2 + 
                   (cs_final.y - cs_initial.y)**2 + 
                   (cs_final.z - cs_initial.z)**2)
    print(f"\nPosition error: {dr:.6f} km")


def example_elliptical_orbit():
    """Example: Elliptical orbit propagation."""
    print_section("Elliptical Orbit Propagation")
    
    GM = astpy.GM_EARTH
    
    # Create a Molniya-type orbit
    a = 26600.0   # Semi-major axis [km]
    e = 0.7       # High eccentricity
    i = math.radians(63.4)  # Critical inclination
    
    oe = astpy.OrbElem(a=a, e=e, i=i, 
                       raan=0.0, argper=math.pi/2, trueA=0.0)
    
    print(f"Molniya orbit:")
    print(f"  Semi-major axis: {a:.1f} km")
    print(f"  Eccentricity: {e:.3f}")
    print(f"  Perigee altitude: {a*(1-e) - astpy.R_EARTH:.1f} km")
    print(f"  Apogee altitude: {a*(1+e) - astpy.R_EARTH:.1f} km")
    
    # Period
    T = 2 * math.pi * math.sqrt(a**3 / GM)
    print(f"  Period: {T/3600:.2f} hours")
    
    # Convert to Cartesian
    cs = astpy.moe2rv(oe, GM)
    
    print(f"\nState at perigee:")
    print(f"  r = ({cs.x:.1f}, {cs.y:.1f}, {cs.z:.1f}) km")
    print(f"  v = ({cs.vx:.3f}, {cs.vy:.3f}, {cs.vz:.3f}) km/s")
    
    # Propagate through orbit
    dt = T / 4  # Quarter orbit
    cs_quarter = astpy.TwoBody.propagate(cs, dt, GM)
    print(f"\nAfter quarter orbit ({dt/3600:.2f} hours):")
    print(f"  r = ({cs_quarter.x:.1f}, {cs_quarter.y:.1f}, {cs_quarter.z:.1f}) km")
    
    # Propagate to apogee
    cs_apogee = astpy.TwoBody.propagate(cs, dt * 2, GM)
    print(f"\nAt apogee ({T/2/3600:.2f} hours):")
    print(f"  r = ({cs_apogee.x:.1f}, {cs_apogee.y:.1f}, {cs_apogee.z:.1f}) km")


def example_state_conversion():
    """Example: Converting between orbital elements and Cartesian."""
    print_section("State Conversion")
    
    GM = astpy.GM_EARTH
    
    # ISS-like orbit
    a = astpy.R_EARTH + 400.0
    e = 0.0005
    i = math.radians(51.6)
    raan = math.radians(10.0)
    argp = math.radians(30.0)
    nu = math.radians(45.0)
    
    # Create orbital elements
    oe = astpy.OrbElem(a=a, e=e, i=i, 
                       raan=raan, argper=argp, trueA=nu)
    
    print("Orbital elements:")
    print(f"  a = {oe.a:.3f} km")
    print(f"  e = {oe.e:.6f}")
    print(f"  i = {math.degrees(oe.i):.4f} deg")
    print(f"  RAAN = {math.degrees(oe.raan):.4f} deg")
    print(f"  ω = {math.degrees(oe.argper):.4f} deg")
    print(f"  ν = {math.degrees(oe.trueA):.4f} deg")
    
    # Convert to Cartesian
    cs = astpy.moe2rv(oe, GM)
    
    print("\nCartesian state:")
    print(f"  r = ({cs.x:.3f}, {cs.y:.3f}, {cs.z:.3f}) km")
    print(f"  v = ({cs.vx:.5f}, {cs.vy:.5f}, {cs.vz:.5f}) km/s")
    
    # Convert back to orbital elements
    oe_back = astpy.rv2coe(cs, GM)
    
    print("\nConverted back to orbital elements:")
    print(f"  a = {oe_back.a:.3f} km (diff: {abs(oe_back.a - oe.a):.6f})")
    print(f"  e = {oe_back.e:.6f} (diff: {abs(oe_back.e - oe.e):.10f})")
    
    # Calculate specific energy and angular momentum
    r = math.sqrt(cs.x**2 + cs.y**2 + cs.z**2)
    v = math.sqrt(cs.vx**2 + cs.vy**2 + cs.vz**2)
    
    energy = v**2 / 2 - GM / r
    h_mag = math.sqrt((cs.y * cs.vz - cs.z * cs.vy)**2 +
                      (cs.z * cs.vx - cs.x * cs.vz)**2 +
                      (cs.x * cs.vy - cs.y * cs.vx)**2)
    
    a_calc = -GM / (2 * energy)
    e_calc = math.sqrt(1 + 2 * energy * h_mag**2 / GM**2)
    
    print("\nDerived quantities:")
    print(f"  Specific energy: {energy:.3f} km^2/s^2")
    print(f"  |h|: {h_mag:.3f} km^2/s")
    print(f"  Calculated a: {a_calc:.3f} km")
    print(f"  Calculated e: {e_calc:.6f}")


def example_time_propagation():
    """Example: Propagating with time points."""
    print_section("Time-Based Propagation")
    
    GM = astpy.GM_EARTH
    
    # Create initial time point
    t0 = astpy.TimePoint.FromUTC(2024, 1, 1, 12, 0, 0)
    print(f"Initial epoch: {t0}")
    
    # Create initial state
    cs0 = astpy.CartState(
        x=astpy.R_EARTH + 400,
        y=0, z=0,
        vx=0,
        vy=math.sqrt(GM / (astpy.R_EARTH + 400)),
        vz=0
    )
    
    # Propagate to future time
    future_tp = t0 + 5400  # 1.5 hours later
    print(f"Target time: {future_tp}")
    
    dt = (future_tp - t0)  # Time difference in seconds
    cs_future = astpy.TwoBody.propagate(cs0, dt, GM)
    
    print(f"\nPropagated state at target time:")
    print(f"  r = ({cs_future.x:.3f}, {cs_future.y:.3f}, {cs_future.z:.3f}) km")
    print(f"  v = ({cs_future.vx:.5f}, {cs_future.vy:.5f}, {cs_future.vz:.5f}) km/s")
    
    # Calculate orbital period
    a = astpy.R_EARTH + 400
    T = 2 * math.pi * math.sqrt(a**3 / GM)
    
    # Calculate ground track (simplified)
    lon0 = 0.0
    lat0 = 0.0
    
    print(f"\nOrbit period: {T/60:.2f} minutes")
    print(f"Time since epoch: {dt:.0f} seconds = {dt/T:.4f} orbits")


def main():
    """Run all propagation examples."""
    print("\n" + "#" * 60)
    print("# SpaceAST - Orbital Propagation Examples")
    print("#" * 60)
    
    example_two_body_propagation()
    example_elliptical_orbit()
    example_state_conversion()
    example_time_propagation()
    
    print("\n" + "#" * 60)
    print("# All propagation examples completed!")
    print("#" * 60 + "\n")


if __name__ == "__main__":
    main()

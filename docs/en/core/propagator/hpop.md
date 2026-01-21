# HPOP High Precision Orbit Propagator

*Note: This document is AI-translated.*

## Overview

HPOP (High Precision Orbit Propagator) is a high-precision orbit propagation module based on numerical integration methods. It employs a modular design philosophy inspired by Simulink simulation concepts, allowing flexible configuration of force models and numerical integrators to meet various precision requirements.

## Core Concepts

### Modular Design

HPOP decomposes complex orbit propagation systems into functional blocks, each responsible for specific tasks:

- **Force Model Blocks**: Calculate various perturbation forces
- **Numerical Integration Blocks**: Solve differential equations
- **Coordinate Transformation Blocks**: Handle coordinate system conversions

### Force Model Configuration

HPOP supports multiple force model configurations:

- **Gravity Models**: Spherical harmonics of different degrees and orders
- **Atmospheric Drag**: Atmospheric resistance effects
- **Third-Body Gravity**: Sun and Moon gravitational perturbations
- **Solar Radiation Pressure**: Solar radiation pressure effects

## Main Interfaces

### Class Interface

```cpp
class HPOP
```

**Key Methods:**

```cpp
err_t setForceModel(const HPOPForceModel& force_model);
err_t setIntegrator(const std::string& integrator_type);
err_t propagate(const TimePoint& start_time, const TimePoint& end_time, Vector3d& pos, Vector3d& vel);
err_t initialize();
```

**Parameter Description:**

- `force_model`: Force model configuration parameters
- `integrator_type`: Numerical integrator type (e.g., "RK4", "RK78")
- `start_time`: Propagation start time
- `end_time`: Propagation end time
- `pos`: Input/output position vector (meters)
- `vel`: Input/output velocity vector (m/s)

## Usage Examples

### 1. Basic HPOP Propagation

@[code](/examples/propagator/hpop/basic_propagation.cpp)

### 2. Advanced Force Model Configuration

@[code](/examples/propagator/hpop/advanced_force_model.cpp)

### 3. Multi-Day Propagation

@[code](/examples/propagator/hpop/multi_day_propagation.cpp)

## System Architecture

### Simulation Engine

HPOP's core is a simulation engine that coordinates various functional modules:

- **Time Management**: Handles time stepping and synchronization
- **Data Flow Control**: Manages data transfer between modules
- **Error Handling**: Provides comprehensive error detection and reporting

### Functional Modules

HPOP includes several types of functional modules:

- **Force Calculation Modules**: Gravity, drag, third-body, etc.
- **Numerical Integration Modules**: Different integration algorithms
- **Coordinate System Modules**: Coordinate transformations
- **Output Modules**: Result processing and output

## Accuracy and Performance

### Accuracy Characteristics

- **Highest Precision**: Numerical integration methods provide the highest precision
- **Force Model Flexibility**: Supports various perturbation force combinations
- **Adaptive Step Control**: Some integrators support adaptive step size control

### Performance Considerations

- **Computational Cost**: Higher than analytical methods, suitable for high-precision requirements
- **Memory Usage**: Moderate memory requirements
- **Real-time Capability**: May not be suitable for real-time applications with strict performance requirements

## Configuration Guide

### Force Model Configuration

Configure appropriate force models based on application requirements:

- **LEO Orbits**: Consider atmospheric drag and Earth gravity field
- **GEO Orbits**: Focus on third-body gravity and solar radiation pressure
- **High-Precision Requirements**: Use higher-degree gravity field models

### Integrator Selection

Choose suitable numerical integrators:

- **RK4**: Good balance of accuracy and performance
- **RK78**: Higher accuracy, suitable for long-term propagation
- **Adaptive Integrators**: Automatic step size adjustment

## Notes

- HPOP requires initialization before use
- Force model configuration affects calculation accuracy and performance
- Numerical integrator selection should consider precision and computational cost
- For long-term propagation, consider using higher-precision integrators

## Advantages and Limitations

### Advantages

- **High Precision**: Numerical integration provides highest accuracy
- **Flexibility**: Modular design supports various configurations
- **Comprehensive Force Models**: Supports multiple perturbation forces

### Limitations

- **Higher Computational Cost**: Slower than analytical methods
- **Complex Configuration**: Requires understanding of force models and integrators
- **Memory Requirements**: Higher than analytical methods

## API Reference

@[code](/src/AstCore/Propagator/HPOP/HPOP.hpp)
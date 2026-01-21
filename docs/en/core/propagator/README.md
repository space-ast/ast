# Orbit Propagator Module

*Note: This document is AI-translated. For specific API and behavior, please refer to the source code.*

## Module Overview

The orbit propagator module provides various orbit propagation algorithms, ranging from simple two-body problems to high-precision numerical integration methods. This module supports orbit prediction requirements with different accuracy and computational efficiency needs.

## Core Features

### Propagator Types

The module provides the following main propagators:

- **Two-Body Propagator (TwoBody)**: Analytical solution based on Kepler's equation, suitable for ideal two-body problems
- **J2 Analytical Propagator (J2Analytical)**: Analytical propagator considering Earth's oblateness J2 term
- **J4 Analytical Propagator (J4Analytical)**: Analytical propagator considering Earth's oblateness J2 and J4 terms
- **High Precision Orbit Propagator (HPOP)**: High-precision orbit propagator based on numerical integration, supporting multiple perturbation force models

### Propagation Methods

- **Position-Velocity Propagation**: Direct propagation of position and velocity vectors
- **Orbital Elements Propagation**: Propagation of modified orbital elements

## Main Features

- Supports orbit propagation at multiple accuracy levels
- Provides both object-oriented and functional interfaces
- Supports custom force model configuration
- Extensible integrator interface
- Complete error handling mechanism

## Dependencies

- `AstGlobal.h`: Project global definitions
- `AstCore/OrbitElement.hpp`: Orbital elements definition
- `AstCore/TimePoint.hpp`: Time point definition
- `AstMath/Vector.hpp`: Vector definition
- `AstMath/Matrix.hpp`: Matrix definition
- `AstUtil/Constants.h`: Physical constants definition
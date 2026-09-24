# 🛰️ Aerospace Simulation Algorithm Library SpaceAST

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/5f27ba68ec9a4928b5b863cd1bc10c81)](https://app.codacy.com/gh/space-ast/ast?utm_source=github.com&utm_medium=referral&utm_content=space-ast/ast&utm_campaign=Badge_Grade)
[![codecov](https://codecov.io/gh/space-ast/ast/graph/badge.svg?token=FIUMJKSPGZ)](https://codecov.io/gh/space-ast/ast)
![build](https://img.shields.io/github/actions/workflow/status/space-ast/ast/build.yml?branch=master&style=flat-square&logo=windows)
![C++](https://img.shields.io/badge/C++-11+-blue.svg)
![License](https://img.shields.io/badge/License-Apache2.0-green.svg)
![Platform](https://img.shields.io/badge/platform-linux%20%7C%20windows%20%7C%20mingw%20%7C%20wasm-lightgrey.svg)

English | [中文](README_zh.md)

**space-ast** is an algorithm library focused on the aerospace simulation domain.

It provides modern C++ implementations of core algorithms such as orbital dynamics, attitude simulation, trajectory planning, and tracking/control window analysis, offering a computational foundation for space mission analysis and design.


Prebuilt binary packages and self-installing source packages are available at: <https://github.com/space-ast/ast/releases>

If you want to know more, please refer to the [User Guide](https://space-ast.github.io/ast/), [API Documentation](https://space-ast.github.io/ast/api/), [GitHub](https://github.com/space-ast/ast), [Gitee](https://gitee.com/space-ast/ast) and [GitCode](https://gitcode.com/space-ast/ast).


## Main Functional Modules

### [Math Module](src/AstMath/)

Provides the low-level mathematical support required by astrodynamics, and is the numerical computing foundation of the entire library.

- Linear algebra: matrices, vectors, and related operations;
- Attitude: representations such as quaternions, axis-angle, and Euler angles, along with conversion functions;
- Coordinate transformations: including transformations of position coordinates, as well as kinematic versions that also transform velocity, supporting arbitrary combinations and inverses of coordinate transformations;
- Ordinary differential equations: fixed-step integrators (RK4, RK8, RKV8) and adaptive-step integrators (RKF45, RKF56, RKF78, RKCK), with event detectors and state observers;
- Nonlinear equations: multiple root-finding algorithms such as the secant method, Ridder, Brent, and bisection;
- Function extrema: extremum-finding algorithms such as Brent and golden section;
- Interpolation: an abstract interpolation interface and a Lagrange interpolator;


### [Utility Module](src/AstUtil/)

Provides general-purpose infrastructure such as parsing, compression, networking, and reflection.

- Units and dimensions: efficient and flexible dimension and unit types, supporting basic arithmetic, unit conversion, and custom units;
- Reflection: runtime type information, supporting dynamic type checking, property access, and similar operations;
- Parsers: parsers for formats such as JSON, XML, Markdown, DAF, and key-value files;
- Compression/decompression: compression and decompression for tar/tgz/zip formats;
- Networking: support for file downloads, HTTP requests, and more;
- Others: string processing, file system, logging, internationalization, colors, extended C++ containers, and more;


### [Space Environment Module](src/AstWeather/)

Provides atmospheric density, geomagnetic field, and radiation belt models.

- Atmospheric models: USSA1976, JacchiaRoberts, Harris-Priester, NRLMSISE-00, MSIS-86, MSISE-90, DTM-2012, and others;
- Geomagnetic field models: IGRF (International Geomagnetic Reference Field);
- Radiation belt models: AE8/AP8 radiation belt electron/proton models;

### [Core Computation Module](src/AstCore/)

Provides complete astrodynamics computation capabilities, including coordinate systems, force models, orbit propagation, orbit design, visibility analysis, and collision detection.

- Time systems: high-precision time point types, Julian dates, leap seconds, EOP, and conversions between different time systems;
- Coordinate systems: a complete hierarchy of coordinate systems, including ICRF, J2000, celestial inertial frames, celestial body-fixed frames, true equator frames, mean equator frames, VVLH, VNC, and more;
- Orbit propagation: SGP4, HPOP, J2/J4 analytical solutions, two-body, Vinti, and other orbit propagators;
- Ephemeris loading: support for loading ephemeris files in DE, SPK, STK, and other formats;
- Force models: perturbation models such as point-mass gravity, gravity fields, third-body gravity, atmospheric drag, and solar radiation pressure;
- Orbital elements: definitions and mutual conversions of state quantities such as classical orbital elements, equinoctial elements, Delaunay elements, spherical coordinates, geodetic coordinates, and B-plane states;
- Orbit designers: designers for frozen orbits, sun-synchronous orbits, Molniya orbits, repeating ground track orbits, geostationary orbits, and more;
- Access window analysis: accurately computes time windows satisfying given constraints, with support for arbitrary logical combinations of constraints;
- Collision detection: detects collisions between spacecraft, returning data such as the closest approach time, minimum distance, and relative velocity;
- Databases: TLE database loading and CelesTrak satellite database loading;


### Other Extension Modules

- [Simulation Module](src/AstSim/): defines various objects (spacecraft, ground stations, antennas, sensors, etc.) along with their motion and attitude models;
- [AI Module](src/AstAI/): provides a framework for integration with large language models, supporting tool calling, agent definitions, group chat, and more;
- [Script Module](src/AstScript/): provides a built-in script interpreter and executors for external scripting languages;
- [Visualization Module](src/AstVisualization/): provides 2D and 3D visualization of simulation data;
- [User Interface Module](src/AstUiCore/): provides interactive UI widgets for algorithm components, supporting parameter configuration, result display, and more;
- [SPICE Module](src/AstSPICE/): a lightweight wrapper around the core algorithms, providing interfaces compatible with NASA SPICE;
- [Scenario Loading Module](src/AstLoader/): parses STK/GMAT configuration files to build simulation objects;

## Quick Start

### Requirements

- C++11 compatible compiler (MSVC 2015+, GCC 5+, clang)
- [xmake 2.9+](https://xmake.io/)


### Building the Project


```bash
git clone https://github.com/space-ast/ast.git --depth=1
cd ast
xmake
```

## Project Architecture

```
ast/
├── data/          # Data
├── docs/          # Documentation
├── examples/      # Example code snippets
├── include/       # Header files
├── projects/      # Example projects
├── repo/          # Third-party library configuration files
├── scripts/       # Tool scripts
├── src/           # Source files
├── test/          # Test projects
└── thirdparty/    # Third-party libraries
```

## API Design Philosophy

- **Simple and easy to use**: concise API interfaces that developers can quickly integrate into their projects.
- **Highly customizable**: allows developers to customize algorithm parameters and behavior to suit their needs.
- **Performance optimized**: key algorithms are performance-optimized for aerospace simulation scenarios.

## Coding Standards

### Naming Conventions

The project adopts the following naming conventions:

```cpp
// Namespace ast
namespace ast
{
    // Global functions - single 'a' prefix
    aPropagateOrbit();
    aSolveLambert();
    aRotateQuaternion();


    // Enums - 'E' and 'e' prefix
    enum EFrame { eECI, eECEF }; 

    // Constants - 'k' prefix
    const kEps15 = 1e-15;
}
```

As you may notice, under the conventions above, every name starting with a lowercase letter denotes a value or an address.


### Comment Conventions

Doxygen comment conventions are followed: comments for functions, files, and classes use the `///` style, and other multi-line comments may also use the `/*! ... */` style.

- **Function comments**: every function should include detailed comments, covering parameter descriptions, return values, exceptional cases, and more.

- **Class comments**: every class should include detailed comments, covering the functional description, member variables, member functions, and more.

- **File comments**: every source file should include a file header comment, covering the file name, author, date, copyright information, and more.


## Contributing

We welcome contributions of all forms! Whether it's code improvements, documentation enhancements, bug reports, or new feature suggestions, they are all valuable support for the project.

### Contribution Process

1. Fork this repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

Please make sure your code follows the project's coding standards and add the corresponding test cases.

## License

This project is licensed under the Apache 2.0 License - see the [LICENSE](LICENSE) file for details.


## Contact Us

- Discussions: [GitHub Discussions](https://github.com/space-ast/ast/discussions)
- Issue reports: [GitHub Issues](https://github.com/space-ast/ast/issues)


## References

The development of this project has referenced and made use of a great deal of material and many tools. We thank all open-source community contributors for their hard work, researchers in the aerospace simulation field for their theoretical contributions, and industry-standard software for its inspiration; these valuable resources have provided a solid foundation for the development of the ast project.

### Open Source Code

- [Orekit](https://www.orekit.org/): an astrodynamics library written in Java, providing a rich set of orbit and attitude algorithms
- [GMAT](https://gmat.atlassian.net/): the General Mission Analysis Tool, an open-source space mission analysis software from NASA
- [Pagmo2](https://esa.github.io/pagmo2/): a parallel global optimization library developed by the European Space Agency
- [PyGMO2](https://esa.github.io/pygmo2/): Python bindings for Pagmo2, an optimization tool for scientific computing
- [PyKEP](https://esa.github.io/pykep/): an ESA astrodynamics Python library focused on orbit optimization
- [brahe](https://github.com/duncaneddy/brahe): a Rust astrodynamics library for research and engineering applications
- [satkit](https://github.com/ssmichael1/satkit): a Rust astrodynamics library implementing some fundamental astrodynamics functionality
- [AstroLib](https://gitee.com/wanghmail/AstroLib): an aerospace/satellite modeling and simulation algorithm library
- [Eigen](http://eigen.tuxfamily.org/): a high-performance C++ template library for linear algebra, matrix, and vector operations
- [Scipy](https://scipy.org/): a Python library for scientific and technical computing, providing functionality across many mathematical, scientific, and engineering domains
- [GNU Octave](https://octave.org/): open-source numerical computing software providing MATLAB-compatible syntax
- [Hipparchus](https://hipparchus.org/): a Java mathematics library providing a rich set of mathematical functions and algorithms
- [Qt](https://www.qt.io/): a cross-platform C++ framework for developing graphical user interface applications
- [Python](https://www.python.org/): a popular general-purpose programming language widely used in scientific computing
- [julia](https://julialang.org/): a high-performance dynamic programming language designed for scientific computing and numerical analysis
- [glibc](https://www.gnu.org/software/libc/): the GNU C Library, providing an implementation of the C standard library
- [abseil](https://abseil.io/): a C++ library providing many commonly used facilities such as string processing, memory management, and concurrent programming, open-sourced by Google

### Industry Software

The following industry software represents the benchmark tools in the aerospace simulation field, and their functional design has provided important references for this project:

- [STK](https://www.ansys.com/products/missions/ansys-stk): commercial space mission analysis, design, and visualization software from Ansys (formerly AGI), widely used across the entire space mission lifecycle
- [FreeFlyer](https://ai-solutions.com/): commercial space mission design and analysis software from a.i. solutions, supporting orbit planning, maneuver analysis, and constellation design
- [Copernicus](https://www.nasa.gov/general/copernicus/): space mission trajectory design and optimization software developed by NASA, supporting trajectory planning for the full range of scenarios from low Earth orbit to deep space exploration

## Project Dependencies

### Toolchain

- [Xmake](https://xmake.io/): a modern Lua-based C/C++ build tool

### Third-Party Libraries

All third-party libraries this project depends on are optional, and users can choose whether to include them as needed.

- [fmt](https://fmt.dev/): a C++ library for formatting strings, providing Python-like formatting syntax
- [Qt](https://www.qt.io/): a cross-platform C++ framework for developing graphical user interface applications
- [SPICE](https://naif.jpl.nasa.gov/naif/toolkit.html): a software library provided by NASA's Navigation and Ancillary Information Facility (NAIF)
- [Matplot++](https://github.com/alandefreitas/matplotplusplus): a C++ library for plotting 2D and 3D charts, providing MATLAB-like syntax
- [agg](https://agg.sourceforge.net/antigrain.com/index.html): provides high-quality 2D vector graphics rendering with support for sub-pixel resolution and anti-aliasing
- [gtest](https://github.com/google/googletest): a C++ unit testing framework for writing and running unit tests
- [benchmark](https://github.com/google/benchmark): a C++ performance benchmarking framework for measuring code performance metrics
- [vtk](https://www.vtk.org/): a cross-platform visualization toolkit for rendering and interactively displaying scientific data
- [replxx](https://github.com/AmokHuginnsson/replxx): provides interactive command-line input, including line editing, history, tab completion, and prompts


## Project Name Inspiration

ast derives from the Greek root astēr (ἀστήρ), meaning "star", carrying a natural imagery of interstellar space, and sharing roots with astronomy, astronaut, astrodynamics, and asteroid. Professionally, AST can be expanded as:

- Aerospace Simulation Tool
- Advanced Space Technology  
- Astrodynamics Solver Tool

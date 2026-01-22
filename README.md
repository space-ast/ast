# ast - Aerospace Simulation Algorithm Library

[![Codacy Badge](https://app.codacy.com/project/badge/Grade/dad4615cdda145a5a4a3ed6681b66ed9)](https://app.codacy.com/gh/space-ast/ast/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)
[![codecov](https://codecov.io/gh/space-ast/ast/graph/badge.svg?token=FIUMJKSPGZ)](https://codecov.io/gh/space-ast/ast)
![build](https://img.shields.io/github/actions/workflow/status/space-ast/ast/build.yml?branch=master&style=flat-square&logo=windows)
![C++](https://img.shields.io/badge/C++-11+-blue.svg)
![License](https://img.shields.io/badge/License-Apache2.0-green.svg)
![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows-lightgrey.svg)

**ast** (/æst/) is an open-source aerospace simulation algorithm library written in C++, providing computational foundations for space mission analysis and design.

*Precise as the stars, born for aerospace simulation*


## 🌟 Project Introduction ([中文](README_zh.md))

**ast** (/æst/) is an algorithm library focused on the aerospace simulation domain, providing modern C++ implementations of core algorithms including orbital mechanics, attitude control, and trajectory planning.

If you want to know more, please refer to the [Documentation](https://space-ast.github.io/ast/), [GitHub](https://github.com/space-ast/ast), [Gitee](https://gitee.com/space-ast/ast), [GitCode](https://gitcode.com/space-ast/ast). 

## 🚀 Core Features

### 🛰️ Algorithm Domains

- **Orbital Mechanics**: Two-body problem, orbit propagation, Lambert problem solving
- **Attitude Systems**: Quaternion operations, attitude dynamics, control algorithms
- **Coordinate Transformations**: ECI/ECF/LVLH and other coordinate system conversions
- **Environment Models**: Atmospheric density, gravity fields, solar radiation pressure
- **Numerical Methods**: Ordinary differential equation integration, linear algebra solvers

## 📦 Quick Start

### Requirements

- C++11 compatible compiler (GCC 5+, MSVC 2015+)
- [xmake 2.9+](https://xmake.io/)

### Building the Project

```bash
git clone https://github.com/space-ast/ast.git
cd ast
xmake
```

## 🏗️ Project Structure

```
ast/
├── 3rdparty/      # Third-party libraries
├── docs/          # Documentation
├── include/       # Header files
├── projects/      # Example projects
├── scripts/       # Tool scripts
├── src/           # Source files
└── test/          # Test projects
```

## 🎯 API Design Philosophy

### Naming Conventions
The project adopts the following naming conventions:

```cpp
// Namespace ast
namespace ast
{
    // ...
}

// Global functions - single 'a' prefix
aPropagateOrbit();
aSolveLambert();
aRotateQuaternion();

// Type aliases - 'A' prefix
typedef ast::OrbitPropagator    AOrbitPropagator;
typedef ast::AttitudeController AAttitudeController;

// Enums - 'E' and 'e' prefix
enum EFrame { eECI, eECEF }; 

// Constants - 'k' prefix
const kEps15 = 1e-15;
```

## 🤝 Contributing

We welcome all forms of contributions! Whether it's code improvements, documentation enhancements, bug reports, or feature suggestions, your input is valuable to the project.

### Contribution Process

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

Please ensure your code follows the project's coding standards and includes appropriate test cases.

## 📄 License

This project is licensed under the Apache 2.0 License - see the [LICENSE](LICENSE) file for details.

## 🛠️ Development Status

**Current Status**: 🟢 Active Development

The project is currently in early development stage, and APIs may undergo significant changes. We are actively improving core functionality and welcome feedback from early users.

### Module Planning

Core modules will be carefully designed and developed, with test coverage improved through test projects:

- AstUtil: Utility functions
- AstCore: Dynamics core
- AstOpt: Nonlinear programming interface library
- AstOCP: Optimal control problem solving

The following modules will be developed with AI assistance (this README was also AI-assisted by https://www.deepseek.com/):

- AstScript: Scripting system
- AstInterp: Interactive console
- AstMex: MATLAB wrappers
- AstPy: Python wrappers
- AstGfx: Visualization
- AstGui: Graphical user interface

## 📞 Contact

- 💬 Discussions: [GitHub Discussions](https://github.com/space-ast/ast/discussions)
- 🐛 Bug Reports: [GitHub Issues](https://github.com/space-ast/ast/issues)

## 🌌 Project Name Inspiration

**ast** carries multiple layers of meaning, each resonating with the essence of aerospace simulation:

### 🪐 **Etymological Depth**

- **Stellar Origins**: Derived from the Greek root `aster` (ἀστήρ), meaning "star"
- **Astronomical Heritage**: Shares roots with astronomy, astronaut, astrodynamics
- **Cosmic Exploration**: Symbolizes our mission to explore the universe through simulation technology

### 🚀 **Professional Connotations**

- **Aerospace Simulation Tool** - Aerospace simulation tool
- **Advanced Space Technology** - Advanced space technology  
- **Astrodynamics Solver Tool** - Astrodynamics solver tool

---

"In the universe of code, we chart the stellar trajectories of aerospace"

---
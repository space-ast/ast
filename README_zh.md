# 🛰️ 航天仿真算法库 SpaceAST

[![Codacy Badge](https://app.codacy.com/project/badge/Grade/dad4615cdda145a5a4a3ed6681b66ed9)](https://app.codacy.com/gh/space-ast/ast/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)
[![codecov](https://codecov.io/gh/space-ast/ast/graph/badge.svg?token=FIUMJKSPGZ)](https://codecov.io/gh/space-ast/ast)
![build](https://img.shields.io/github/actions/workflow/status/space-ast/ast/build.yml?branch=master&style=flat-square&logo=windows)
![C++](https://img.shields.io/badge/C++-11+-blue.svg)
![License](https://img.shields.io/badge/License-Apache2.0-green.svg)
![Platform](https://img.shields.io/badge/platform-linux%20%7C%20windows%20%7C%20mingw%20%7C%20wasm-lightgrey.svg)


**space-ast** (/æst/) 是一个用C++编写的航天仿真算法库，为航天任务分析和设计提供计算基础。

*像星辰般精准，为航天仿真而生*


## 🌟 项目简介

**space-ast** (/æst/) 是一个专注于航天仿真领域的算法库，提供了轨道力学、姿态控制、轨迹规划等核心算法的现代C++实现。

如果你想要了解更多，请参考：[使用指南](https://space-ast.github.io/ast/), [API文档](https://space-ast.github.io/ast/api/), [Github](https://github.com/space-ast/ast)以及 [Gitee](https://gitee.com/space-ast/ast) 和 [GitCode](https://gitcode.com/space-ast/ast)。

## 🚀 核心特性

### 🛰️ 算法领域

- **轨道力学**：二体问题、轨道外推、兰伯特问题求解
- **姿态系统**：四元数运算、姿态动力学、控制算法
- **坐标转换**：J2000, MOD, TOD, ECF, LVLH等坐标系间的转换
- **环境模型**：大气密度、引力场、太阳辐射压
- **数值方法**：常微分方程数值积分、线性代数求解


## 📦 快速开始

### 📋 环境要求

- C++11兼容编译器 (MSVC 2015+, GCC 5+, clang)
- [xmake 2.9+](https://xmake.io/)


### 🏗️ 构建项目


```bash
git clone https://github.com/space-ast/ast.git --recursive
cd ast
xmake
```

## 🏗️ 项目架构

```
ast/
├── data/          # 数据
├── docs/          # 文档
├── examples/      # 示例代码片段
├── include/       # 头文件
├── projects/      # 示例工程
├── repo/          # 第三方库配置文件
├── scripts/       # 工具脚本
├── src/           # 源文件
├── test/          # 测试工程
└── thirdparty/    # 第三方库
```

## 🎯 API 设计理念

- **简单易用**：提供简洁的API接口，方便开发者快速集成到项目中。
- **高度可定制**：允许开发者根据需求自定义算法参数和行为。
- **性能优化**：针对航天仿真场景，对关键算法进行了性能优化。

## 📏代码规范

### 📝 命名规范

项目采用以下命名约定：

```cpp
// 命名空间 ast
namespace ast
{
    // ...
}

// 全局函数 - 单个'a'前缀
aPropagateOrbit();
aSolveLambert();
aRotateQuaternion();

// 类型别名 - 'A'前缀
typedef ast::OrbitPropagator    AOrbitPropagator;
typedef ast::AttitudeController AAttitudeController;

// 枚举 - 'E'和'e'前缀
enum EFrame { eECI, eECEF }; 

// 常量 - 'k' 前缀
const kEps15 = 1e-15;
```

可以发现：在以上命名规范里，所有以小写字母开头的都有值或者地址


### 👁️‍🗨️ 注释规范

遵循Doxygen注释规范，函数、文件、类的注释采用`///`格式，其他多行注释也可采用`/*! ... */`格式。

- **函数注释**：每个函数都需要包含详细的注释，包括参数说明、返回值、异常情况等。

- **类注释**：每个类都需要包含详细的注释，包括功能描述、成员变量、成员函数等。

- **文件注释**：每个源文件都需要包含文件头注释，包括文件名、作者、日期、版权信息等。


## 🤝 参与贡献

我们欢迎各种形式的贡献！无论是代码改进、文档完善、bug报告还是新功能建议，都是对项目的宝贵支持。

### 🔄 贡献流程

1. Fork 本仓库
2. 创建功能分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启 Pull Request

请确保代码遵循项目的编码规范，并添加相应的测试用例。

## 📄 许可证

本项目采用 Apache2.0 许可证 - 详见 [LICENSE](LICENSE) 文件。

## 🛠️ 开发状态

**当前状态**: 🟢 积极开发中

项目目前处于早期开发阶段，API可能会有较大变化。我们正在积极完善核心功能，欢迎早期使用者的反馈和建议。

### 🗂️ 模块规划

核心模块将认真设计与开发，并进行完善的测试与验证，确保算法的准确性和稳定性。

- [工具模块](src/AstUtil): 提供通用的工具函数，如数学运算、字符串处理等
- [数学库](src/AstMath): 提供数值计算、线性代数、优化等数学函数
- [仿真引擎](src/AstSim): 提供仿真环境，支持轨道模拟、姿态控制、环境交互等
- [航天算法](src/AstCore): 提供轨道力学、姿态系统、坐标转换等核心算法

下面的模块将通过AI辅助进行开发，但会通过人工审查和自动测试确保质量。

- [脚本系统](src/AstScript): 提供脚本接口，支持用户自定义仿真场景
- [交互式控制台](src/AstInterp): 提供交互式命令行界面，方便用户进行仿真实验
- [MatLab封装](src/AstMex): 提供MatLab接口，方便用户在MatLab环境中使用ast算法
- [Python封装](src/AstPy): 提供Python接口，方便用户在Python环境中使用ast算法
- [Web Assembly封装](src/AstWasm): 提供Web Assembly接口，方便用户在浏览器中使用ast算法
- [可视化](src/AstGfx): 提供可视化工具，方便用户查看仿真结果
- [图形化界面](src/AstGui): 提供图形化界面，方便用户进行仿真实验
- [代码生成器](src/AstCoder): 提供代码生成器，方便用户根据仿真场景生成代码


## 📞 联系我们

- 💬 讨论区: [GitHub Discussions](https://github.com/space-ast/ast/discussions)
- 🐛 问题报告: [GitHub Issues](https://github.com/space-ast/ast/issues)


## 📚 参考资料

ast项目的开发参考和使用了很多资料与工具，感谢所有开源社区贡献者的辛勤工作，以及航天仿真领域研究人员的理论贡献，这些宝贵的资源为ast项目的开发提供了坚实的基础。

### 💻 开源代码

- [Orekit](https://www.orekit.org/): Java语言编写的航天动力学库，提供丰富的轨道和姿态算法
- [GMAT](https://gmat.atlassian.net/): 通用任务分析工具，NASA开源的航天任务分析软件
- [Pagmo2](https://esa.github.io/pagmo2/): 欧洲空间局开发的并行全局优化库
- [PyGMO2](https://esa.github.io/pygmo2/): Pagmo2的Python绑定，用于科学计算的优化工具
- [PyKEP](https://esa.github.io/pykep/): 欧洲空间局的航天动力学Python库，专注于轨道优化
- [brahe](https://github.com/duncaneddy/brahe): 一个Rust航天动力学库，可用于研究和工程应用
- [satkit](https://github.com/ssmichael1/satkit): 一个Rust航天动力学库，实现了一些航天中的基础功能
- [AstroLib](https://gitee.com/wanghmail/AstroLib): 航天/卫星建模与仿真算法库
- [Eigen](http://eigen.tuxfamily.org/): 高性能C++模板库，用于线性代数、矩阵和向量运算
- [Scipy](https://scipy.org/): 一个Python库，用于科学计算和技术计算，提供了许多数学、科学和工程领域的功能
- [GNU Octave](https://octave.org/): 开源数值计算软件，提供与MATLAB兼容的语法
- [Hipparchus](https://hipparchus.org/): 一个Java数学库，提供了丰富的数学函数和算法
- [Qt](https://www.qt.io/): 跨平台C++图形用户界面应用程序开发框架
- [Python](https://www.python.org/): 流行的通用编程语言，在科学计算领域广泛应用
- [julia](https://julialang.org/): 一种高性能的动态编程语言，专为科学计算和数值分析而设计
- [glibc](https://www.gnu.org/software/libc/): GNU C库，提供了C语言标准库的实现
- [abseil](https://abseil.io/): 一个C++库，提供了许多常用的功能，如字符串处理、内存管理、并发编程等，是Google开源的一个项目

## 🔗 项目依赖

### 🔧 工具链

- [Xmake](https://xmake.io/): 基于Lua的现代化C/C++构建工具

### 📦 第三方库

- [iau-sofa](https://www.iausofa.org/): 一个C库，提供了许多常用的天文计算函数
- [Eigen](http://eigen.tuxfamily.org/): 高性能C++模板库，用于线性代数、矩阵和向量运算
- [fmt](https://fmt.dev/): 一个用于格式化字符串的C++库，提供了类似于Python的格式化语法
- [openscenegraph](https://www.openscenegraph.org/): 一个跨平台的3D图形库，用于渲染和交互显示3D场景
- [opengl](https://www.opengl.org/): 一个跨平台的3D图形库，用于渲染和交互显示3D场景
- [Qt](https://www.qt.io/): 跨平台C++图形用户界面应用程序开发框架


## 🌌 项目名称灵感

**ast** 这个名字蕴含着多重深意，每一层都呼应着航天仿真的本质：

### 🪐 **词源深意**

- **星辰之源**：源自希腊语词根 `aster` (ἀστήρ)，意为"星辰"
- **天文传承**：与 astronomy（天文学）、astronaut（宇航员）、Astrodynamics(航天动力学)等同源
- **宇宙探索**：象征着我们通过仿真技术探索宇宙奥秘的使命

### 🚀 **专业内涵**

- **Aerospace Simulation Tool**    - 航天仿真工具
- **Advanced Space Technology**    - 先进空间技术  
- **Astrodynamics Solver Tool**    - 航天动力学求解工具

---

"在代码的宇宙中，我们绘制航天的星辰轨迹"

---
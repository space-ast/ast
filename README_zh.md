# ast - 航天仿真算法库

![build](https://img.shields.io/github/actions/workflow/status/space-ast/ast/build-test.yml?branch=master&style=flat-square&logo=windows)
![C++](https://img.shields.io/badge/C++-11+-blue.svg)
![License](https://img.shields.io/badge/License-Apache2.0-green.svg)
![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows-lightgrey.svg)


**ast** (/æst/) 是一个用C++编写的开源航天仿真算法库，为航天任务分析和设计提供计算基础。

*像星辰般精准，为航天仿真而生*

[中文文档](README_zh.md)

## 🌟 项目简介

**ast** (/æst/) 是一个专注于航天仿真领域的算法库，提供了轨道力学、姿态控制、轨迹规划等核心算法的现代C++实现。

## 🚀 核心特性

### 🛰️ 算法领域

- **轨道力学**：二体问题、轨道外推、兰伯特问题求解
- **姿态系统**：四元数运算、姿态动力学、控制算法
- **坐标转换**：ECI/ECF/LVLH等坐标系转换
- **环境模型**：大气密度、引力场、太阳辐射压
- **数值方法**：常微分方程数值积分、线性代数求解


## 📦 快速开始

### 环境要求

- C++11兼容编译器 (GCC 5+, MSVC 2015+)
- [xmake 2.7+](https://xmake.io/)


### 构建项目

```bash
git clone https://github.com/space-ast/ast.git
cd ast
xmake
```

## 🏗️ 项目架构

```
ast/
├── 3rdparty/      # 第三方库
├── docs/          # 文档
├── include/       # 头文件
├── projects/      # 示例工程
├── scripts/       # 工具脚本
├── src/           # 源文件
└── test/          # 测试工程
```

## 🎯 API 设计理念

### 命名规范
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


## 🤝 参与贡献

我们欢迎各种形式的贡献！无论是代码改进、文档完善、bug报告还是新功能建议，都是对项目的宝贵支持。

### 贡献流程

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

### 模块规划

内核模块将仔细设计与开发，并通过测试工程提升测试覆盖率

- AstUtil:   工具函数
- AstCore:   动力学核心
- AstOpt:    非线性规划接口库
- AstOCP:    最优控制问题求解

下面的模块将通过AI辅助进行开发，此README也是AI辅助生成的

- AstScript: 脚本系统
- AstInterp: 交互式控制台
- AstMex:    MatLab封装
- AstPy:     Python封装
- AstWasm:   Web Assembly封装
- AstGfx:    可视化
- AstGui:    图形化界面


## 📞 联系我们

- 💬 讨论区: [GitHub Discussions](https://github.com/space-ast/ast/discussions)
- 🐛 问题报告: [GitHub Issues](https://github.com/space-ast/ast/issues)


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

# CLAUDE.md

本文件为 Claude Code（claude.ai/code）在此仓库中工作时提供指导。

## 构建系统

本项目使用 **xmake 2.9+**，而非 CMake。

```bash
xmake                       # 构建（默认 release 模式）
xmake f -m debug; xmake     # 构建 debug 模式
xmake f -m coverage; xmake  # 构建覆盖率模式
xmake test                  # 运行所有测试
xmake run <target>          # 运行指定目标（自动先构建）
xmake cpdata                # 将 data/ 目录拷贝到所有构建输出目录
xmake genheader             # 重新生成转发头文件、聚合头文件和 SWIG 接口文件
xmake pack -f zip           # 打包为 zip 压缩包
```

配置选项：`--with_test=y`（启用测试，默认开启）、`--check_warnings=y`（警告视为错误，默认开启）。

## 项目概述

**ast** 是一个面向航天仿真领域的 C++11 算法库，涵盖轨道力学、姿态动力学、坐标变换、环境模型和数值方法。编译产物为一组模块粒度的共享库（Windows 下为 DLL，Linux 下为 SO）。

跨平台：Windows（MSVC）和 Linux（GCC/Clang）。实验性支持 WASM。Windows 下 debug 构建会在库名末尾追加 `D` 后缀。

## 架构：模块化共享库

`src/Ast*` 和 `include/Ast*` 下的每个子目录定义一个模块，编译为共享库。`src/` 中的 `xmake.lua` 会自动包含所有 `*/xmake.lua`，因此新增模块只需创建目录并编写其 `xmake.lua` 即可。

| 模块 | 用途 |
|--------|---------|
| AstCore | 动力学核心：轨道、传播器、坐标系、时间系统、力模型、太阳系 |
| AstMath | 数学：线性代数、ODE 积分器、插值器、姿态变换、非线性方程 |
| AstUtil | 工具：容器、字符串、解析、IO、单位/量纲、RTTI、序列化、网络 |
| AstSim | 仿真：航天器运动、姿态、基础对象、访问者模式 |
| AstOpt | 优化：NLP 问题接口、Ipopt/Snopt/Minpack 求解器封装 |
| AstScript | 嵌入式脚本语言（自定义解释器、解析器、调试器） |
| AstCalc | 天文计算器与几何搜索 |
| AstAnalyzer | 任务分析：约束、变量、打靶法、灵敏度、权衡研究、遍历搜索 |
| AstGfx | 基于 OpenSceneGraph 的 3D 可视化（天体、航天器、设施） |
| AstGUI | 基于 Qt5 的 GUI 组件 |
| AstPy | 通过 SWIG 生成的 Python 绑定 |
| AstAI | AI 代理、对话服务 |
| AstCmd | CLI 命令系统（分发、路由） |
| AstCLI | CLI 应用程序入口 |
| AstCOM | Windows COM 封装 |
| AstSPICE | CSPICE 工具箱集成 |
| AstWeather | 空间天气模型 |
| AstMock | 模拟/测试工具 |
| AstLoader | 动态库加载器 |

`include/Ast.hpp` 是聚合头文件，包含所有核心模块头文件。`include/AstGlobal.h` 定义了命名空间宏、DLL 导出/导入声明以及常用类型的前置声明。

模块依赖顺序：AstUtil → AstMath → AstCore → AstSim。其他模块（AstOpt、AstGfx 等）依赖这些核心层。

## 代码规范

**命名约定**（来自 README）：
- 全局函数：`a` 前缀 — `aPropagateOrbit()`、`aSolveLambert()`
- 类型别名：`A` 前缀 — `AOrbitPropagator`、`AVector3d`
- 枚举：类型用 `E` 前缀，值用 `e` 前缀 — `EFrame { eECI, eECEF }`
- 常量：`k` 前缀 — `kEps15 = 1e-15`

**命名空间**：所有公开类型位于 `namespace ast` 中。命名空间由 `AstGlobal.h` 中的 `AST_ENABLE_NAMESPACE` 宏控制 — 代码必须使用 `AST_NAMESPACE_BEGIN` / `AST_NAMESPACE_END` 宏，而非直接写 `namespace ast {`。

**DLL 导出**：每个模块定义 `AST_<MODULE>_API` 宏（如 `AST_CORE_API`），由对应的构建宏（如 `AST_BUILD_LIB_CORE`）控制。公开类/函数上需使用这些宏。

**编译器抽象**：`include/AstCompiler.h` 提供了可移植宏，包括 `A_MSVC`、`A_GCC`、`A_CLANG`、`A_ALWAYS_INLINE`、`A_DECL_EXPORT/IMPORT`、`A_NODISCARD`、`A_DEPRECATED(msg)`、`A_RESTRICT`、`A_LIKELY/A_UNLIKELY` 等。

**前置声明**：`AstGlobal.h` 中声明了常用类型 — `Quaternion`、`Euler`、`CartState`、`OrbElem`、`JulianDate`、`Frame`、`CelestialBody`、`SpacecraftState` 等。

## 可选第三方依赖

所有依赖均为可选，通过 xmake 的 `add_requires(..., {optional = true})` 控制。构建时检查可用性：
- **Eigen** — 线性代数（`AST_WITH_EIGEN` / `AST_NO_EIGEN`）
- **fmt** — 格式化输出（`AST_WITH_FMT` / `AST_NO_FMT`）
- **CSPICE** — NASA SPICE 工具箱（`AST_WITH_CSPICE` / `AST_NO_CSPICE`）
- **matplot++** — 绘图（`AST_WITH_MATPLOT` / `AST_NO_MATPLOT`）
- **gtest**、**benchmark** — 测试与性能测试
- **Qt5**、**OpenSceneGraph**、**OpenGL** — GUI/可视化
- **Python**、**SWIG** — Python 绑定

自定义 xmake 包描述文件位于 `repo/packages/`（按首字母分为 `c/`、`l/`、`m/`、`s/` 子目录）。

## 测试

测试代码位于 `test/<Module>/`。每个 `test*.cpp`（匹配 `test/**/test*.cpp`）会自动成为一个独立的 xmake 二进制目标。脚本测试（`*.asc` 文件）通过 `ascript` 解释器运行。性能测试文件（`bm*.cpp`）使用 Google Benchmark。

```bash
xmake test                           # 运行所有测试
xmake run testState                  # 运行单个单元测试
xmake run testOrbitElement           # 按文件名基本名运行特定测试
```

CI 上（覆盖率工作流），构建前会通过 `cp -r test/Archive/* test/` 恢复归档的测试用例。

## CI

GitHub Actions 工作流：
- `build.yml` — 在 Windows 和 Linux 上构建（x64 + arm64），包括 debug 和 release。发布 tag 时上传产物。
- `coverage.yml` — 在 Ubuntu 上以覆盖率模式构建，运行测试，上传 lcov 数据到 Codecov。
- `deploy-docs.yml` — 将文档部署到 GitHub Pages。

## 自定义 xmake 任务

- `xmake cpdata` — 将 `data/` 拷贝到所有平台/模式组合的构建输出目录
- `xmake genheader` — 依次运行 Python 脚本：生成转发头文件、生成聚合头文件、生成 SWIG 接口文件
- `xmake gitpush` — 运行 git push 重试脚本

## Unity 构建

项目使用 xmake 的 C++ unity 构建（`c++.unity_build`，`batchsize=20`）。新增的 `.cpp` 文件会自动被批量合并编译，无需手动列举。C 语言的 unity 构建已禁用。

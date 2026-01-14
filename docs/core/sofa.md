# 基础天文计算模块

## 模块概述

SOFA模块是ast项目中的天文基础计算核心模块，基于国际天文联合会(IAU)的SOFA(Standards of Fundamental Astronomy)库实现。
该模块提供了一系列高精度的天文计算函数，用于处理岁差、章动、恒星时等天文数据的计算。

SOFA模块在原始SOFA库的基础上进行了接口适配，使其能够与ast项目的自定义类型（如TimePoint）无缝集成，并提供了更加灵活的扩展机制。
函数命名遵循ast项目的命名规范，部分实现细节根据项目需求进行了优化和调整，不保证与原始SOFA库在边界情况和精度上完全一致。

## 核心概念

### 1. SOFA库简介

SOFA库是国际天文联合会(IAU)发布的权威天文算法库，包含了实现各种天文标准模型的算法和程序。SOFA模块基于SOFA库2023-10-11版本实现。

### 2. 时间系统

SOFA模块涉及多种时间系统：
- **TT（地球时）**：用于计算岁差、章动等长期天文现象
- **TDB（质心动力学时）**：理论上用于高精度天文计算
- **UT1（世界时）**：用于计算地球自转角度

### 3. 天文标准

SOFA模块实现了多种IAU天文标准：
- **IAU1976**：岁差模型
- **IAU1980**：章动模型
- **IERS1996**：章动修正模型
- **IAU1982**：格林尼治平恒星时模型
- **IAU1994**：格林尼治视恒星时模型
- **IAU2000**：地球自转角度模型

## 主要功能

### 1. 岁差计算

- `aPrecession_IAU1976`：依据IAU1976规范计算岁差角

### 2. 章动计算

- `aNutation_IAU1980`：依据IAU1980规范计算章动角
- `aNutationFuncSet`：设置自定义章动角计算函数
- `aNutationMethodSet`：设置章动角计算方法

### 3. 黄赤交角计算

- `aMeanObliquity_IAU1980`：计算平均黄赤交角

### 4. 恒星时计算

- `aGMST_IAU1982`：计算格林尼治平恒星时
- `aGAST_IAU1994`：计算格林尼治视恒星时
- `aEquationOfEquinoxes_IAU1994`：计算赤经章动

### 5. 地球自转计算

- `aEarthRotationAngle_IAU2000`：计算地球自转角度

## 用法示例

### 1. 岁差角计算

@[code](/examples/core/sofa/precession.cpp)

### 2. 章动角计算

@[code](/examples/core/sofa/nutation.cpp)

### 3. 格林尼治恒星时计算

@[code](/examples/core/sofa/gmst_gast.cpp)


## 注意事项

1. **时间系统差异**：注意区分TT、TDB、UT1等不同时间系统的使用场景
2. **精度考量**：SOFA模块提供高精度计算，但某些近似计算（如直接用TT代替TDB）可能在极端情况下影响精度
3. **自定义实现**：可以通过`aNutationFuncSet`设置自定义章动角计算函数，但需确保接口一致性
4. **SOFA许可证**：SOFA模块基于SOFA库衍生，需遵守SOFA软件许可证条款


## 许可证声明

本模块基于国际天文联合会(IAU)的SOFA软件库衍生实现，必须遵守SOFA软件许可证条款。

根据SOFA软件许可证的要求，特此声明：

1. 本模块的代码使用了从SOFA软件库（版本2023-10-11）获得的算法和计算例程。
   具体对应的SOFA函数请参考函数注释

2. SOFA软件由国际天文联合会(IAU) SOFA Board所有。
   SOFA软件许可证请参考：https://www.iausofa.org/terms-and-conditions

3. 根据SOFA许可证第3(c)条要求，本衍生作品的函数名不使用"iau"或"sofa"前缀。
   本模块使用"a"前缀表示"astronomy"天文计算。

4. 根据SOFA许可证第3(a)条要求，声明本软件：
   (i) 使用了从SOFA软件派生的计算例程；
   (ii) 不是由SOFA提供或认可的软件。

5. 本衍生作品与原始SOFA软件的主要区别：
   - 函数接口针对ast项目的TimePoint等自定义类型进行了适配
   - 函数命名遵循ast项目的命名规范
   - 部分实现细节根据项目需求进行了优化和调整
   - 不保证与原始SOFA库在边界情况和精度上完全一致

6. 根据SOFA许可证第3(e)条要求，必须在SOFA软件衍生作品源代码分发中完整保留SOFA软件的版权声明
   SOFA软件的版权声明请参阅源代码文件。


## API参考

@[code](/src/AstCore/Coordinate/IERS/SOFA.hpp)

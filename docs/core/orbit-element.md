# 轨道根数定义及转换

*说明：本文档旨在帮助理解模块功能，具体API和行为请查阅源代码。*

## 模块概述

OrbitElement模块提供各种轨道根数（直角坐标、经典轨道根数、修正轨道根数、春分点根数、改进春分点轨道根数）之间的转换函数，是天体力学和航天动力学计算的基础组件。

## 核心概念

### 轨道根数类型

模块定义了以下主要轨道根数类型：

1. **直角坐标系 (CartState)**
   - 包含位置矢量和速度矢量
   - 直观表示空间位置和运动状态

2. **经典轨道根数 (OrbElem)**
   - 长半轴 (a)
   - 偏心率 (e)
   - 轨道倾角 (i)
   - 升交点赤经 (raan)
   - 近拱点角 (argper)
   - 真近点角 (trueA)

3. **修正轨道根数 (ModOrbElem)**
   - 近拱点半径 (rp)
   - 偏心率 (e)
   - 轨道倾角 (i)
   - 升交点赤经 (raan)
   - 近拱点角 (argper)
   - 真近点角 (trueA)

4. **春分点根数 (EquinElem)**
   - 长半轴 (a)
   - h = e*sin(argper + raan)
   - k = e*cos(argper + raan)
   - p = tan(i/2)*sin(raan)
   - q = tan(i/2)*cos(raan)
   - lambda = 平经度 = M + raan + argper

5. **改进春分点轨道根数 (ModEquinElem)**
   - 半通径 (p = a(1-e^2))
   - f = e*cos(argper+raan)
   - g = e*sin(argper+raan)
   - h = tan(i/2)cos(raan)
   - k = tan(i/2)sin(raan)
   - L = raan + argper + trueA

### 关键常数和单位

- 位置单位：米 (m)
- 速度单位：米/秒 (m/s)
- 角度单位：弧度 (rad)
- 引力参数单位：立方米/秒平方 (m³/s²)

## 用法示例

### 1. 经典轨道根数与直角坐标转换

@[code](/examples/core/orbitelement/convert_classical_scalar.cpp)

### 2. 使用类引用版本的函数

@[code](/examples/core/orbitelement/convert_classical_class.cpp)

### 3. 不同轨道根数类型之间的转换

@[code](/examples/core/orbitelement/convert_basic.cpp)

## 依赖关系

- `AstGlobal.h`: 项目全局定义
- `AstCore/Vector.hpp`: 向量类支持

## 注意事项

- 所有角度参数均使用弧度制
- 距离参数默认使用米(m)
- 时间参数默认使用秒(s)
- 引力参数单位为m³/s²
- 部分转换函数返回错误码，请适当处理错误情况
- 零偏心率轨道（圆轨道）在某些转换中可能需要特殊处理
- 改进春分点轨道根数在轨道倾角接近180度时可能存在奇异情况

## API参考

@[code](/src/AstCore/Orbit/OrbitElement.hpp)
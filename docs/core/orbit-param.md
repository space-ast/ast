# OrbitParam 轨道参数计算

*说明：本文档旨在帮助理解模块功能，具体API和行为请查阅源代码。*

## 模块概述

OrbitParam模块提供各种轨道参数之间的转换函数，支持近地点/远地点高度/半径、周期、平均角速度、偏心率等参数的相互转换，以及近点角（真近点角、偏近点角、平近点角）之间的转换。

## 核心概念

### 轨道要素类型

模块支持以下主要轨道要素的计算和转换：

1. **近地点/远地点要素**
   - 近地点高度/半径
   - 远地点高度/半径
   - 近地点幅角时刻/通过时刻

2. **近点角**
   - 真近点角（True Anomaly）
   - 偏近点角（Eccentric Anomaly）
   - 平近点角（Mean Anomaly）

3. **轨道几何参数**
   - 长半轴
   - 偏心率
   - 扁平率
   - 轨道周期
   - 平均角速度

4. **角度关系**
   - 近地点幅角
   - 近地点经度
   - 幅角
   - 真近点经度

### 关键常数

- 所有长度单位默认使用米（m）
- 所有角度单位默认使用弧度（rad）
- 所有时间单位默认使用秒（s）
- 迭代函数默认精度为1e-14，最大迭代次数为100次

## 用法示例

### 1. 半径与高度转换

以下示例展示如何在近地点/远地点半径和高度之间进行转换：

```cpp
#include "AstCore/OrbitParam.hpp"

// 地球平均半径 (m)
double earthRadius = 6371000.0;

// 近地点高度转半径
double perigeeAlt = 300000.0; // 300 km
double perigeeRadius = aPeriAltToPeriRad(perigeeAlt, earthRadius);

// 远地点半径转高度
double apogeeRadius = 42164000.0; // 地球同步轨道半径
double apogeeAlt = aApoRadToApoAlt(apogeeRadius, earthRadius);
```

### 2. 轨道周期与角速度计算

以下示例展示如何计算轨道周期和平均角速度：

```cpp
#include "AstCore/OrbitParam.hpp"

// 地球引力参数 (m^3/s^2)
double gmEarth = 3.986004418e14;

// 从长半轴计算轨道周期
double semiMajorAxis = 42164000.0; // 地球同步轨道长半轴
double period = aSMajAxToPeriod(semiMajorAxis, gmEarth);

// 从轨道周期计算平均角速度
double meanMotion = aPeriodToMeanMotn(period);
```

### 3. 近点角转换

```cpp
#include "AstCore/OrbitParam.hpp"

// 真近点角转偏近点角
double trueAnomaly = 30.0 * AST_DEG_TO_RAD;
double eccentricity = 0.1;
double eccentricAnomaly = aTrueToEcc(trueAnomaly, eccentricity);

// 偏近点角转平近点角
double meanAnomaly = aEccToMean(eccentricAnomaly, eccentricity);

// 反向转换 - 平近点角转真近点角（迭代求解）
double tolerance = 1e-15;
int maxIter = 50;
double trueAnomalyReconverted = aMeanToTrue(meanAnomaly, eccentricity, tolerance, maxIter);
```

### 4. 完整轨道分析

```cpp
#include "AstCore/OrbitParam.hpp"

// 地球参数
double earthRadius = 6371000.0;
double gmEarth = 3.986004418e14;

// 已知近地点高度和偏心率
double perigeeAlt = 200000.0; // 200 km
double eccentricity = 0.01;

// 计算其他轨道参数
double perigeeRad = aPeriAltToPeriRad(perigeeAlt, earthRadius);
double apogeeAlt = aPeriAltToApoAlt(perigeeAlt, eccentricity, earthRadius);
double semiMajorAxis = aPeriRadToSMajAx(perigeeRad, eccentricity);
double period = aPeriRadToPeriod(perigeeRad, eccentricity, gmEarth);
double meanMotion = aSMajAxToMeanMotn(semiMajorAxis, gmEarth);
```

## API 参考

### 近地点/远地点转换

#### 近地点高度与半径转换

| 函数名 | 说明 | 参数 | 返回值 |
|--------|------|------|--------|
| `aPeriAltToPeriRad` | 近地点高度转半径 | perigeeAlt: 近地点高度 (m)<br>cbRadius: 中心天体半径 (m) | 近地点半径 (m) |
| `aPeriRadToPeriAlt` | 近地点半径转高度 | perigeeRad: 近地点半径 (m)<br>cbRadius: 中心天体半径 (m) | 近地点高度 (m) |

#### 远地点高度与半径转换

| 函数名 | 说明 | 参数 | 返回值 |
|--------|------|------|--------|
| `aApoAltToApoRad` | 远地点高度转半径 | apogeeAlt: 远地点高度 (m)<br>cbRadius: 中心天体半径 (m) | 远地点半径 (m) |
| `aApoRadToApoAlt` | 远地点半径转高度 | apogeeRad: 远地点半径 (m)<br>cbRadius: 中心天体半径 (m) | 远地点高度 (m) |

#### 近地点与远地点之间的转换

| 函数名 | 说明 | 参数 | 返回值 |
|--------|------|------|--------|
| `aPeriAltToApoAlt` | 近地点高度转远地点高度 | perigeeAlt: 近地点高度 (m)<br>eccentricity: 偏心率<br>cbRadius: 中心天体半径 (m) | 远地点高度 (m) |
| `aPeriAltToApoRad` | 近地点高度转远地点半径 | perigeeAlt: 近地点高度 (m)<br>eccentricity: 偏心率<br>cbRadius: 中心天体半径 (m) | 远地点半径 (m) |
| `aApoAltToPeriAlt` | 远地点高度转近地点高度 | apogeeAlt: 远地点高度 (m)<br>eccentricity: 偏心率<br>cbRadius: 中心天体半径 (m) | 近地点高度 (m) |
| `aApoAltToPeriRad` | 远地点高度转近地点半径 | apogeeAlt: 远地点高度 (m)<br>eccentricity: 偏心率<br>cbRadius: 中心天体半径 (m) | 近地点半径 (m) |

### 近点角转换

#### 真近点角与偏近点角转换

| 函数名 | 说明 | 参数 | 返回值 |
|--------|------|------|--------|
| `aTrueToEcc` | 真近点角转偏近点角 | trueAnomaly: 真近点角 (rad)<br>eccentricity: 偏心率 | 偏近点角 (rad) |
| `aEccToTrue` | 偏近点角转真近点角 | eccentricAnomaly: 偏近点角 (rad)<br>eccentricity: 偏心率 | 真近点角 (rad) |

#### 偏近点角与平近点角转换

| 函数名 | 说明 | 参数 | 返回值 |
|--------|------|------|--------|
| `aEccToMean` | 偏近点角转平近点角 | eccentricAnomaly: 偏近点角 (rad)<br>eccentricity: 偏心率 | 平近点角 (rad) |
| `aMeanToEcc` | 平近点角转偏近点角 | meanAnomaly: 平近点角 (rad)<br>eccentricity: 偏心率<br>eps: 迭代精度 (默认1e-14)<br>maxIter: 最大迭代次数 (默认100) | 偏近点角 (rad) |

#### 平近点角与真近点角转换

| 函数名 | 说明 | 参数 | 返回值 |
|--------|------|------|--------|
| `aTrueToMean` | 真近点角转平近点角 | trueAnomaly: 真近点角 (rad)<br>eccentricity: 偏心率 | 平近点角 (rad) |
| `aMeanToTrue` | 平近点角转真近点角 | meanAnomaly: 平近点角 (rad)<br>eccentricity: 偏心率<br>eps: 迭代精度 (默认1e-14)<br>maxIter: 最大迭代次数 (默认100) | 真近点角 (rad) |

### 轨道周期与平均角速度

| 函数名 | 说明 | 参数 | 返回值 |
|--------|------|------|--------|
| `aPeriodToMeanMotn` | 轨道周期转平均角速度 | period: 轨道周期 (s) | 平均角速度 (rad/s) |
| `aMeanMotnToPeriod` | 平均角速度转轨道周期 | meanMotn: 平均角速度 (rad/s) | 轨道周期 (s) |

### 长半轴相关转换

| 函数名 | 说明 | 参数 | 返回值 |
|--------|------|------|--------|
| `aPeriAltToSMajAx` | 近地点高度转长半轴 | perigeeAlt: 近地点高度 (m)<br>eccentricity: 偏心率<br>cbRadius: 中心天体半径 (m) | 长半轴 (m) |
| `aPeriRadToSMajAx` | 近地点半径转长半轴 | perigeeRad: 近地点半径 (m)<br>eccentricity: 偏心率 | 长半轴 (m) |
| `aApoAltToSMajAx` | 远地点高度转长半轴 | apogeeAlt: 远地点高度 (m)<br>eccentricity: 偏心率<br>cbRadius: 中心天体半径 (m) | 长半轴 (m) |
| `aApoRadToSMajAx` | 远地点半径转长半轴 | apogeeRad: 远地点半径 (m)<br>eccentricity: 偏心率 | 长半轴 (m) |
| `aSMajAxToPeriRad` | 长半轴转近地点半径 | semiMajorAxis: 长半轴 (m)<br>eccentricity: 偏心率 | 近地点半径 (m) |
| `aSMajAxToApoRad` | 长半轴转远地点半径 | semiMajorAxis: 长半轴 (m)<br>eccentricity: 偏心率 | 远地点半径 (m) |
| `aSMajAxToPeriAlt` | 长半轴转近地点高度 | semiMajorAxis: 长半轴 (m)<br>eccentricity: 偏心率<br>cbRadius: 中心天体半径 (m) | 近地点高度 (m) |
| `aSMajAxToApoAlt` | 长半轴转远地点高度 | semiMajorAxis: 长半轴 (m)<br>eccentricity: 偏心率<br>cbRadius: 中心天体半径 (m) | 远地点高度 (m) |
| `aSMajAxToPeriod` | 长半轴转轨道周期 | semiMajorAxis: 长半轴 (m)<br>gm: 引力参数 (m^3/s^2) | 轨道周期 (s) |
| `aSMajAxToMeanMotn` | 长半轴转平均角速度 | semiMajorAxis: 长半轴 (m)<br>gm: 引力参数 (m^3/s^2) | 平均角速度 (rad/s) |
| `aPeriodToSMajAx` | 轨道周期转长半轴 | period: 轨道周期 (s)<br>gm: 引力参数 (m^3/s^2) | 长半轴 (m) |

### 其他轨道参数计算

| 函数名 | 说明 | 参数 | 返回值 |
|--------|------|------|--------|
| `aRadiiToEcc` | 根据近地点和远地点半径计算偏心率 | perigeeRad: 近地点半径 (m)<br>apogeeRad: 远地点半径 (m) | 偏心率 |
| `aSMinAxToSMajAx` | 短半轴转长半轴 | semiminorAxis: 短半轴 (m)<br>eccentricity: 偏心率 | 长半轴 (m) |

### 时间相关转换

| 函数名 | 说明 | 参数 | 返回值 |
|--------|------|------|--------|
| `aTPANToEcc` | 近地点幅角时刻转偏近点角 | TPAN: 近地点幅角时刻 (s)<br>argPeri: 近地点幅角 (rad)<br>semiMajorAxis: 长半轴 (m)<br>eccentricity: 偏心率<br>gm: 引力参数 (m^3/s^2)<br>eps: 迭代精度 (默认1e-14)<br>maxIter: 最大迭代次数 (默认100) | 偏近点角 (rad) |
| `aTPANToMean` | 近地点幅角时刻转平近点角 | TPAN: 近地点幅角时刻 (s)<br>argPeri: 近地点幅角 (rad)<br>semiMajorAxis: 长半轴 (m)<br>eccentricity: 偏心率<br>gm: 引力参数 (m^3/s^2) | 平近点角 (rad) |
| `aTPPToTrue` | 近地点通过时刻转真近点角 | TPP: 近地点通过时刻 (s)<br>semiMajorAxis: 长半轴 (m)<br>eccentricity: 偏心率<br>gm: 引力参数 (m^3/s^2)<br>eps: 迭代精度 (默认1e-14)<br>maxIter: 最大迭代次数 (默认100) | 真近点角 (rad) |
| `aTrueToTPAN` | 真近点角转近地点幅角时刻 | trueAnomaly: 真近点角 (rad)<br>argPeri: 近地点幅角 (rad)<br>semiMajorAxis: 长半轴 (m)<br>eccentricity: 偏心率<br>gm: 引力参数 (m^3/s^2) | 近地点幅角时刻 (s) |
| `aTrueToTPP` | 真近点角转近地点通过时刻 | trueAnomaly: 真近点角 (rad)<br>semiMajorAxis: 长半轴 (m)<br>eccentricity: 偏心率<br>gm: 引力参数 (m^3/s^2) | 近地点通过时刻 (s) |

### 角度关系转换

| 函数名 | 说明 | 参数 | 返回值 |
|--------|------|------|--------|
| `aTrueToArgLat` | 真近点角转幅角 | trueAnomaly: 真近点角 (rad)<br>argPeri: 近地点幅角 (rad) | 幅角 (rad) |
| `aTrueToTrueLong` | 真近点角转真近点经度 | trueAnomaly: 真近点角 (rad)<br>argPeri: 近地点幅角 (rad)<br>raan: 升交点赤经 (rad) | 真近点经度 (rad) |
| `aArgPeriToLongPeri` | 近地点幅角转近地点经度 | argPeri: 近地点幅角 (rad)<br>raan: 升交点赤经 (rad) | 近地点经度 (rad) |

## 依赖关系

- `AstGlobal.h`: 项目全局定义
- `math.h`: 数学函数支持
- 相关数学运算模块

## 注意事项

- 所有角度参数均使用弧度制
- 距离参数默认使用米(m)
- 时间参数默认使用秒(s)
- 迭代计算函数包含精度和最大迭代次数参数，可根据实际需求调整
- 对于高偏心率轨道，可能需要增加最大迭代次数以确保收敛
- 零偏心率轨道（圆轨道）有特殊处理逻辑，避免除以零错误

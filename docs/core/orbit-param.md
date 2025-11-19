# OrbitParam 轨道参数计算

*说明：本文档旨在帮助理解模块功能，具体API和行为请查阅源代码。*

## 模块概述

OrbitParam模块提供了航天仿真中常用的轨道参数计算和转换功能，支持各种轨道元素之间的相互转换、轨道特性计算等。该模块是航天动力学仿真的基础组件，为轨道分析、任务设计提供必要的计算支持。

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
   - 平近点角的时间导数

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

## 关键常数

- 地球平均半径（用于高度和半径转换）
- 迭代计算的精度参数（通常为1e-15）
- 最大迭代次数（确保数值方法收敛）

## 用法示例

### 1. 半径与高度转换

以下示例展示如何在近地点/远地点半径和高度之间进行转换：

```cpp
#include "AstCore/OrbitParam.hpp"

// 地球平均半径 (m)
double earthRadius = 6371000.0;

// 近地点高度转半径
double perigeeAlt = 300000.0; // 300 km
double perigeeRadius = aPerigeeAltToRadius(perigeeAlt, earthRadius);

// 远地点半径转高度
double apogeeRadius = 42164000.0; // 地球同步轨道半径
double apogeeAlt = aApogeeRadiusToAlt(apogeeRadius, earthRadius);
```

### 2. 轨道周期与角速度计算

```cpp
#include "AstCore/OrbitParam.hpp"

// 计算轨道周期
double semiMajorAxis = 7000000.0; // 7000 km
double gm = 398600441800000.0; // 地球引力常数 (m³/s²)
double period = aSemiMajorAxisToPeriod(semiMajorAxis, gm);

// 计算平均角速度
double meanAngularVelocity = aSemiMajorAxisToMeanAngularVelocity(semiMajorAxis, gm);
```

### 3. 近点角转换

```cpp
#include "AstCore/OrbitParam.hpp"

// 真近点角转偏近点角
double trueAnomaly = 30.0 * AST_DEG_TO_RAD;
double eccentricity = 0.1;
double eccentricAnomaly = aTrueAnomalyToEccentricAnomaly(trueAnomaly, eccentricity);

// 偏近点角转平近点角
double meanAnomaly = aEccentricAnomalyToMeanAnomaly(eccentricAnomaly, eccentricity);

// 反向转换 - 平近点角转真近点角（迭代求解）
double tolerance = 1e-15;
int maxIter = 50;
double trueAnomalyReconverted = aMeanAnomalyToTrueAnomaly(meanAnomaly, eccentricity, tolerance, maxIter);
```

### 4. 完整轨道分析

```cpp
#include "AstCore/OrbitParam.hpp"

// 已知近地点和远地点高度，计算完整轨道参数
double perigeeAlt = 300000.0; // 300 km
double apogeeAlt = 35786000.0; // 地球同步轨道高度
double earthRadius = 6371000.0;
double gm = 398600441800000.0;

// 计算半径
double perigeeRadius = aPerigeeAltToRadius(perigeeAlt, earthRadius);
double apogeeRadius = aApogeeAltToRadius(apogeeAlt, earthRadius);

// 计算长半轴和偏心率
double semiMajorAxis = aPerigeeApogeeRadiusToSemiMajorAxis(perigeeRadius, apogeeRadius);
double eccentricity = aPerigeeApogeeRadiusToEccentricity(perigeeRadius, apogeeRadius);

// 计算轨道周期
double period = aSemiMajorAxisToPeriod(semiMajorAxis, gm);
```

## API 参考

### 近地点/远地点转换

#### 近地点高度与半径转换

| 函数名 | 说明 | 参数 | 返回值 |
|--------|------|------|--------|
| `aPerigeeAltToRadius` | 近地点高度转半径 | alt: 近地点高度 (m)<br>centralBodyRadius: 中心天体半径 (m) | 近地点半径 (m) |
| `aPerigeeRadiusToAlt` | 近地点半径转高度 | radius: 近地点半径 (m)<br>centralBodyRadius: 中心天体半径 (m) | 近地点高度 (m) |

#### 远地点高度与半径转换

| 函数名 | 说明 | 参数 | 返回值 |
|--------|------|------|--------|
| `aApogeeAltToRadius` | 远地点高度转半径 | alt: 远地点高度 (m)<br>centralBodyRadius: 中心天体半径 (m) | 远地点半径 (m) |
| `aApogeeRadiusToAlt` | 远地点半径转高度 | radius: 远地点半径 (m)<br>centralBodyRadius: 中心天体半径 (m) | 远地点高度 (m) |

### 近点角转换

#### 真近点角与偏近点角转换

| 函数名 | 说明 | 参数 | 返回值 |
|--------|------|------|--------|
| `aTrueAnomalyToEccentricAnomaly` | 真近点角转偏近点角 | trueAnomaly: 真近点角 (rad)<br>eccentricity: 偏心率 | 偏近点角 (rad) |
| `aEccentricAnomalyToTrueAnomaly` | 偏近点角转真近点角 | eccentricAnomaly: 偏近点角 (rad)<br>eccentricity: 偏心率 | 真近点角 (rad) |

#### 偏近点角与平近点角转换

| 函数名 | 说明 | 参数 | 返回值 |
|--------|------|------|--------|
| `aEccentricAnomalyToMeanAnomaly` | 偏近点角转平近点角 | eccentricAnomaly: 偏近点角 (rad)<br>eccentricity: 偏心率 | 平近点角 (rad) |
| `aMeanAnomalyToEccentricAnomaly` | 平近点角转偏近点角 | meanAnomaly: 平近点角 (rad)<br>eccentricity: 偏心率<br>tolerance: 迭代精度<br>maxIter: 最大迭代次数 | 偏近点角 (rad) |

#### 平近点角与真近点角转换

| 函数名 | 说明 | 参数 | 返回值 |
|--------|------|------|--------|
| `aTrueAnomalyToMeanAnomaly` | 真近点角转平近点角 | trueAnomaly: 真近点角 (rad)<br>eccentricity: 偏心率 | 平近点角 (rad) |
| `aMeanAnomalyToTrueAnomaly` | 平近点角转真近点角 | meanAnomaly: 平近点角 (rad)<br>eccentricity: 偏心率<br>tolerance: 迭代精度<br>maxIter: 最大迭代次数 | 真近点角 (rad) |

### 轨道周期与角速度

| 函数名 | 说明 | 参数 | 返回值 |
|--------|------|------|--------|
| `aSemiMajorAxisToPeriod` | 长半轴转轨道周期 | semiMajorAxis: 长半轴 (m)<br>gm: 引力常数 (m³/s²) | 轨道周期 (s) |
| `aPeriodToSemiMajorAxis` | 轨道周期转长半轴 | period: 轨道周期 (s)<br>gm: 引力常数 (m³/s²) | 长半轴 (m) |
| `aSemiMajorAxisToMeanAngularVelocity` | 长半轴转平均角速度 | semiMajorAxis: 长半轴 (m)<br>gm: 引力常数 (m³/s²) | 平均角速度 (rad/s) |
| `aMeanAngularVelocityToSemiMajorAxis` | 平均角速度转长半轴 | meanAngularVelocity: 平均角速度 (rad/s)<br>gm: 引力常数 (m³/s²) | 长半轴 (m) |

### 偏心率计算

| 函数名 | 说明 | 参数 | 返回值 |
|--------|------|------|--------|
| `aPerigeeApogeeRadiusToEccentricity` | 近地点远地点半径计算偏心率 | perigeeRadius: 近地点半径 (m)<br>apogeeRadius: 远地点半径 (m) | 偏心率 |
| `aSemiMajorAxisPerigeeRadiusToEccentricity` | 长半轴和近地点半径计算偏心率 | semiMajorAxis: 长半轴 (m)<br>perigeeRadius: 近地点半径 (m) | 偏心率 |
| `aSemiMajorAxisApogeeRadiusToEccentricity` | 长半轴和远地点半径计算偏心率 | semiMajorAxis: 长半轴 (m)<br>apogeeRadius: 远地点半径 (m) | 偏心率 |

### 角度关系计算

| 函数名 | 说明 | 参数 | 返回值 |
|--------|------|------|--------|
| `aTrueAnomalyToArgument` | 真近点角转幅角 | trueAnomaly: 真近点角 (rad)<br>argumentOfPerigee: 近地点幅角 (rad) | 幅角 (rad) |
| `aArgumentToTrueAnomaly` | 幅角转真近点角 | argument: 幅角 (rad)<br>argumentOfPerigee: 近地点幅角 (rad) | 真近点角 (rad) |
| `aTrueAnomalyToTrueLongitude` | 真近点角转真近点经度 | trueAnomaly: 真近点角 (rad)<br>argumentOfPerigee: 近地点幅角 (rad)<br>longitudeOfAscendingNode: 升交点赤经 (rad) | 真近点经度 (rad) |
| `aTrueLongitudeToTrueAnomaly` | 真近点经度转真近点角 | trueLongitude: 真近点经度 (rad)<br>argumentOfPerigee: 近地点幅角 (rad)<br>longitudeOfAscendingNode: 升交点赤经 (rad) | 真近点角 (rad) |
| `aArgumentOfPerigeeToLongitudeOfPerigee` | 近地点幅角转近地点经度 | argumentOfPerigee: 近地点幅角 (rad)<br>longitudeOfAscendingNode: 升交点赤经 (rad) | 近地点经度 (rad) |
| `aLongitudeOfPerigeeToArgumentOfPerigee` | 近地点经度转近地点幅角 | longitudeOfPerigee: 近地点经度 (rad)<br>longitudeOfAscendingNode: 升交点赤经 (rad) | 近地点幅角 (rad) |

### 其他轨道特性计算

| 函数名 | 说明 | 参数 | 返回值 |
|--------|------|------|--------|
| `aPerigeeApogeeRadiusToSemiMajorAxis` | 近地点远地点半径计算长半轴 | perigeeRadius: 近地点半径 (m)<br>apogeeRadius: 远地点半径 (m) | 长半轴 (m) |
| `aEccentricityToFlattening` | 偏心率转扁平率 | eccentricity: 偏心率 | 扁平率 |
| `aFlatteningToEccentricity` | 扁平率转偏心率 | flattening: 扁平率 | 偏心率 |
| `aCalculateSemiMajorAxisForGroundTrackRepeat` | 计算地面轨迹重复的长半轴 | centralBodyRadius: 中心天体半径 (m)<br>centralBodyAngularVelocity: 中心天体自转角速度 (rad/s)<br>numOrbits: 重复周期内的轨道数<br>numDays: 重复周期的天数<br>gm: 引力常数 (m³/s²) | 长半轴 (m) |

## 依赖关系

- `AstGlobal.hpp`: 项目全局定义
- `math.h`: 数学函数支持
- 相关数学运算模块

## 注意事项

- 所有角度参数均使用弧度制
- 距离参数默认使用米(m)
- 时间参数默认使用秒(s)
- 迭代计算函数包含精度和最大迭代次数参数，可根据实际需求调整
- 对于高偏心率轨道，可能需要增加最大迭代次数以确保收敛
- 零偏心率轨道（圆轨道）有特殊处理逻辑，避免除以零错误

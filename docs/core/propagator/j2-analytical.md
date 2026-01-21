# J2解析预报器

## 概述

J2解析预报器考虑地球扁率J2项的解析预报器，提供比二体预报器更高精度的轨道预报。该方法在保持较高计算效率的同时，能够考虑地球非球形引力场的主要摄动效应。

## 核心概念

### J2摄动项

J2项是地球引力场球谐展开中的主要项，表示地球的扁率效应。J2摄动主要影响：

- **轨道面进动**：轨道平面绕地球自转轴进动
- **近地点进动**：近地点在轨道面内旋转

### 预报原理

J2解析预报器基于平均轨道根数的长期变化率：

- 计算J2摄动引起的轨道根数长期变化率
- 对轨道根数进行线性预报
- 计算对应的位置和速度向量

## 主要接口

### 类接口

```cpp
class J2Analytical : public J2J4Analytical
```

**构造函数：**

```cpp
J2Analytical(const ModOrbElem& modOrbElem, const TimePoint& epoch, double gm, double j2, double re);
J2Analytical(const Vector3d& pos, const Vector3d& vel, const TimePoint& epoch, double gm, double j2, double re);
```

### 函数接口

```cpp
AST_CORE_API err_t aJ2AnalyticalProp(double duration, double gm, double j2, double re, Vector3d& r, Vector3d& v);
AST_CORE_API err_t aJ2AnalyticalProp(double duration, double gm, double j2, double re, ModOrbElem& modOrbElem);
```

**参数说明：**

- `duration`：传播时间间隔（秒）
- `gm`：引力常数（m³/s²）
- `j2`：J2项系数（无量纲）
- `re`：天体半径（米）
- `r`：输入/输出位置向量（米）
- `v`：输入/输出速度向量（米/秒）
- `modOrbElem`：输入/输出改进轨道根数

## 用法示例

### 1. 位置速度传播

@[code](/examples/propagator/j2_analytical/j2_position_velocity_prop.cpp)

### 2. 轨道根数传播

@[code](/examples/propagator/j2_analytical/j2_orbital_elements_prop.cpp)

### 3. 类接口使用

@[code](/examples/propagator/j2_analytical/j2_class_interface.cpp)

## 精度与限制

### 精度特点

- **计算精度**：考虑J2项长期摄动，精度优于二体传播器
- **适用范围**：适用于需要考虑地球扁率效应的近地轨道
- **计算效率**：较高，适合需要较高效率的应用

### 使用限制

- 仅考虑J2项摄动，忽略高阶项和其他摄动力
- 对于高精度轨道预报需求，建议使用HPOP等数值方法
- 长时间传播时，由于模型简化，误差会累积

## 地球J2参数

对于地球轨道计算，推荐使用以下参数：

- **J2系数**：1.08262668e-3
- **地球半径**：6378137.0米
- **引力常数**：3.986004418e14 m³/s²

## 注意事项

- 输入的位置和速度向量应在同一惯性坐标系下，例如J2000、ICRF、TOD等
- J2系数和天体半径应与中心天体匹配
- 传播时间间隔不宜过长，避免数值误差累积

## API参考

@[code](/src/AstCore/Propagator/J2Analytical.hpp)
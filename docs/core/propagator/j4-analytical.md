# J4解析预报器

## 概述

J4解析预报器考虑地球扁率J2和J4项的解析预报器，提供比J2预报器更高精度的轨道预报。该方法在保持较高计算效率的同时，能够更准确地模拟地球非球形引力场的摄动效应。

## 核心概念

### J4摄动项

J4项是地球引力场球谐展开中的高阶项，J4摄动与J2摄动共同作用，影响：

- **轨道面进动**：更精确的轨道平面进动计算
- **近地点进动**：更准确的近地点旋转速率

### 预报原理

J4解析预报器扩展了J2预报器的模型：

- 计算J2和J4摄动引起的轨道根数长期变化率
- 对轨道根数进行线性预报
- 计算对应的位置和速度向量

## 主要接口

### 类接口

```cpp
class J4Analytical: public J2J4Analytical
```

**构造函数：**

```cpp
J4Analytical(const ModOrbElem& modOrbElem, const TimePoint& epoch, double gm, double j2, double j4, double re);
J4Analytical(const Vector3d& pos, const Vector3d& vel, const TimePoint& epoch, double gm, double j2, double j4, double re);
```

### 函数接口

```cpp
AST_CORE_API err_t aJ4AnalyticalProp(double duration, double gm, double j2, double j4, double re, Vector3d& r, Vector3d& v);
AST_CORE_API err_t aJ4AnalyticalProp(double duration, double gm, double j2, double j4, double re, ModOrbElem& modOrbElem);
```

**参数说明：**

- `duration`：传播时间间隔（秒）
- `gm`：引力常数（m³/s²）
- `j2`：J2项系数（无量纲）
- `j4`：J4项系数（无量纲）
- `re`：天体半径（米）
- `r`：输入/输出位置向量（米）
- `v`：输入/输出速度向量（米/秒）
- `modOrbElem`：输入/输出改进轨道根数

## 用法示例

### 1. 位置速度传播

@[code](/examples/propagator/j4_analytical/j4_position_velocity_prop.cpp)

### 2. 轨道根数传播

@[code](/examples/propagator/j4_analytical/j4_orbital_elements_prop.cpp)

### 3. 类接口使用

@[code](/examples/propagator/j4_analytical/j4_class_interface.cpp)

## 精度与限制

### 精度特点

- **计算精度**：考虑J2和J4项长期摄动，精度优于J2传播器
- **适用范围**：适用于需要较高精度地球扁率效应的近地轨道
- **计算效率**：较高，适合需要较高效率的应用

### 使用限制

- 仅考虑J2和J4项摄动，忽略更高阶项和其他摄动力
- 对于高精度轨道预报需求，建议使用HPOP等数值方法
- 长时间传播时，由于模型简化，误差会累积

## 地球J2/J4参数

对于地球轨道计算，推荐使用以下参数：

- **J2系数**：1.08262668e-3
- **J4系数**：-1.620e-6
- **地球半径**：6378137.0米
- **引力常数**：3.986004418e14 m³/s²

## 精度对比

与J2传播器相比，J4传播器在以下方面具有优势：

- **近地点进动**：更准确的近地点旋转速率计算
- **长期预报**：长时间轨道预报精度更高
- **高倾角轨道**：对极轨道和临界倾角轨道的模拟更精确

## 注意事项

- 输入的位置和速度向量应在同一惯性坐标系下
- J2/J4系数和天体半径应与中心天体匹配
- 传播时间间隔不宜过长，避免数值误差累积
- 对于科学任务和精密轨道确定，建议验证模型精度

## API参考

@[code](/src/AstCore/Propagator/J4Analytical.hpp)
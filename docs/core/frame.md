# 坐标系模块

*说明：本文档旨在帮助理解模块功能，具体API和行为请查阅源代码。*

## 模块概述

坐标系(Frame)模块定义了三维空间中的坐标系，是航天动力学和天体力学计算的基础。坐标系由原点(Point)和轴系(Axes)组成，用于描述空间中位置和方向。

## 核心概念

### 坐标系层次结构

坐标系采用树形层次结构组织：
- **FrameRoot**：根坐标系，是所有坐标系的根节点
- **FrameICRF**：国际天球参考系坐标系
- **FrameAssembly**：组装坐标系，由任意轴系和原点组合而成
- **FrameWithEpoch**：带轴系历元的坐标系，支持冻结特定时刻的轴系方向

### 坐标系组成

- **原点(Point)**：定义坐标系的空间位置
- **轴系(Axes)**：定义坐标轴的方向

### 坐标系与参考系

坐标系与参考系(Reference System)概念略有区别：
- 参考系是抽象的数学概念，定义物理量的测量标准
- 坐标系是参考系的具体实现，包含原点位置和轴系方向

## 主要功能

### 1. 坐标系变换

- `aFrameTransform`：计算两个坐标系之间的变换
- 支持返回Transform或KinematicTransform对象

### 2. 坐标系查询

- `getParent()`：获取父坐标系
- `getTransform()`：获取相对父坐标系的变换
- `getTransformTo()`：获取到目标坐标系的变换
- `getTransformFrom()`：获取从源坐标系到当前坐标系的变换
- `getBody()`：获取坐标系中心对应的天体
- `getGM()`：获取引力参数
- `getAxes()`：获取轴系
- `getOrigin()`：获取原点

## 类层次结构

```
Frame (抽象基类)
├── FrameRoot (根坐标系)
├── FrameICRF (国际天球参考系)
├── FrameAssembly (组装坐标系)
└── FrameWithEpoch (带历元坐标系)
```

## 主要坐标系类型

### 1. 惯性坐标系

- **ICRF (International Celestial Reference Frame)**：国际天球参考系，是目前最精确的惯性参考系
- 适用于描述卫星相对天体的惯性运动

### 2. 固连坐标系

- **CBF (Central Body Fixed Frame)**：中心天体固连系，随天体自转
- 适用于描述卫星相对天体表面的位置

### 3. 中间参考系

- **CIRF (Celestial Intermediate Reference Frame)**：天球中间参考系
- **TIRF (Terrestrial Intermediate Reference Frame)**：地球中间参考系
- 用于高精度坐标变换的中间步骤

## 依赖关系

- `AstGlobal.h`：项目全局定义
- `AstCore/Object.hpp`：对象基类
- `AstCore/Axes.hpp`：轴系定义
- `AstCore/Point.hpp`：原点定义
- `AstCore/Transform.hpp`：变换定义
- `AstCore/KinematicTransform.hpp`：运动学变换定义

## 注意事项

- 坐标系变换涉及旋转和平移，完整的变换由Transform或KinematicTransform表示
- 长时间跨度的坐标变换需考虑岁差、章动等天文现象的影响
- 地球固连系需要EOP(地球定向参数)数据支持极移校正
- FrameWithEpoch适用于需要固定轴系方向的分析场景

## API参考

@[code](/src/AstCore/Geometry/Euclid/Frame.hpp)
@[code](/src/AstCore/Coordinate/Frame/FrameRoot.hpp)
@[code](/src/AstCore/Coordinate/Frame/FrameICRF.hpp)
@[code](/src/AstCore/Coordinate/Frame/FrameAssembly.hpp)

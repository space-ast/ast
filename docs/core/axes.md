# 轴系模块

*说明：本文档旨在帮助理解模块功能，具体API和行为请查阅源代码。*

## 模块概述

轴系(Axes)模块定义了三维空间中的坐标轴系统，是坐标系的重要组成部分。轴系描述了坐标轴的方向（X、Y、Z三轴的指向），与原点(Point)组合后构成完整的坐标系(Frame)。

## 核心概念

### 轴系层次结构

轴系采用树形层次结构组织：
- **AxesRoot**：根轴系，是所有轴系的根节点，相对自身无旋转
- **AxesBodyRelated**：天体相关轴系的基类
  - **AxesBodyInertial**：天体惯性轴系，相对惯性参考系静止
  - **AxesBodyFixed**：天体固连轴系，随天体一同旋转
  - **AxesBodyTOD**：真赤道系轴系
  - **AxesBodyMOD**：平赤道系轴系
- **AxesFrozen**：冻结轴系，固定在某一时刻的方向
- **AxesFrozenAtEventTime**：事件时刻冻结轴系

### 轴系与坐标系的关系

- 轴系定义了坐标轴的方向
- 原点(Point)定义了坐标系的原点位置
- 轴系与原点的组合构成完整的坐标系(Frame)

## 主要功能

### 1. 轴系变换

- `aAxesTransform`：计算两个轴系之间的旋转变换
- 支持返回Rotation、KinematicRotation或转换矩阵

### 2. 轴系查询

- `getParent()`：获取父轴系
- `getTransform()`：获取相对父轴系的旋转变换
- `getTransformTo()`：获取到目标轴系的旋转变换
- `getTransformFrom()`：获取从源轴系到当前轴系的旋转变换
- `getDepth()`：获取轴系深度
- `getAncestor()`：获取指定深度的祖先轴系

## 类层次结构

```
Axes (抽象基类)
├── AxesRoot (根轴系)
├── AxesBodyRelated (天体相关轴系基类)
│   ├── AxesBodyInertial (天体惯性轴系)
│   ├── AxesBodyFixed (天体固连轴系)
│   ├── AxesBodyTOD (真赤道系轴系)
│   └── AxesBodyMOD (平赤道系轴系)
├── AxesFrozen (冻结轴系)
└── AxesFrozenAtEventTime (事件时刻冻结轴系)
```

## 依赖关系

- `AstGlobal.h`：项目全局定义
- `AstCore/Object.hpp`：对象基类
- `AstCore/Rotation.hpp`：旋转定义
- `AstCore/KinematicRotation.hpp`：运动学旋转定义

## 注意事项

- 轴系本身不包含原点信息，需与Point组合才能构成完整坐标系
- 轴系变换遵循树形遍历逻辑，从当前轴系向上遍历到共同祖先
- 冻结轴系在创建后固定在特定时刻的方向，适用于事件分析等场景

## API参考

@[code](/src/AstCore/Geometry/Euclid/Axes.hpp)
@[code](/src/AstCore/Coordinate/Axes/AxesRoot.hpp)

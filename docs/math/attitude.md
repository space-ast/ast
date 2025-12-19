# 姿态

*说明：本文档旨在帮助理解模块功能，具体API和行为请查阅源代码。*

## 概述

Attitude 模块提供了完整的姿态表示和转换功能，支持四元数、欧拉角和旋转矩阵三种主要的姿态表示方法，并提供了它们之间的相互转换。

## 核心概念

### 姿态表示方法

#### 1. 四元数 (Quaternion)
- 无奇异的姿态表示方法
- 适合插值和连续旋转
- 内存占用小（4个double）

#### 2. 欧拉角 (Euler)
- 直观的姿态表示（滚转、俯仰、偏航）
- 支持12种旋转顺序
- 可能存在万向节锁问题

#### 3. 旋转矩阵 (Matrix3d)
- 3×3正交矩阵
- 适合向量变换
- 无奇异问题

### 旋转顺序

模块支持12种欧拉角旋转顺序：

| 类型 | 旋转顺序 | 枚举值 |
|------|----------|--------|
| 泰特-布莱恩角 | X→Y→Z | `eXYZ = 123` |
| 泰特-布莱恩角 | X→Z→Y | `eXZY = 132` |
| 泰特-布莱恩角 | Y→X→Z | `eYXZ = 213` |
| 泰特-布莱恩角 | Y→Z→X | `eYZX = 231` |
| 泰特-布莱恩角 | Z→X→Y | `eZXY = 312` |
| 泰特-布莱恩角 | Z→Y→X | `eZYX = 321` |
| 经典欧拉角 | X→Y→X | `eXYX = 121` |
| 经典欧拉角 | X→Z→X | `eXZX = 131` |
| 经典欧拉角 | Y→X→Y | `eYXY = 212` |
| 经典欧拉角 | Y→Z→Y | `eYZY = 232` |
| 经典欧拉角 | Z→X→Z | `eZXZ = 313` |
| 经典欧拉角 | Z→Y→Z | `eZYZ = 323` |

**注意**：泰特-布莱恩角使用三个不同的轴，经典欧拉角第一个和第三个轴相同。

## 使用示例

### 示例1：基本转换

@[code](/examples/math/attitude/basic_convert.cpp)

### 示例2：欧拉角使用

@[code](/examples/math/attitude/euler.cpp)

### 示例3：四元数操作

@[code](/examples/math/attitude/quaternion.cpp)

### 示例4：完整转换流程

@[code](/examples/math/attitude/example_convert.cpp)


## 角度范围说明

根据旋转顺序类型，欧拉角的有效范围不同：

- **泰特-布莱恩角** (ABC顺序)：
  - `angle2` 在区间 `[-π/2, π/2]`
  - `angle1`, `angle3` 在区间 `[-π, π]`

- **经典欧拉角** (ABA顺序)：
  - `angle2` 在区间 `[0, π]` 
  - `angle1`, `angle3` 在区间 `[-π, π]`

## 错误处理

转换函数返回 `err_t` 类型表示操作状态，调用者应检查返回值以确保转换成功。

## 依赖关系

- `AstGlobal.h`: 项目全局定义
- `MathOperator.hpp`: 数学运算功能
- `Matrix3d`, `Vector3d`: 矩阵和向量类型

## API参考

@[code](/src/AstMath/Attitude/AttitudeConvert.hpp)

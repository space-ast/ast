# 数学运算

*说明：本文档旨在帮助理解模块功能，具体API和行为请查阅源代码。*

## 概述

`MathOperator.hpp` 是 ast 项目中的数学运算工具模块，提供了一系列通用的向量和矩阵运算函数。

该模块采用模板化设计，支持多种数据容器类型，包括标准库容器、原生数组和指针等。

## 功能特性

### 支持的运算类型

- **基本数学函数**: `sign`, `cot`, `acot`, `eps`
- **向量运算**: 
  - 点积 (`dot`)
  - 叉积 (`cross`) 
  - 范数 (`norm`)
  - 归一化 (`normalize`, `normalized`)
- **算术运算符**: 支持向量与标量的加减乘除运算
- **矩阵乘法**: 支持固定大小矩阵的乘法运算

### 多容器支持

模块设计为支持多种数据容器类型：

- `std::vector`, `std::array` 等标准库容器
- 原生 C 数组 `double[N]`
- 原生指针 `double*`
- 任何支持 `size()` 和下标访问 `[]` 的自定义容器


## 使用示例

### 基本向量运算

@[code](/examples/util/operator/vector_op_basic.cpp)

### 算术运算

@[code](/examples/util/operator/vector_op_arithmetic.cpp)

### 矩阵运算

@[code](/examples/util/operator/matrix_op_basic.cpp)

## 设计说明

### 模板化设计

模块采用模板函数实现，只要类型支持标准化的接口（`size()` 和下标访问 `[]`）即可使用这些函数。这种设计提供了极大的灵活性，同时保持类型安全。

### 性能考虑

- 对于已知维度的运算（如 `dot3`, `cross3`），提供专用函数以避免循环开销
- 支持原地操作 (`normalize`) 和返回副本的操作 (`normalized`)
- 编译时尺寸检查用于原生数组版本

### 错误处理

- 使用 `assert` 检查向量尺寸一致性
- 使用 `static_assert` 在编译时检查数组尺寸
- 对零向量归一化进行安全处理

## 依赖关系

- `AstGlobal.h`: 项目全局定义
- `math.h`: 数学函数
- `assert.h`: 断言检查
- `type_traits`: 类型特征
- `array`: 标准数组容器


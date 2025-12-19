# 单位量纲系统

单位量纲系统是ast库中的一个核心工具模块，用于处理物理量的单位和量纲。该系统支持带单位的数值运算、自动单位转换以及量纲一致性检查，确保物理计算的正确性。

## 核心概念

### 量纲（Dimension）

量纲是物理量的基本属性，表示物理量的类型。例如，长度、质量、时间等都是基本量纲，而速度、加速度等是导出量纲。

::: warning
目前支持的导出量纲，其形式必须是基本量纲幂的乘积，其中指数必须为整数，且取值范围在-8到7之间
:::

#### 基本量纲

ast库定义了以下基本量纲：
- 质量（Mass）
- 长度（Length）
- 角度（Angle）
- 时间（Time）
- 温度（Temperature）
- 电流（Current）
- 物质量（Amount）
- 发光强度（Luminous）

#### 量纲运算

量纲支持乘法、除法和幂运算，用于创建导出量纲：

@[code](/examples/util/quantity/dimension_op.cpp)

### 单位（Unit）

单位是量纲的具体表示，用于度量物理量的大小。每个单位都有一个对应的量纲和缩放因子。

#### 预定义单位

ast库提供了丰富的预定义单位，包括：

- 长度：mm, cm, m, km, in, ft, yd, mi
- 时间：s, min, h, day
- 质量：kg, g, mg, lb
- 角度：rad, deg
- 力：N
- 电流：A
- 面积：m²
- 体积：m³, L

#### 单位运算

单位可以进行乘法、除法和幂运算，创建组合单位：

@[code](/examples/util/quantity/unit_op.cpp)

### 数量（Quantity）

数量是带单位的数值，结合了数值和单位信息，支持带单位的数值运算。

#### 创建数量

@[code](/examples/util/quantity/quantity_create.cpp)

#### 数量运算

数量支持加法、减法、乘法和除法，但要求量纲兼容：

@[code](/examples/util/quantity/quantity_op.cpp)


## 量纲一致性检查

系统会自动检查运算中的量纲一致性，防止物理上不合理的运算：

@[code](/examples/util/quantity/quantity_op_invalid.cpp)


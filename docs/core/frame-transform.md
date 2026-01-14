# 坐标转换模块

*说明：本文档旨在帮助理解模块功能，具体API和行为请查阅源代码。*

## 模块概述

坐标变换模块提供了天体力学和航天动力学中常用的各种坐标系之间的转换功能。该模块支持多种参考系之间的精确转换，包括惯性系、固连系以及各种中间参考系，为轨道计算、姿态控制等应用提供基础支持。

## 核心概念

### 坐标系定义

模块支持以下主要坐标系：

- **CBI (Central Body Inertial Frame)**：中心天体惯性系
- **CBF (Central Body Fixed Frame)**：中心天体固连系
- **ECI (Earth Centered Inertial Frame)**：地球惯性系，根据配置可表示ICRF或J2000
- **J2000**：地球时(TT)2000年1月1日12时0分0秒的地球平赤道系
- **MOD (Mean of Date)**：平赤道系，考虑岁差
- **TOD (True of Date)**：真赤道系，考虑岁差和章动
- **GTOD (Greenwich True of Date)**：格林尼治真赤道系，XY平面为赤道平面，正X轴指向本初子午线，随地球旋转
  - 又称TDR、GCR、PEF或EFG
- **ECF (Earth Centered Fixed Frame)**：地球固连系，在GTOD基础上考虑了极移
- **ICRF (International Celestial Reference Frame)**：国际天球参考系
- **CIRF (Celestial Intermediate Reference Frame)**：天球中间参考系
  - 第一极（z轴方向）为给定时刻的天球中间极（CIP）
  - 基本平面为与该时刻的天球中间极垂直的中间赤道
  - 经度坐标起量点为天球中间零点
- **TIRF (Terrestrial Intermediate Reference Frame)**：地球中间参考系
  - 与国际地球参考系（ITRF）的联系由极移和地球中间零点的定位角确定
  - 通过绕天球中间极CIP的转动角度（地球自转角ERA）与CIRF相联系

### 坐标变换类型

模块提供三种主要的坐标变换接口：

1. **Transform**：返回Rotation或KinematicRotation对象
2. **Matrix**：返回坐标转换矩阵
3. **直接转换**：直接将坐标从一个参考系转换到另一个参考系

部分变换还支持带速度的转换，考虑旋转坐标系的角速度影响。

## 主要功能

模块支持以下主要坐标系之间的转换：

- **ECI ↔ ECF**：地球惯性系与地球固连系之间的转换
- **J2000 ↔ ECF**：J2000地球平赤道系与地球固连系之间的转换
- **J2000 ↔ MOD**：J2000地球平赤道系与平赤道系之间的转换
- **MOD ↔ TOD**：平赤道系与真赤道系之间的转换
- **TOD ↔ GTOD**：真赤道系与格林尼治真赤道系之间的转换
- **GTOD ↔ ECF**：格林尼治真赤道系与地球固连系之间的转换
- **ICRF ↔ ECF**：国际天球参考系与地球固连系之间的转换
- **ICRF ↔ CIRF**：国际天球参考系与天球中间参考系之间的转换
- **CIRF ↔ TIRF**：天球中间参考系与地球中间参考系之间的转换
- **TIRF ↔ ECF**：地球中间参考系与地球固连系之间的转换

## 用法示例

### 1. J2000到ECF的坐标转换

@[code](/examples/core/frame_transform/j2000_to_ecf.cpp)

### 2. 带速度的J2000到ECF转换

@[code](/examples/core/frame_transform/j2000_to_ecf_with_velocity.cpp)

### 3. 使用转换矩阵

@[code](/examples/core/frame_transform/using_transform_matrix.cpp)

## 依赖关系

- `AstGlobal.h`：项目全局定义
- `AstCore/TimePoint.hpp`：时间点定义
- `AstMath/Vector.hpp`：向量定义
- `AstMath/Matrix.hpp`：矩阵定义
- `AstMath/Rotation.hpp`：旋转定义
- `AstCore/SOFA.hpp`：天文算法库接口
- `AstCore/EOP.hpp`：地球定向参数接口

## 注意事项

- 所有角度参数默认使用弧度(rad)
- 转换精度取决于所使用的岁差、章动模型
- 极移校正需要EOP数据支持
- 部分转换函数提供了IAU1980和JPL DE两种实现，可通过`aNutationMethodSet`选择
- 长时间跨度的转换可能需要更精确的模型选择

## API参考

@[code](/src/AstCore/Coordinate/FrameTransform.hpp)
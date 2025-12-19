# AstCore 动力学内核

AstCore是ast项目的动力学核心模块，提供航天动力学相关的核心功能，包括时间系统、轨道表示、轨道传播、天体参考系等。

## 模块概述

AstCore模块主要提供以下功能：
- 时间系统（儒略日、简约儒略日、日期时间转换）
- 角度系统（度、分、秒转换）
- 轨道表示（直角坐标、经典轨道根数、位置速度、轨道元素）
- 轨道传播（二体轨道传播）
- 天体参考系（地心惯性系、地心地固系、轨道坐标系）
- 天体定义（地球、月球、行星等）
- 坐标系转换

## 核心功能

### 时间系统

提供各种时间系统的定义和转换功能


### 角度系统

提供角度相关的转换功能

### 轨道表示

提供多种轨道表示方式

### 轨道预报

提供轨道预报功能


### 天体参考系

提供天体参考系的定义和转换功能


## 目录结构

```
AstCore/
├── Alias/              # 别名和类型定义
├── CAPI/               # C API接口
├── CoordSystem/        # 坐标系定义和转换
│   ├── CoordSystem.hpp # 坐标系基类
│   └── Transform.hpp   # 坐标系转换
├── Date/               # 时间系统
│   ├── Date.hpp        # 日期基类
│   └── JulianDate.hpp  # 儒略日实现
├── Geometry/           # 几何相关
│   ├── Cartesian.hpp   # 直角坐标
│   ├── Orbital.hpp     # 轨道相关
│   └── Spherical.hpp   # 球坐标
├── Orbit/              # 轨道相关
│   ├── TwoBody.hpp     # 二体轨道
│   ├── OrbElement.hpp  # 轨道元素
│   └── Propagation.hpp # 轨道传播
├── README.md           # 本文档
├── Time/               # 时间相关
│   └── Time.hpp        # 时间定义
└── xmake.lua           # XMake构建脚本
```
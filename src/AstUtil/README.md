# AstUtil 实用函数库

AstUtil是ast项目中的工具函数库，提供与航天动力学无关的通用功能，可以用于其他工程。

## 模块概述

AstUtil模块主要提供以下功能：
- 常量定义
- 对象系统（运行时类型信息、属性访问）
- 智能指针（ScopedPtr、SharedPtr、WeakPtr）
- 单位和量纲系统
- 文件系统操作
- 字符串处理
- 反射机制
- 其他实用工具函数

## 核心功能

### 常量定义

提供各种常用的数学和物理常量，定义在`Constants.h`中：

### 对象系统

`Object`类是所有可反射对象的基类，提供：
- 运行时类型信息
- 引用计数（支持强引用和弱引用）
- 属性访问接口


### 单位和量纲系统

提供完整的单位和量纲管理系统

- **Dimension**：表示物理量的量纲
- **Unit**：表示具体的单位
- **Quantity**：表示带单位的物理量
- **UnitManager**：管理预定义单位

### 文件系统操作

提供跨平台的文件系统操作接口


### 字符串处理

提供增强的字符串处理功能


## 目录结构

```
AstUtil/
├── Object/           # 对象系统和反射机制
│   ├── Object.hpp    # 对象基类
│   ├── Class.hpp     # 类元信息
│   ├── Property.hpp  # 属性元信息
│   └── Reflect/      # 反射实现
├── OpSystem/         # 操作系统相关功能
│   └── LibraryLoader.hpp  # 动态库加载
├── Quantity/         # 单位和量纲系统
│   ├── Dimension.hpp # 量纲定义
│   ├── Unit.hpp      # 单位定义
│   ├── Quantity.hpp  # 带单位的物理量
│   └── UnitManager.hpp # 单位管理器
├── StdLib/           # 标准库增强
│   ├── FileSystem.hpp # 文件系统操作
│   ├── String.hpp    # 字符串处理
│   └── Math.hpp      # 数学函数
├── Util/             # 通用工具
│   └── Constants.h   # 常量定义
└── README.md         # 本文档
```

# 时间模块

*说明：本文档旨在帮助理解模块功能，具体API和行为请查阅源代码。*

## 模块概述

Time模块提供了全面的时间处理功能，包括日期、时间、日期时间、儒略日、简约儒略日、时长以及时间系统转换等功能。该模块是天体力学和航天动力学计算的基础组件，支持各种时间表示形式之间的转换和操作。

## 核心概念

### 时间表示类型

模块定义了以下主要时间表示类型：

1. **Date (日期)**
   - 表示年、月、日信息
   - 支持闰年判断、月份天数计算、年中日计算等功能

2. **Time (时间)**
   - 表示时、分、秒信息
   - 支持时间规范化、总秒数计算等功能

3. **DateTime (日期时间)**
   - 组合日期和时间信息
   - 支持本地时间与UTC时间转换、日期时间规范化等功能

4. **JulianDate (儒略日)**
   - 用于天文学的时间表示方法
   - 支持日期时间与儒略日之间的转换

5. **ModJulianDate (简约儒略日)**
   - 儒略日的简化形式
   - 基于JulianDate类，提供更简洁的表示方式

6. **时长类型**
   - **ShorDuration**: 短时长，使用double表示秒数
   - **LongDuration**: 长时长，使用整数秒和小数秒分离表示
   - **DaySecDuration**: 整数天 + 小数秒时长，适合长时间跨度表示

### 时间系统

模块支持以下时间系统之间的转换：

- **TAI (原子时)**: 国际原子时
- **TT (地球时)**: 地球动力学时，用于天体力学计算
- **GPS (GPS时)**: 全球定位系统时间
- **UTC (协调世界时)**: 世界协调时间

## 用法示例

### 1. 日期时间创建与规范化

@[code](/examples/core/time/date_time_normalize.cpp)

### 2. 儒略日转换

@[code](/examples/core/time/julian_date_convert.cpp)

### 3. 时间系统转换

@[code](/examples/core/time/time_system_convert.cpp)


## 依赖关系

- `AstGlobal.h`: 项目全局定义
- `AstUtil/StringView.hpp`: 字符串处理支持
- `AstCore/Constants.h`: 常数定义

## 注意事项

- 所有时间参数默认使用秒(s)
- 角度参数默认使用弧度(rad)
- 部分转换函数考虑了闰秒，部分则不考虑，请查看函数文档
- 使用单个double表示的时间精度有限，长时间跨度建议使用DaySecDuration或LongDuration
- 日期时间规范化函数有多个版本（本地时间、UTC时间、北京时间），请根据需要选择
- 儒略日和简约儒略日之间的转换有精确和不精确两种版本

## API参考

@[code](/src/AstCore/Time/Date.hpp)
@[code](/src/AstCore/Time/Time.hpp)
@[code](/src/AstCore/Time/DateTime.hpp)
@[code](/src/AstCore/Time/JulianDate.hpp)
@[code](/src/AstCore/Time/ModJulianDate.hpp)
@[code](/src/AstCore/Time/Duration.hpp)
@[code](/src/AstCore/Time/TimeSystem.hpp)
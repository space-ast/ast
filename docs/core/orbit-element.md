# OrbitElement 轨道根数定义及转换

*说明：本文档旨在帮助理解模块功能，具体API和行为请查阅源代码。*

## 模块概述

OrbitElement模块提供各种轨道根数（直角坐标、经典轨道根数、修正轨道根数、春分点根数、改进春分点轨道根数）之间的转换函数，是天体力学和航天动力学计算的基础组件。

## 核心概念

### 轨道根数类型

模块定义了以下主要轨道根数类型：

1. **直角坐标系 (CartState)**
   - 包含位置矢量和速度矢量
   - 直观表示空间位置和运动状态

2. **经典轨道根数 (OrbElem)**
   - 长半轴 (a)
   - 偏心率 (e)
   - 轨道倾角 (i)
   - 升交点赤经 (raan)
   - 近拱点角 (argper)
   - 真近点角 (trueA)

3. **修正轨道根数 (ModOrbElem)**
   - 近拱点半径 (rp)
   - 偏心率 (e)
   - 轨道倾角 (i)
   - 升交点赤经 (raan)
   - 近拱点角 (argper)
   - 真近点角 (trueA)

4. **春分点根数 (EquinElem)**
   - 长半轴 (a)
   - h = e*sin(argper + raan)
   - k = e*cos(argper + raan)
   - p = tan(i/2)*sin(raan)
   - q = tan(i/2)*cos(raan)
   - lambda = 平经度 = M + raan + argper

5. **改进春分点轨道根数 (ModEquinElem)**
   - 半通径 (p = a(1-e^2))
   - f = e*cos(argper+RAAN)
   - g = e*sin(argper+RAAN)
   - h = tan(i/2)cos(RAAN)
   - k = tan(i/2)sin(RAAN)
   - L = RAAN + argper + trueA

### 关键常数和单位

- 位置单位：米 (m)
- 速度单位：米/秒 (m/s)
- 角度单位：弧度 (rad)
- 引力参数单位：立方米/秒平方 (m³/s²)

## 用法示例

### 1. 经典轨道根数与直角坐标转换

```cpp
#include "AstCore/Orbit/OrbitElement.hpp"
#include <iostream>

using namespace AST_NAMESPACE;

int main() {
    // 地球引力参数 (m^3/s^2)
    double gmEarth = 3.986004418e14;
    
    // 定义经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
    double coe[6] = {7000000.0, 0.01, 0.0, 0.0, 0.0, 0.0};
    
    // 转换为直角坐标
    double pos[3], vel[3];
    err_t err = coe2rv(coe, gmEarth, pos, vel);
    
    if (err == eNoError) {
        std::cout << "位置向量: [" << pos[0] << ", " << pos[1] << ", " << pos[2] << "]\n";
        std::cout << "速度向量: [" << vel[0] << ", " << vel[1] << ", " << vel[2] << "]\n";
        
        // 从直角坐标转回经典轨道根数
        double coeBack[6];
        err = rv2coe(pos, vel, gmEarth, coeBack);
        
        if (err == eNoError) {
            std::cout << "长半轴: " << coeBack[0] << " m\n";
            std::cout << "偏心率: " << coeBack[1] << std::endl;
        }
    }
    
    return 0;
}
```

### 2. 使用类引用版本的函数

```cpp
#include "AstCore/Orbit/OrbitElement.hpp"
#include <iostream>

using namespace AST_NAMESPACE;

int main() {
    // 地球引力参数 (m^3/s^2)
    double gmEarth = 3.986004418e14;
    
    // 定义位置和速度向量
    Vector3d pos(6778137.0, 0.0, 0.0);  // 近地轨道位置
    Vector3d vel(0.0, 7726.0, 0.0);      // 近地轨道速度
    
    // 转换为经典轨道根数
    OrbElem elem;
    err_t err = aCartToOrbElem(pos, vel, gmEarth, elem);
    
    if (err == eNoError) {
        std::cout << "长半轴: " << elem.a() << " m\n";
        std::cout << "偏心率: " << elem.e() << "\n";
        std::cout << "轨道倾角: " << elem.i() << " rad\n";
        
        // 从经典轨道根数转回直角坐标
        Vector3d posBack, velBack;
        err = aOrbElemToCart(elem, gmEarth, posBack, velBack);
        
        if (err == eNoError) {
            std::cout << "位置向量: [" << posBack[0] << ", " << posBack[1] << ", " << posBack[2] << "]\n";
        }
    }
    
    return 0;
}
```

### 3. 不同轨道根数类型之间的转换

```cpp
#include "AstCore/Orbit/OrbitElement.hpp"
#include <iostream>

using namespace AST_NAMESPACE;

int main() {
    // 定义经典轨道根数
    OrbElem coe;
    coe.a() = 7000000.0;  // 7000 km
    coe.e() = 0.1;
    coe.i() = 0.0;        // 0 radians
    coe.raan() = 0.0;
    coe.argper() = 0.0;
    coe.trueA() = 0.0;
    
    // 转换为春分点根数
    EquinElem ee;
    coe2ee(coe.data(), ee.data());
    
    // 转换为修正轨道根数
    ModOrbElem moe;
    coe2moe(coe.data(), moe.data());
    
    // 转换为改进春分点轨道根数
    ModEquinElem mee;
    coe2mee(coe.data(), mee.data());
    
    std::cout << "转换完成\n";
    std::cout << "春分点根数长半轴: " << ee.a() << " m\n";
    std::cout << "修正轨道根数近拱点半径: " << moe.rp() << " m\n";
    
    return 0;
}
```

## API 参考

### 轨道根数类定义

#### CartState 直角坐标

```cpp
class CartState {
public:
    Vector3d pos_;   ///< 位置
    Vector3d vel_;   ///< 速度
public:
    A_DEF_POD_ITERABLE(double)
    const Vector3d& pos() const {return pos_;} 
    Vector3d& pos() {return pos_;}
    const Vector3d& vel() const {return vel_;} 
    Vector3d& vel() {return vel_;}
};
```

#### OrbElem 经典轨道根数

```cpp
class OrbElem {
public:
    double	 a_;          ///< 长半轴 
    double	 e_;          ///< 偏心率
    double	 i_;          ///< 轨道倾角 
    double	 raan_;       ///< 升交点赤经 
    double	 argper_;     ///< 近拱点角
    double	 trueA_;      ///< 真近点角
public:
    A_DEF_POD_ITERABLE(double)
    AST_DEF_ACCESS_METHOD(double, a)
    AST_DEF_ACCESS_METHOD(double, e)
    AST_DEF_ACCESS_METHOD(double, i)
    AST_DEF_ACCESS_METHOD(double, raan)
    AST_DEF_ACCESS_METHOD(double, argper)
    AST_DEF_ACCESS_METHOD(double, trueA)
};
```

#### ModOrbElem 修正轨道根数

```cpp
class ModOrbElem {
public:
    double rp_;         ///<近拱点半径 
    double e_;          ///<偏心率 
    double i_;          ///<轨道倾角
    double raan_;       ///<升交点赤经
    double argper_;     ///<近拱点角
    double trueA_;      ///<真近点角
public:
    A_DEF_POD_ITERABLE(double)
    AST_DEF_ACCESS_METHOD(double, rp)
    AST_DEF_ACCESS_METHOD(double, e)
    AST_DEF_ACCESS_METHOD(double, i)
    AST_DEF_ACCESS_METHOD(double, raan)
    AST_DEF_ACCESS_METHOD(double, argper)
    AST_DEF_ACCESS_METHOD(double, trueA)
};
```

#### EquinElem 春分点根数

```cpp
class EquinElem {
public:
    double a_;          ///< semimajor axis length
    double h_;          ///< e*sin(argper + raan)   omegabar=argper + raan
    double k_;          ///< e*cos(argper + raan)
    double p_;          ///< tan(i/2)*sin(raan)
    double q_;          ///< tan(i/2)*cos(raan)
    double lambda_;     ///< mean longitude = M + raan + argper
public:
    A_DEF_POD_ITERABLE(double)
    AST_DEF_ACCESS_METHOD(double, a)
    AST_DEF_ACCESS_METHOD(double, h)
    AST_DEF_ACCESS_METHOD(double, k)
    AST_DEF_ACCESS_METHOD(double, p)
    AST_DEF_ACCESS_METHOD(double, q)
    AST_DEF_ACCESS_METHOD(double, lambda)
};
```

#### ModEquinElem 改进春分点轨道根数

```cpp
class ModEquinElem {
public:
    double p_;        ///< p = a(1-e^2) 半通径
    double f_;        ///< f = e*cos(argper+RAAN)
    double g_;        ///< g = e*sin(argper+RAAN)
    double h_;        ///< h = tan(i/2)cos(RAAN)
    double k_;        ///< k = tan(i/2)sin(RAAN)
    double L_;        ///< L = RAAN + argper + trueA
public:
    A_DEF_POD_ITERABLE(double)
    AST_DEF_ACCESS_METHOD(double, p)
    AST_DEF_ACCESS_METHOD(double, f)
    AST_DEF_ACCESS_METHOD(double, g)
    AST_DEF_ACCESS_METHOD(double, h)
    AST_DEF_ACCESS_METHOD(double, k)
    AST_DEF_ACCESS_METHOD(double, L)
};
```

### 主要转换函数

#### 经典轨道根数相关转换

| 函数名 | 说明 | 参数 | 返回值 |
|--------|------|------|--------|
| `coe2rv` | 经典轨道根数转换为直角坐标 | coe: 经典轨道根数数组<br>gm: 引力参数 [m³/s²]<br>pos: 输出位置矢量 [m]<br>vel: 输出速度矢量 [m/s] | 错误码，成功返回eNoError |
| `rv2coe` | 直角坐标转换为经典轨道根数 | pos: 位置矢量 [m]<br>vel: 速度矢量 [m/s]<br>gm: 引力参数 [m³/s²]<br>coe: 输出经典轨道根数 | 错误码，成功返回eNoError |
| `coe2moe` | 经典轨道根数转换为修正轨道根数 | coe: 经典轨道根数数组<br>moe: 输出修正轨道根数 | 无返回值 |
| `moe2coe` | 修正轨道根数转换为经典轨道根数 | moe: 修正轨道根数数组<br>coe: 输出经典轨道根数 | 无返回值 |
| `coe2ee` | 经典轨道根数转换为春分点根数 | coe: 经典轨道根数数组<br>ee: 输出春分点根数 | 无返回值 |
| `ee2coe` | 春分点根数转换为经典轨道根数 | ee: 春分点根数数组<br>coe: 输出经典轨道根数 | 无返回值 |
| `coe2mee` | 经典轨道根数转换为改进春分点轨道根数 | coe: 经典轨道根数数组<br>mee: 输出改进春分点轨道根数 | 错误码，成功返回eNoError |

#### 修正轨道根数相关转换

| 函数名 | 说明 | 参数 | 返回值 |
|--------|------|------|--------|
| `moe2rv` | 修正轨道根数转换为直角坐标 | moe: 修正轨道根数数组<br>gm: 引力参数 [m³/s²]<br>pos: 输出位置矢量 [m]<br>vel: 输出速度矢量 [m/s] | 错误码，成功返回eNoError |
| `rv2moe` | 直角坐标转换为修正轨道根数 | pos: 位置矢量 [m]<br>vel: 速度矢量 [m/s]<br>gm: 引力参数 [m³/s²]<br>moe: 输出修正轨道根数 | 错误码，成功返回eNoError |
| `moe2ee` | 修正轨道根数转换为春分点根数 | moe: 修正轨道根数数组<br>ee: 输出春分点根数 | 错误码，成功返回eNoError |
| `ee2moe` | 春分点根数转换为修正轨道根数 | ee: 春分点根数数组<br>moe: 输出修正轨道根数 | 错误码，成功返回eNoError |
| `moe2mee` | 修正轨道根数转换为改进春分点轨道根数 | moe: 修正轨道根数数组<br>mee: 输出改进春分点轨道根数 | 无返回值 |

#### 春分点根数相关转换

| 函数名 | 说明 | 参数 | 返回值 |
|--------|------|------|--------|
| `ee2rv` | 春分点根数转换为直角坐标 | ee: 春分点根数数组<br>gm: 引力参数 [m³/s²]<br>pos: 输出位置矢量 [m]<br>vel: 输出速度矢量 [m/s] | 无返回值 |
| `rv2ee` | 直角坐标转换为春分点根数 | pos: 位置矢量 [m]<br>vel: 速度矢量 [m/s]<br>gm: 引力参数 [m³/s²]<br>ee: 输出春分点根数 | 无返回值 |
| `ee2mee` | 春分点根数转换为改进春分点轨道根数 | ee: 春分点根数数组<br>mee: 输出改进春分点轨道根数 | 无返回值 |
| `mee2ee` | 改进春分点轨道根数转换为春分点根数 | mee: 改进春分点轨道根数数组<br>ee: 输出春分点根数 | 无返回值 |

#### 改进春分点轨道根数相关转换

| 函数名 | 说明 | 参数 | 返回值 |
|--------|------|------|--------|
| `mee2rv` | 改进春分点轨道根数转换为直角坐标 | mee: 改进春分点轨道根数数组<br>gm: 引力参数 [m³/s²]<br>pos: 输出位置矢量 [m]<br>vel: 输出速度矢量 [m/s] | 无返回值 |
| `rv2mee` | 直角坐标转换为改进春分点轨道根数 | pos: 位置矢量 [m]<br>vel: 速度矢量 [m/s]<br>gm: 引力参数 [m³/s²]<br>mee: 输出改进春分点轨道根数 | 错误码，成功返回eNoError |
| `mee2coe` | 改进春分点轨道根数转换为经典轨道根数 | mee: 改进春分点轨道根数数组<br>coe: 输出经典轨道根数 | 错误码，成功返回eNoError |
| `mee2moe` | 改进春分点轨道根数转换为修正轨道根数 | mee: 改进春分点轨道根数数组<br>moe: 输出修正轨道根数 | 无返回值 |

### 类引用版本的转换函数

#### 改进春分点轨道根数转换函数

```cpp
// 改进春分点轨道根数转换为直角坐标
void aModEquinElemToCart(const ModEquinElem& mee, double gm, Vector3d& pos, Vector3d& vel);

// 直角坐标转换为改进春分点轨道根数
err_t aCartToModEquinElem(const Vector3d& pos, const Vector3d& vel, double gm, ModEquinElem& mee);

// 经典轨道根数转换为改进春分点轨道根数
err_t aOrbElemToModEquinElem(const OrbElem& elem, ModEquinElem& mee);

// 改进春分点轨道根数转换为经典轨道根数
err_t aModEquinElemToOrbElem(const ModEquinElem& mee, OrbElem& elem);
```

#### 经典轨道根数转换函数

```cpp
// 直角坐标转换为经典轨道根数
err_t aCartToOrbElem(const Vector3d& pos, const Vector3d& vel, double gm, OrbElem& elem);

// 经典轨道根数转换为直角坐标
err_t aOrbElemToCart(const OrbElem& elem, double gm, Vector3d& pos, Vector3d& vel);
```

#### 修正轨道根数转换函数

```cpp
// 直角坐标转换为修正轨道根数
err_t aCartToModOrbElem(const Vector3d& pos, const Vector3d& vel, double gm, ModOrbElem& modOrb);

// 修正轨道根数转换为直角坐标
void aModOrbElemToCart(const ModOrbElem& modOrb, double gm, Vector3d& pos, Vector3d& vel);

// 春分点根数转换为修正轨道根数
err_t aEquinElemToModOrb(const EquinElem& equinElem, ModOrbElem& modOrb);

// 修正轨道根数转换为春分点根数
err_t aModOrbToEquinElem(const ModOrbElem& modOrb, EquinElem& equinElem);
```

#### 春分点根数转换函数

```cpp
// 直角坐标转换为春分点根数
void aCartToEquinElem(const Vector3d& pos, const Vector3d& vel, double gm, EquinElem& equinElem);

// 春分点根数转换为直角坐标
void aEquinElemToCart(const EquinElem& equinElem, double gm, Vector3d& pos, Vector3d& vel);
```

## 依赖关系

- `AstGlobal.h`: 项目全局定义
- `AstCore/Vector.hpp`: 向量类支持

## 注意事项

- 所有角度参数均使用弧度制
- 距离参数默认使用米(m)
- 时间参数默认使用秒(s)
- 引力参数单位为m³/s²
- 部分转换函数返回错误码，请适当处理错误情况
- 零偏心率轨道（圆轨道）在某些转换中可能需要特殊处理
- 改进春分点轨道根数在轨道倾角接近180度时可能存在奇异情况

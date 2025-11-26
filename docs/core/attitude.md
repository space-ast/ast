# Attitude 姿态功能

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

```cpp
#include "AstCore/AttitudeConvert.hpp"
#include "AstCore/Euler.hpp"
#include "AstCore/Quaternion.hpp"
#include <iostream>

using namespace AST_NAMESPACE;

int main() {
    // 创建四元数 (绕Z轴旋转90度)
    Quaternion quat = {0.707, 0, 0, 0.707};
    
    // 四元数转旋转矩阵
    Matrix3d matrix;
    aQuatToMatrix(quat, matrix);
    
    std::cout << "四元数转矩阵成功" << std::endl;
    
    // 旋转矩阵转欧拉角 (ZYX顺序)
    Euler euler;
    aMatrixToEuler(matrix, Euler::eZYX, euler);
    
    std::cout << "欧拉角: " << euler.angle1() << ", " 
              << euler.angle2() << ", " << euler.angle3() << std::endl;
    
    return 0;
}
```

### 示例2：欧拉角使用

```cpp
#include "AstCore/Euler.hpp"
#include "AstCore/Quaternion.hpp"
#include <iostream>

using namespace AST_NAMESPACE;

int main() {
    {
        // 创建欧拉角 (弧度) - 滚转10°, 俯仰20°, 偏航30°
        Euler euler{0.1745, 0.3491, 0.5236};
        
        // 转换为旋转矩阵 (ZYX顺序)
        Matrix3d rot_mat;
        euler.toMatrix(Euler::eZYX, rot_mat);
        
        std::cout << "欧拉角转矩阵完成" << std::endl;
    }
    
    {
        // 另一个作用域：转换为四元数
        Euler euler{0.1745, 0.3491, 0.5236};
        Quaternion quat;
        euler.toQuat(Euler::eZYX, quat);
        
        std::cout << "四元数: " << quat.qs() << ", " << quat.qx() 
                  << ", " << quat.qy() << ", " << quat.qz() << std::endl;
    }
    
    return 0;
}
```

### 示例3：四元数操作

```cpp
#include "AstCore/Quaternion.hpp"
#include <iostream>

using namespace AST_NAMESPACE;

int main() {
    {
        Quaternion q = {0.5, 0.5, 0.5, 0.5};
        
        // 归一化
        q.normalize();
        
        std::cout << "归一化后范数: " << q.norm() << std::endl;
    }
    
    {
        // 获取归一化副本
        Quaternion q = {1.0, 2.0, 3.0, 4.0};
        Quaternion q_normalized = q.normalized();
        
        std::cout << "归一化副本范数: " << q_normalized.norm() << std::endl;
    }
    
    {
        // 设置为单位四元数
        Quaternion q;
        q.setIdentity();
        
        std::cout << "单位四元数: " << q.qs() << ", " << q.qx() 
                  << ", " << q.qy() << ", " << q.qz() << std::endl;
    }
    
    return 0;
}
```

### 示例4：完整转换流程

```cpp
#include "AstCore/AttitudeConvert.hpp"
#include "AstCore/Euler.hpp"
#include "AstCore/Quaternion.hpp"
#include <iostream>

using namespace AST_NAMESPACE;

int main() {
    // 初始欧拉角 (滚转, 俯仰, 偏航)
    Euler initial_euler{0.3, 0.2, 0.1};
    std::cout << "初始欧拉角: " << initial_euler.angle1() << ", " 
              << initial_euler.angle2() << ", " << initial_euler.angle3() << std::endl;
    
    {
        // 欧拉角 -> 四元数
        Quaternion quat;
        initial_euler.toQuat(Euler::eZYX, quat);
        std::cout << "转换后的四元数: " << quat.qs() << ", " << quat.qx() 
                  << ", " << quat.qy() << ", " << quat.qz() << std::endl;
    }
    
    {
        // 四元数 -> 矩阵
        Quaternion quat;
        initial_euler.toQuat(Euler::eZYX, quat);
        
        Matrix3d matrix;
        aQuatToMatrix(quat, matrix);
        std::cout << "转换到矩阵完成" << std::endl;
    }
    
    {
        // 矩阵 -> 欧拉角 (完整循环)
        Quaternion quat;
        initial_euler.toQuat(Euler::eZYX, quat);
        
        Matrix3d matrix;
        aQuatToMatrix(quat, matrix);
        
        Euler final_euler;
        aMatrixToEuler(matrix, Euler::eZYX, final_euler);
        
        std::cout << "最终欧拉角: " << final_euler.angle1() << ", " 
                  << final_euler.angle2() << ", " << final_euler.angle3() << std::endl;
    }
    
    return 0;
}
```

## API 参考

### 核心转换函数

#### 四元数与矩阵转换
```cpp
void aQuatToMatrix(const Quaternion& quat, Matrix3d& mtx);
void aMatrixToQuat(const Matrix3d& mtx, Quaternion& quat);
```

#### 欧拉角与矩阵转换
```cpp
err_t aMatrixToEuler(const Matrix3d& mtx, int seq, Euler& euler);
err_t aEulerToMatrix(const Euler& euler, int seq, Matrix3d& mtx);
```

### 欧拉角类方法

```cpp
class Euler {
    // 转换为其他表示形式
    void toMatrix(int seqFlag, Matrix3d& mtx) const;
    void toQuat(int seqFlag, Quaternion& quat) const;
    void toAxisAngle(int seqFlag, Vector3d& axis, double& angle);
    
    // 从其他表示形式初始化
    void fromQuat(const Quaternion& quat, int seqFlag);
    void fromAxisAngle(Vector3d& axis, double& angle, int seqFlag);
    void fromMatrix(const Matrix3d& mtx, int seqFlag);
    
    // 访问器
    double angle1() const;
    double angle2() const; 
    double angle3() const;
};
```

### 四元数类方法

```cpp
class Quaternion {
    // 静态构造函数
    static Quaternion Identity();
    
    // 操作
    double norm() const;
    void normalize();
    Quaternion normalized() const;
    void setIdentity();
    
    // 访问器
    double qs() const;  // 标量部分 (w)
    double qx() const;  // 向量部分 i (x)
    double qy() const;  // 向量部分 j (y) 
    double qz() const;  // 向量部分 k (z)
};
```

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

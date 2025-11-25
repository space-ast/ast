---
title: Attitude Conversion
description: Complete attitude representation conversion between Euler angles, quaternions, and rotation matrices
---

# Attitude Conversion Module

## Overview

The Attitude Conversion module provides comprehensive functionality for converting between different attitude representations used in 3D rotations and orientation calculations. This module is essential for applications in robotics, aerospace, computer graphics, and navigation systems.

## Core Concepts

### Supported Representations

1. **Euler Angles**: Three sequential rotation angles around coordinate axes
2. **Quaternions**: Four-dimensional complex numbers for efficient rotation representation
3. **Rotation Matrices**: 3x3 orthogonal matrices representing 3D rotations

### Rotation Sequence Types

The module supports 12 different rotation sequences:

#### Tait-Bryan Angles (ABC sequences - 6 types)
- XYZ, XZY, YXZ, YZX, ZXY, ZYX

#### Classical Euler Angles (ABA sequences - 6 types)  
- XYX, XZX, YXY, YZY, ZXZ, ZYZ

## Usage Examples

### Example 1: Basic Conversion Flow

```cpp
#include "AstCore/AttitudeConvert.hpp"
#include "AstCore/Euler.hpp"
#include "AstCore/Quaternion.hpp"
#include <iostream>

using namespace AST_NAMESPACE;

int main() {
    // Create Euler angles (roll, pitch, yaw) in radians
    Euler euler{0.1, 0.2, 0.3};
    
    // Convert to quaternion
    Quaternion quat;
    euler.toQuat(Euler::eZYX, quat);
    
    // Convert to rotation matrix
    Matrix3d matrix;
    aQuatToMatrix(quat, matrix);
    
    // Convert back to Euler angles (full cycle)
    Euler final_euler;
    aMatrixToEuler(matrix, Euler::eZYX, final_euler);
    
    std::cout << "Original Euler: " << euler.angle1() << ", " 
              << euler.angle2() << ", " << euler.angle3() << std::endl;
    std::cout << "Final Euler: " << final_euler.angle1() << ", " 
              << final_euler.angle2() << ", " << final_euler.angle3() << std::endl;
    
    return 0;
}
```

### Example 2: Euler Angle Usage

```cpp
#include "AstCore/Euler.hpp"
#include <iostream>

using namespace AST_NAMESPACE;

int main() {
    {
        // Create Euler angles (radians) - roll 10°, pitch 20°, yaw 30°
        Euler euler{0.1745, 0.3491, 0.5236};
        
        // Convert to rotation matrix (ZYX sequence)
        Matrix3d rot_mat;
        euler.toMatrix(Euler::eZYX, rot_mat);
        
        std::cout << "Euler to matrix conversion completed" << std::endl;
    }
    
    {
        // Another scope: Convert to quaternion
        Euler euler{0.1745, 0.3491, 0.5236};
        Quaternion quat;
        euler.toQuat(Euler::eZYX, quat);
        
        std::cout << "Quaternion: " << quat.qs() << ", " << quat.qx() 
                  << ", " << quat.qy() << ", " << quat.qz() << std::endl;
    }
    
    return 0;
}
```

### Example 3: Quaternion Operations

```cpp
#include "AstCore/Quaternion.hpp"
#include <iostream>

using namespace AST_NAMESPACE;

int main() {
    {
        Quaternion q = {0.5, 0.5, 0.5, 0.5};
        
        // Normalize
        q.normalize();
        
        std::cout << "Normalized quaternion norm: " << q.norm() << std::endl;
    }
    
    {
        // Get normalized copy
        Quaternion q = {1.0, 2.0, 3.0, 4.0};
        Quaternion q_normalized = q.normalized();
        
        std::cout << "Normalized copy norm: " << q_normalized.norm() << std::endl;
    }
    
    {
        // Set to identity quaternion
        Quaternion q;
        q.setIdentity();
        
        std::cout << "Identity quaternion: " << q.qs() << ", " << q.qx() 
                  << ", " << q.qy() << ", " << q.qz() << std::endl;
    }
    
    return 0;
}
```

### Example 4: Complete Conversion Process

```cpp
#include "AstCore/AttitudeConvert.hpp"
#include "AstCore/Euler.hpp"
#include "AstCore/Quaternion.hpp"
#include <iostream>

using namespace AST_NAMESPACE;

int main() {
    // Initial Euler angles (roll, pitch, yaw)
    Euler initial_euler{0.3, 0.2, 0.1};
    std::cout << "Initial Euler angles: " << initial_euler.angle1() << ", " 
              << initial_euler.angle2() << ", " << initial_euler.angle3() << std::endl;
    
    {
        // Euler angles -> Quaternion
        Quaternion quat;
        initial_euler.toQuat(Euler::eZYX, quat);
        std::cout << "Converted quaternion: " << quat.qs() << ", " << quat.qx() 
                  << ", " << quat.qy() << ", " << quat.qz() << std::endl;
    }
    
    {
        // Quaternion -> Matrix
        Quaternion quat;
        initial_euler.toQuat(Euler::eZYX, quat);
        
        Matrix3d matrix;
        aQuatToMatrix(quat, matrix);
        std::cout << "Conversion to matrix completed" << std::endl;
    }
    
    {
        // Matrix -> Euler angles (full cycle)
        Quaternion quat;
        initial_euler.toQuat(Euler::eZYX, quat);
        
        Matrix3d matrix;
        aQuatToMatrix(quat, matrix);
        
        Euler final_euler;
        aMatrixToEuler(matrix, Euler::eZYX, final_euler);
        
        std::cout << "Final Euler angles: " << final_euler.angle1() << ", " 
                  << final_euler.angle2() << ", " << final_euler.angle3() << std::endl;
    }
    
    return 0;
}
```

## API Reference

### Core Conversion Functions

#### Quaternion and Matrix Conversion
```cpp
void aQuatToMatrix(const Quaternion& quat, Matrix3d& mtx);
void aMatrixToQuat(const Matrix3d& mtx, Quaternion& quat);
```

#### Euler Angles and Matrix Conversion
```cpp
err_t aMatrixToEuler(const Matrix3d& mtx, int seq, Euler& euler);
err_t aEulerToMatrix(const Euler& euler, int seq, Matrix3d& mtx);
```

### Euler Angle Class Methods

```cpp
class Euler {
    // Convert to other representations
    void toMatrix(int seqFlag, Matrix3d& mtx) const;
    void toQuat(int seqFlag, Quaternion& quat) const;
    void toAxisAngle(int seqFlag, Vector3d& axis, double& angle);
    
    // Initialize from other representations
    void fromQuat(const Quaternion& quat, int seqFlag);
    void fromAxisAngle(Vector3d& axis, double& angle, int seqFlag);
    void fromMatrix(const Matrix3d& mtx, int seqFlag);
    
    // Accessors
    double angle1() const;
    double angle2() const; 
    double angle3() const;
};
```

### Quaternion Class Methods

```cpp
class Quaternion {
    // Static constructors
    static Quaternion Identity();
    
    // Operations
    double norm() const;
    void normalize();
    Quaternion normalized() const;
    void setIdentity();
    
    // Accessors
    double qs() const;  // Scalar part (w)
    double qx() const;  // Vector part i (x)
    double qy() const;  // Vector part j (y) 
    double qz() const;  // Vector part k (z)
};
```

## Angle Range Specifications

Based on rotation sequence type, Euler angles have different valid ranges:

- **Tait-Bryan Angles** (ABC sequences):
  - `angle2` in range `[-π/2, π/2]`
  - `angle1`, `angle3` in range `[-π, π]`

- **Classical Euler Angles** (ABA sequences):
  - `angle2` in range `[0, π]` 
  - `angle1`, `angle3` in range `[-π, π]`

## Error Handling

Conversion functions return `err_t` type indicating operation status. Callers should check return values to ensure successful conversion.

## Dependencies

- `AstGlobal.h`: Project global definitions
- `MathOperator.hpp`: Mathematical operations functionality
- `Matrix3d`, `Vector3d`: Matrix and vector types
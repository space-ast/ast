# 积分器

## 概述

ODE（常微分方程）积分器模块提供了多种数值积分方法，用于求解常微分方程初值问题。该模块参考了 [hipparchus-ode 项目](https://hipparchus.org/) 的类结构，实现了多种经典的 Runge-Kutta 方法。

## 核心类结构

### ODEIntegrator 基类

ODE 积分器的抽象基类，定义了积分器的通用接口。

```cpp
class ODEIntegrator {
public:
    virtual ~ODEIntegrator() {};
    
    // 初始化积分器
    virtual err_t initialize(ODE& ode) = 0;
    
    // 积分ODE
    virtual err_t integrate(ODE& ode, double t0, double tf, const double* y0, double* yf) = 0;
    
    // 积分ODE一步
    virtual err_t integrateStep(ODE& ode, double& t, double tf, const double* y0, double* y) = 0;
    
    // 执行一步积分
    virtual err_t singleStep(ODE& ode, double t0, double step, const double* y0, double* yf) = 0;
};
```

### ODEFixedStepIntegrator

定步长积分器基类，适用于步长固定的积分方法。

### ODEVarStepIntegrator

变步长积分器基类，支持自适应步长控制，根据误差估计自动调整步长。

## 积分器实现

### 定步长积分器

#### RK4 - 4阶龙格库塔方法

- **阶数**: 4阶
- **类型**: 定步长
- **精度**: 中等精度，适用于一般精度要求的计算
- **特点**: 经典的4阶Runge-Kutta方法，计算稳定，实现简单

```cpp
RK4 integrator;
err_t result = integrator.initialize(ode);
result = integrator.integrate(ode, t0, tf, y0, yf);
```

#### RK8 - 8阶龙格库塔方法

- **阶数**: 8阶
- **类型**: 定步长
- **精度**: 高精度，适用于高精度要求的计算
- **特点**: 高阶方法，精度高但计算量较大

#### RKV8 - 8阶Verner方法

- **阶数**: 8阶
- **类型**: 定步长
- **精度**: 高精度
- **特点**: Verner提出的8阶方法，具有较好的数值稳定性

### 变步长积分器

#### RKF45 - Runge-Kutta-Fehlberg 4(5)方法

- **阶数**: 4(5)阶（主阶4阶，误差估计阶5阶）
- **类型**: 变步长
- **精度**: 中等精度，带误差控制
- **特点**: 经典的Fehlberg方法，通过比较4阶和5阶结果来估计误差

```cpp
RKF45 integrator;
integrator.setTolerance(1e-8);  // 设置容差
integrator.setMinStep(1e-6);    // 设置最小步长
integrator.setMaxStep(1.0);     // 设置最大步长
```

#### RKF56 - Runge-Kutta-Fehlberg 5(6)方法

- **阶数**: 5(6)阶
- **类型**: 变步长
- **精度**: 较高精度，带误差控制
- **特点**: 5阶主方法，6阶误差估计

#### RKF78 - Runge-Kutta-Fehlberg 7(8)方法

- **阶数**: 7(8)阶
- **类型**: 变步长
- **精度**: 高精度，带误差控制
- **特点**: 高阶方法，适用于高精度要求的计算

#### RKCK - Cash-Karp方法

- **阶数**: 4(5)阶
- **类型**: 变步长
- **精度**: 中等精度，带误差控制
- **特点**: Cash和Karp提出的方法，具有较好的数值特性

## 使用方法

### 基本使用流程

1. **定义ODE方程**: 继承 `ODE` 类并实现 `evaluate` 方法
2. **选择积分器**: 根据精度和性能需求选择合适的积分器
3. **配置参数**: 设置积分器参数（容差、步长等）
4. **执行积分**: 调用积分方法进行求解

### 使用示例

#### 示例1：基本积分操作

演示如何使用不同的ODE积分器求解简单的常微分方程，包括指数衰减方程和简谐振动方程。

@[code](/examples/math/ode/basic_integration.cpp)

#### 示例2：变步长积分器自适应控制

展示变步长积分器的自适应步长控制和误差估计功能，包括不同容差设置的影响。

@[code](/examples/math/ode/adaptive_step.cpp)

#### 示例3：性能比较分析

比较不同积分器在精度和计算效率方面的表现，包括定步长和变步长积分器的对比。

@[code](/examples/math/ode/performance_comparison.cpp)

## 性能比较

| 积分器 | 阶数 | 类型 | 精度 | 计算量 | 适用场景 |
|--------|------|------|------|--------|----------|
| RK4    | 4    | 定步长 | 中等 | 中等 | 一般精度要求，计算量适中 |
| RK8    | 8    | 定步长 | 高   | 大    | 高精度要求，可接受较大计算量 |
| RKV8   | 8    | 定步长 | 高   | 大    | 高精度要求，数值稳定性好 |
| RKF45  | 4(5) | 变步长 | 中等 | 中等 | 一般精度，带误差控制 |
| RKF56  | 5(6) | 变步长 | 较高 | 较大 | 较高精度，带误差控制 |
| RKF78  | 7(8) | 变步长 | 高   | 大    | 高精度要求，带误差控制 |
| RKCK   | 4(5) | 变步长 | 中等 | 中等 | 一般精度，数值特性好 |

## 注意事项

1. **步长选择**: 定步长积分器需要合理选择步长，步长过大会导致精度不足，步长过小会增加计算量
2. **容差设置**: 变步长积分器的容差设置会影响计算精度和效率
3. **内存使用**: 高阶方法需要更多的中间变量存储，内存使用量较大

## 参考文献

1. Fehlberg, E. (1969). Low-order classical Runge-Kutta formulas with stepsize control and their application to some heat transfer problems.
2. Classical Fifth-, Sixth-, Seventh-, and Eighth-Order Runge-Kutta Formulas with Stepsize Control
3. Cash, J. R., & Karp, A. H. (1990). A variable order Runge-Kutta method for initial value problems with rapidly varying right-hand sides.
4. Verner, J. H. (1978). Explicit Runge-Kutta methods with estimates of the local truncation error.
5. Some Explicit Runge-Kutta Methods of High Order
6. Shanks, E. Baylis "Solutions of Differential Equations by Evaluations of Functions"

<Catalog />
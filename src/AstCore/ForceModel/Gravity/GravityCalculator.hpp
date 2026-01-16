///
/// @file      GravityAttraction.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-14
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#pragma once

#include "AstGlobal.h"
#include "GravityField.hpp"
#include "AstMath/Matrix.hpp"
#include "AstMath/Vector.hpp"
#include <cmath>
#include <vector>
#include <memory>

AST_NAMESPACE_BEGIN

class GravityCalculator1;
class GravityCalculator2;
class GravityCalculator3;

//#define _AST_ENABLE_GRAVITY_CALCULATOR_1
using GravityCalculatorDefault = GravityCalculator3;

class AST_CORE_API GravityCalculator
{
public:
    GravityCalculator();
    GravityCalculator(const GravityField &gravityField);
    GravityCalculator(const GravityField &gravityField, int degree, int order);

    /// @brief 获取重力场系数
    /// @return 重力场系数
    const GravityField& getGravityField() const { return gravityField_; }
    GravityField& getGravityField() { return gravityField_; }
    
    /// @brief 获取计算阶数
    /// @return 计算阶数
    int getDegree() const { return degree_; }

    /// @brief 获取计算次数
    /// @return 计算次数
    int getOrder() const { return order_; }

    virtual ~GravityCalculator () = default;

    /// @brief 计算摄动加速度
    /// @param positionCBF 位置向量（天体固连系）
    /// @param accelerationCBF 摄动加速度向量（天体固连系）
    virtual void calcPertAcceleration (const Vector3d &positionCBF, Vector3d &accelerationCBF) = 0;

    /// @brief 计算总加速度（包含摄动项）
    /// @param positionCBF 位置向量（天体固连系）
    /// @param accelerationCBF 总加速度向量（天体固连系）
    virtual void calcTotalAcceleration (const Vector3d &positionCBF, Vector3d &accelerationCBF);
protected:
    int degree_;                    ///< 计算所使用的阶数
    int order_;                     ///< 计算所使用的次数
    GravityField gravityField_;     ///< 重力场系数
};

#ifdef _AST_ENABLE_GRAVITY_CALCULATOR_1

/// @brief 重力加速度计算类1
class AST_CORE_API GravityCalculator1: public GravityCalculator
{
public:
    GravityCalculator1(const GravityField &gravityField, int degree, int order);
    ~GravityCalculator1();

    void calcPertAcceleration (const Vector3d &positionCBF, Vector3d &accelerationCBF) final;

protected:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

#endif


/// @brief 重力加速度计算类2 - Holmes-Featherstone算法
class AST_CORE_API GravityCalculator2: public GravityCalculator
{
public:
    GravityCalculator2(const GravityField &gravityField, int degree, int order);
    
    /// @brief 计算重力加速度
    void calcPertAcceleration (const Vector3d &positionCBF, Vector3d &accelerationCBF) final;

private:
    /// @brief 初始化递归系数
    void initializeRecursionCoefficients();
    
    /// @brief 计算非中心部分的梯度
    void computeGradient(const Vector3d &position, Vector3d &gradient);
    
    /// @brief 计算田谐项
    int computeTesseral(int m, int degree, int index, double t, double u, double tOu);

private:
    /// @brief 递归系数 g_{n,m}/√j
    std::vector<double> gnmOj_;
    
    /// @brief 递归系数 h_{n,m}/√j
    std::vector<double> hnmOj_;
    
    /// @brief 递归系数 e_{n,m}
    std::vector<double> enm_;
    
    /// @brief 缩放扇区项 Pbar_{m,m}/u^m × 2^{-SCALING}
    std::vector<double> sectorial_;
private:
    std::vector<double> pnm0Plus2;
    std::vector<double> pnm0Plus1;
    std::vector<double> pnm0;
    std::vector<double> pnm1;
private:
    std::vector<double> aOrN;
    std::vector<double> cosLambdaN;
    std::vector<double> sinLambdaN;
};


/// @brief 重力加速度计算类3
/// @details Pines算法，来自GMAT的实现
class AST_CORE_API GravityCalculator3: public GravityCalculator
{
public:
    GravityCalculator3(const GravityField &gravityField, int degree, int order);
    ~GravityCalculator3() override;
    
    void calcPertAcceleration (const Vector3d &positionCBF, Vector3d &accelerationCBF) final;
private:
    double        Factor;  // Factor = 1 (magnetic) or -mu (gravity)
    double**      C;       // Normalized harmonic coefficients
    double**      S;       // Normalized harmonic coefficients
    double**      A;       // Normalized 'derived' Assoc. Legendre Poly
    double**      V;       // Normalization factor
    double*       Re;      // powers of projection of pos onto x_ecf (re)
    double*       Im;      // powers of projection of pos onto y_ecf (im)
    double**      N1;      // Temporary
    double**      N2;      // Temporary
    double**      VR01;    // Temporary
    double**      VR11;    // Temporary
    double**      VR02;    // Temporary
    double**      VR12;    // Temporary
    double**      VR22;    // Temporary
};


class AST_CORE_API GravityCalculator4: public GravityCalculator
{
public:
    using GravityCalculator::GravityCalculator;

    void calcPertAcceleration (const Vector3d &positionCBF, Vector3d &accelerationCBF) final;
};


class AST_CORE_API GravityCalculator5: public GravityCalculator
{
public:
    using GravityCalculator::GravityCalculator;

    void calcPertAcceleration (const Vector3d &positionCBF, Vector3d &accelerationCBF) final;
};


AST_NAMESPACE_END
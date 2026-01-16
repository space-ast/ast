/// 
/// @file      GravityCalculator2.cpp
/// @brief     Holmes-Featherstone重力场计算实现
/// @details   基于Holmes和Featherstone算法实现的高精度重力场计算
/// @author    axel
/// @date      2026-01-15
/// @copyright 版权所有 (C) 2026-present, ast项目。
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

#include "GravityCalculator.hpp"
#include "AstUtil/Logger.hpp"
#include <cmath>
#include <vector>
#include <algorithm>

AST_NAMESPACE_BEGIN

// 常量定义
namespace {
    /// @brief 指数缩放避免浮点溢出
    /// @details 论文使用10^280，我们使用2^930保持相同数量级
    constexpr int SCALING = 930;
    
    /// @brief 中心引力缩放因子
    /// @details 使用2的幂避免乘除序列中的数值噪声
    constexpr double MU_SCALE = 4294967296.0; // 2^32
}


// 将球坐标梯度转换为笛卡尔坐标梯度
static void convertSphericalToCartesianGradient(
    const Vector3d& position,
    const Vector3d& sphericalGradient,
    Vector3d& cartesianGradient) 
{
    const double x = position.x();
    const double y = position.y();
    const double z = position.z();
    
    // 计算球坐标
    // const double r = position.norm();
    // const double theta = std::atan2(position.y(), position.x());
    // const double phi = acos(position.z() / r);

    const double rho2 = x * x + y * y;
    const double rho = std::sqrt(rho2);
    const double r2 = rho2 + z * z;
    const double r = std::sqrt(r2);
    
    // 雅可比矩阵
    Matrix3d jacobian;// = Matrix3d::Zero();
    
    // r梯度行
    jacobian(0, 0) = x / r;
    jacobian(0, 1) = y / r;
    jacobian(0, 2) = z / r;
    
    // θ梯度行（经度）
    jacobian(1, 0) = -y / rho2;
    jacobian(1, 1) = x / rho2;
    jacobian(1, 2) = 0; //
    
    // φ梯度行（极角）
    const double rhoTimesR2 = rho * r2;
    jacobian(2, 0) = x * z / rhoTimesR2;
    jacobian(2, 1) = y * z / rhoTimesR2;
    jacobian(2, 2) = -rho / r2;
    
    // 转换为笛卡尔梯度
    cartesianGradient = sphericalGradient * jacobian;
}


// 创建距离幂次数组
static void createDistancePowersArray(double aOr, int maxDegree, std::vector<double>& aOrN) 
{
    // const int maxDegree = getDegree();
    // aOrN.resize(maxDegree + 1, 0.0);
    
    aOrN[0] = 1.0;
    if (maxDegree > 0) {
        aOrN[1] = aOr;
    }
    
    // 填充数组
    for (int n = 2; n < static_cast<int>(aOrN.size()); ++n) {
        const int p = n / 2;
        const int q = n - p;
        aOrN[n] = aOrN[p] * aOrN[q];
    }
    
    // return aOrN;
}


// 创建经度余弦/正弦数组

void createCosSinArrays(double cosLambda, double sinLambda, int maxOrder, std::vector<double>& cosML, std::vector<double>& sinML) 
{
    // cosML.resize(maxOrder + 1, 0.0);
    // sinML.resize(maxOrder + 1, 0.0);
    
    cosML[0] = 1.0;
    sinML[0] = 0.0;
    
    if (maxOrder > 0) {
        cosML[1] = cosLambda;
        sinML[1] = sinLambda;
        
        // 填充数组
        for (int m = 2; m <= maxOrder; ++m) {
            // 将m*λ拆分为p*λ + q*λ，避免p或q远大于另一个
            const int p = m / 2;
            const int q = m - p;
            
            cosML[m] = cosML[p] * cosML[q] - sinML[p] * sinML[q];
            sinML[m] = sinML[p] * cosML[q] + cosML[p] * sinML[q];
        }
    }
}



GravityCalculator2::GravityCalculator2(const GravityField &gravityField, int degreeIn, int orderIn)
    : GravityCalculator(gravityField, degreeIn, orderIn)
{
    getGravityField().normalize();

    int degree = this->getDegree();
    int order = this->getOrder();
    pnm0Plus2.resize(degree + 1, 0.0);
    pnm0Plus1.resize(degree + 1, 0.0);
    pnm0.resize(degree + 1, 0.0);
    pnm1.resize(degree + 1, 0.0);
    // std::vector<double> pnm0Plus2(degree + 1, 0.0);
    // std::vector<double> pnm0Plus1(degree + 1, 0.0);
    // std::vector<double> pnm0(degree + 1, 0.0);
    // std::vector<double> pnm1(degree + 1, 0.0);

    aOrN.resize(degree + 1, 0.0);
    cosLambdaN.resize(order + 1, 0.0);
    sinLambdaN.resize(order + 1, 0.0);

    // 预计算递归系数
    initializeRecursionCoefficients();
}

// 初始化递归系数（对应Java版本的构造函数中的预计算）
void GravityCalculator2::initializeRecursionCoefficients()
{
    const int degree = this->getDegree();
    
    // 计算数组大小：三角阵列但不包含对角线元素(n=m)和n=1,m=0
    const int size = std::max(0, degree * (degree + 1) / 2 - 1);
    
    // 分配内存
    gnmOj_.resize(size);
    hnmOj_.resize(size);
    enm_.resize(size);
    
    // 预计算递归系数（对应论文公式19和22）
    // 按使用顺序存储以提高缓存效率：从右到左列
    int index = 0;
    for (int m = degree; m >= 0; --m) {
        const int j = (m == 0) ? 2 : 1;
        for (int n = std::max(2, m + 1); n <= degree; ++n) {
            const double f = static_cast<double>(n - m) * (n + m + 1);
            gnmOj_[index] = 2.0 * (m + 1.) / std::sqrt(j * f);
            hnmOj_[index] = std::sqrt((n + m + 2) * (n - m - 1.) / (j * f));
            enm_[index] = std::sqrt(f / j);
            ++index;
        }
    }
    
    // 缩放扇区项（对应论文公式28）
    sectorial_.resize(degree + 1);
    sectorial_[0] = std::ldexp(1.0, -SCALING);
    if (degree > 0) {
        sectorial_[1] = std::sqrt(3.0) * sectorial_[0];
    }
    for (int m = 2; m < static_cast<int>(sectorial_.size()); ++m) {
        sectorial_[m] = std::sqrt((2.0 * m + 1.0) / (2.0 * m)) * sectorial_[m - 1];
    }
}

// 计算重力加速度
void GravityCalculator2::calcPertAcceleration(const Vector3d &positionCBF, Vector3d &accelerationCBF)
{
    // 计算非中心部分的梯度
    computeGradient(positionCBF, accelerationCBF);
}


// 计算非中心部分的梯度
void GravityCalculator2::computeGradient(const Vector3d &position, Vector3d &gradient)
{
    const int degree = this->getDegree();
    const int order = this->getOrder();
    double mu = getGravityField().getGM();
    
    // 分配递归列
    // std::vector<double> pnm0Plus2(degree + 1, 0.0);
    // std::vector<double> pnm0Plus1(degree + 1, 0.0);
    // std::vector<double> pnm0(degree + 1, 0.0);
    // std::vector<double> pnm1(degree + 1, 0.0);
    
    // 计算极坐标
    const double x = position.x();
    const double y = position.y();
    const double z = position.z();
    const double x2 = x * x;
    const double y2 = y * y;
    const double z2 = z * z;
    const double rho2 = x2 + y2;
    const double r2 = rho2 + z2;
    const double r = std::sqrt(r2);
    const double rho = std::sqrt(rho2);
    const double t = z / r;    // cos(θ)
    const double u = rho / r;  // sin(θ)
    const double tOu = z / rho;
    
    // 计算距离幂次
    createDistancePowersArray(getGravityField().getRefDistance() / r, degree, aOrN);
    
    // 计算经度余弦/正弦
    createCosSinArrays(x / rho, y / rho, order, cosLambdaN, sinLambdaN);
    
    // 外层求和：按阶数
    int index = 0;
    double value = 0.0;
    gradient = {0,0,0};
    
    for (int m = degree; m >= 0; --m) {
        // 计算带导数的田谐项
        index = computeTesseral(m, degree, index, t, u, tOu);
        
        if (m <= order) {
            // 计算当前阶数对场的贡献（论文公式5）
            
            // 内层求和：按度数
            double sumDegreeS = 0.0;
            double sumDegreeC = 0.0;
            double dSumDegreeSdR = 0.0;
            double dSumDegreeCdR = 0.0;
            double dSumDegreeSdTheta = 0.0;
            double dSumDegreeCdTheta = 0.0;
            
            for (int n = std::max(2, m); n <= degree; ++n) {
                const double qSnm = aOrN[n] * getGravityField().getSnm(n, m);
                const double qCnm = aOrN[n] * getGravityField().getCnm(n, m);
                const double nOr = n / r;
                const double s0 = pnm0[n] * qSnm;
                const double c0 = pnm0[n] * qCnm;
                const double s1 = pnm1[n] * qSnm;
                const double c1 = pnm1[n] * qCnm;
                
                sumDegreeS        += s0;
                sumDegreeC        += c0;
                dSumDegreeSdR     -= nOr * s0;
                dSumDegreeCdR     -= nOr * c0;
                dSumDegreeSdTheta += s1;
                dSumDegreeCdTheta += c1;
            }
            
            // 对阶数外层求和的贡献
            // 注意：梯度顺序遵循数学约定，兼容球坐标类
            // λ对应球坐标的θ（索引1），θ对应球坐标的φ（索引2）
            const double sML = sinLambdaN[m];
            const double cML = cosLambdaN[m];
            
            value            = value       * u + sML * sumDegreeS        + cML * sumDegreeC;
            gradient[0]      = gradient[0] * u + sML * dSumDegreeSdR     + cML * dSumDegreeCdR;
            gradient[1]      = gradient[1] * u + m * (cML * sumDegreeS - sML * sumDegreeC);
            gradient[2]      = gradient[2] * u + sML * dSumDegreeSdTheta + cML * dSumDegreeCdTheta;
        }
        
        // 旋转递归数组
        std::swap(pnm0Plus2, pnm0Plus1);
        std::swap(pnm0Plus1, pnm0);
        // auto tmp  = pnm0Plus2;
        // pnm0Plus2 = pnm0Plus1;
        // pnm0Plus1 = pnm0;
        // pnm0      = tmp;

        // 新的pnm0会被下一次迭代填充，所以这里清空
        // std::fill(pnm0.begin(), pnm0.end(), 0.0);
    }
    
    // 缩放回
    #if 1
    double factor = std::ldexp(1, SCALING);
    value *= factor;
    gradient[0] *= factor;
    gradient[1] *= factor;
    gradient[2] *= factor;
    #else
    value = std::ldexp(value, SCALING);
    gradient[0] = ldexp(gradient[0], SCALING);
    gradient[1] = ldexp(gradient[1], SCALING);
    gradient[2] = ldexp(gradient[2], SCALING);
    #endif
    
    // 应用全局mu/r因子
    const double muOr = mu / r;
    value            *= muOr;
    gradient[0]       = muOr * gradient[0] - value / r;
    gradient[1]      *= muOr;
    gradient[2]      *= muOr;
    
    // 将梯度从球坐标转换为笛卡尔坐标
    // convertSphericalToCartesianGradient(position, gradient, gradient);
    {
        // 雅可比矩阵
        Matrix3d jacobian;// = Matrix3d::Zero();
        
        // r梯度行
        jacobian(0, 0) = x / r;
        jacobian(0, 1) = y / r;
        jacobian(0, 2) = t; //z / r;
        
        // θ梯度行（经度）
        jacobian(1, 0) = -y / rho2;
        jacobian(1, 1) = x / rho2;
        jacobian(1, 2) = 0; //
        
        // φ梯度行（极角）
        const double rhoTimesR2 = rho * r2;
        jacobian(2, 0) = x * z / rhoTimesR2;
        jacobian(2, 1) = y * z / rhoTimesR2;
        jacobian(2, 2) = -rho / r2;
        
        // 转换为笛卡尔梯度
        gradient = gradient * jacobian;
    }
}


// 计算田谐项
int GravityCalculator2::computeTesseral(int m, int degree, int index, double t, double u, double tOu)
{
    const double u2 = u * u;
    
    // 从扇区项初始化递归
    int n = std::max(2, m);
    if (n == m) {
        pnm0[n] = sectorial_[n];
        ++n;
    }
    
    // 计算田谐值
    int localIndex = index;
    while (n <= degree) {
        // 值（论文公式27）
        pnm0[n] = gnmOj_[localIndex] * t * pnm0Plus1[n] - hnmOj_[localIndex] * u2 * pnm0Plus2[n];
        ++localIndex;
        ++n;
    }
    
    {
        // 从扇区项初始化递归
        n = std::max(2, m);
        if (n == m) {
            (pnm1)[n] = m * tOu * pnm0[n];
            ++n;
        }
        
        // 计算田谐值及对极角的导数
        localIndex = index;
        while (n <= degree) {
            // 一阶导数（论文公式30）
            (pnm1)[n] = m * tOu * pnm0[n] - enm_[localIndex] * u * pnm0Plus1[n];
            ++localIndex;
            ++n;
        }
        
    }
    
    return localIndex;
}


AST_NAMESPACE_END
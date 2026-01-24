///
/// @file      LagrangeInterpolator.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-22
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

#include "LagrangeInterpolator.hpp"
#include <cmath>
#include <cstring>

AST_NAMESPACE_BEGIN

void aLagrangeInterpolate(const double arrX[], const double tableY[], int n, int dimy, double x, double* y)
{
    memset(y, 0, dimy * sizeof(double));
    for(int i=0; i<n; i++){
        double tmp1 = 1.0, tmp2 = 1.0;
        for(int j=0; j<n; j++){
            if(j==i) continue;
            tmp1 *= (x -arrX[j]);
			tmp2 *= (arrX[i] - arrX[j]);
        }
        for(int j=0;j<dimy;j++)
        {
            y[j] += tableY[i * dimy + j] * tmp1/tmp2;
        }
    }
}

// double* workArray = new double[20];

void aLagrangeInterpolate2(const double arrX[], const double tableY[], int n, int dimy, double x, double* y)
{
    // 这个实现并不能明显提升速度，甚至有下降
    memset(y, 0, dimy * sizeof(double));
    double* workArray = (double*)alloca(n * sizeof(double));
    // double workArray[20];
    for(int i=0;i<n;i++)
    {
        workArray[i] = x - arrX[i];
    }
    for(int i=0; i<n; i++){
        double tmp1 = 1.0, tmp2 = 1.0;
        for(int j=0; j<n; j++){
            if(j==i) continue;
            tmp1 *= workArray[j];
			tmp2 *= (arrX[i] - arrX[j]);
        }
        for(int j=0;j<dimy;j++)
        {
            y[j] += tableY[i * dimy + j] * tmp1/tmp2;
        }
    }
}

void aLagrangeInterpolate(const double arrX[], const double tableY[], int n, int dimy, double x, double* y, double* workArray)
{
    memset(y, 0, dimy * sizeof(double));
    for(int i=0;i<n;i++)
    {
        workArray[i] = x - arrX[i];
    }
    for(int i=0; i<n; i++){
        double tmp1 = 1.0, tmp2 = 1.0;
        for(int j=0; j<n; j++){
            if(j==i) continue;
            tmp1 *= workArray[j];
			tmp2 *= (arrX[i] - arrX[j]);
        }
        for(int j=0;j<dimy;j++)
        {
            y[j] += tableY[i * dimy + j] * tmp1/tmp2;
        }
    }
}

void aLagrangeInterpolateFixedStep(const double arrX[], const double tableY[], int n, int dimy, double x, double *y, const double *denomArray)
{
    memset(y, 0, dimy * sizeof(double));
    for(int i=0;i<n;i++)
    {
        double tmp1 = denomArray[i];
        for(int j=0;j<n;j++)
        {
            if(j==i) continue;
            tmp1 *= (x - arrX[j]);
        }
        for(int j=0;j<dimy;j++)
        {
            y[j] += tableY[i * dimy + j] * tmp1;
        }
    }
}


void aLagrangeInterpolateDenomArray(int n, double step, double* denomArray)
{
    const int order = n - 1;
    double step_pow_order = pow(step, order);
    for(int i=0;i<=order;i++)
    {
        double denom = 1.0;
        for(int j=0;j<=order;j++)
        {
            if(j!=i)
            {
                denom *= (i - j);
            }
        }
        denomArray[i] = 1.0 / (denom * step_pow_order);
    }
}

#if 0  // @todo

LagrangeInterpolator::LagrangeInterpolator(int order, bool fixedStep, double step)
{
    const int n = order + 1;
    // int order = n - 1;
    workArray_ = new double[n];
    denomArray_ = new double[n];
    if(fixedStep){
        aLagrangeInterpolateDenomArray(n, step, denomArray_);
    }else{
        // pass
    }
}

LagrangeInterpolator::~LagrangeInterpolator()
{
    delete[] workArray_;
    delete[] denomArray_;
}

void LagrangeInterpolator::evaluate(double x, double *out) const
{
    aLagrangeInterpolate(xtable_, ytable_, order_+1, dim_, x, out, workArray_);
}

int LagrangeInterpolator::getDimension() const
{
    return dim_;
}
#endif


AST_NAMESPACE_END



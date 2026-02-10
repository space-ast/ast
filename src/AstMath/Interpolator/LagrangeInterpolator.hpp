///
/// @file      LagrangeInterpolator.hpp
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

#pragma once

#include "AstGlobal.h"
#include "Interpolator.hpp"

AST_NAMESPACE_BEGIN

/*!
   @ingroup Interpolator
   @{
*/



/// @brief 拉格朗日插值
/// @param arrX 插值节点
/// @param tableY 插值节点对应函数值
/// @param n 节点数
/// @param dimy 向量维度
/// @param x 输入值
/// @param y 输出向量，长度需大于等于向量维度
AST_MATH_API void aLagrangeInterpolate(const double arrX[], const double tableY[], int n, int dimy, double x, double* y);
AST_MATH_API void aLagrangeInterpolate2(const double arrX[], const double tableY[], int n, int dimy, double x, double* y);


/// @brief 拉格朗日插值
/// @param arrX 插值节点
/// @param tableY 插值节点对应函数值
/// @param n 节点数
/// @param dimy 向量维度
/// @param x 输入值
/// @param y 输出向量，长度需大于等于向量维度
/// @param workArray 工作数组，长度需大于等于节点数
AST_MATH_API void aLagrangeInterpolate(const double arrX[], const double tableY[], int n, int dimy, double x, double* y, double* workArray);

/// @brief 拉格朗日插值（针对等距节点优化）
/// @param arrX 插值节点
/// @param tableY 插值节点对应函数值
/// @param n 节点数
/// @param dimy 向量维度
/// @param x 输入值
/// @param y 输出向量，长度需大于等于向量维度
/// @param denomArray 分母数组，长度需大于等于节点数
AST_MATH_API void aLagrangeInterpolateFixedStep(const double arrX[], const double tableY[], int n, int dimy, double x, double* y, const double* denomArray);


/// @brief 计算拉格朗日插值的分母数组
/// @param n 节点数
/// @param step 等距节点间隔
/// @param denomArray 分母数组，长度需大于等于节点数
AST_MATH_API void aLagrangeInterpolateDenomArray(int n, double step, double* denomArray);

#if 0  // @todo

/// @brief 拉格朗日插值器
class LagrangeInterpolator: public Interpolator
{
public:
    /// @brief 构造函数
    /// @param x 插值节点
    /// @param y 插值节点对应函数值
    /// @param n 节点数
    LagrangeInterpolator(int order, bool fixedStep, double step);

    /// @brief 析构函数
    ~LagrangeInterpolator() override;

    /// @brief 计算插值结果
    /// @param x 输入值
    /// @param out 输出向量，长度需大于等于向量维度
    void evaluate(double x, double* out) const override;

    /// @brief 获取维度
    /// @return 维度
    int getDimension() const override;

protected:
    double* workArray_;     ///< 工作数组
    double* denomArray_;    ///< 分母数组
    double* xtable_;        ///< 
    double* ytable_;        ///<
    int dimtable_;          ///< 表格维度
    int dim_;               ///< 向量维度
    int order_;             ///< 插值阶数
};

#endif


/*! @} */


AST_NAMESPACE_END

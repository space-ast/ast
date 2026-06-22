///
/// @file      Util.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-30
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
#include <complex>
#include <string>

AST_NAMESPACE_BEGIN

/// @brief 将列主序矩阵转换为字符串
/// @param matrix 列主序矩阵
/// @param rows 矩阵行数
/// @param cols 矩阵列数
/// @return 矩阵字符串表示
AST_MATH_API std::string aColMajorMatrixToString(const double* matrix, int rows, int cols);


/// @brief 打印列主序矩阵到文件
/// @param matrix 列主序矩阵
/// @param rows 矩阵行数
/// @param cols 矩阵列数
/// @param file 输出文件指针，默认打印到标准输出
AST_MATH_CAPI void aColMajorMatrixPrint(const double* matrix, int rows, int cols, FILE* file = stdout);


/// @brief 求解一元二次方程 ax² + bx + c = 0
/// @param a 二次项系数
/// @param b 一次项系数
/// @param c 常数项
/// @param root1 第一个实根（当有实根时）
/// @param root2 第二个实根（当有两个实根时）
/// @return 实根个数：2（两个不同实根），1（一个重根），0（无实根，有一对共轭复根）
/// @note 当 a=0 时方程退化为 bx + c = 0，此时若 b≠0 则返回 1 个根，否则返回 0
AST_MATH_API int aSolveQuadratic(double a, double b, double c, double& root1, double& root2);


/// @brief 求解一元二次方程 ax² + bx + c = 0（含复根）
/// @param a 二次项系数
/// @param b 一次项系数
/// @param c 常数项
/// @param root1 第一个根
/// @param root2 第二个根
/// @note 实根时虚部为零，复根时互为共轭且 root1 为负虚部
AST_MATH_API void aSolveQuadraticComplex(double a, double b, double c,
                                         std::complex<double>& root1,
                                         std::complex<double>& root2);


AST_NAMESPACE_END

///
/// @file      Util.cpp
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

#include "Util.hpp"
#include <cmath>
#include <utility>
#ifdef AST_WITH_FMT
#include <fmt/core.h>
#include <fmt/format.h>
#endif

AST_NAMESPACE_BEGIN

std::string aColMajorMatrixToString(const double *matrix, int rows, int cols)
{
    std::string str;
    #ifndef AST_WITH_FMT
    char buffer[128];
    #endif
    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
        {
        #ifdef AST_WITH_FMT
            str += fmt::format("{} ", matrix[i + j * rows]);
        #else
            snprintf(buffer, sizeof(buffer), "%12g ", matrix[i + j * rows]);
            str += buffer;
        #endif
        }
        str += "\n";
    }
    return str;
}

void aColMajorMatrixPrint(const double *matrix, int rows, int cols, FILE *file)
{
    std::string str = aColMajorMatrixToString(matrix, rows, cols);
    fprintf(file, "%s", str.c_str());
}

int aSolveQuadratic(double a, double b, double c, double& root1, double& root2)
{
    // 退化为线性方程 bx + c = 0
    if (a == 0.0)
    {
        if (b == 0.0)
        {
            return 0; // 无解
        }
        root1 = -c / b;
        root2 = root1;
        return 1;
    }

    const double discriminant = b * b - 4.0 * a * c;

    if (discriminant < 0.0)
    {
        return 0; // 一对共轭复根，无实根
    }

    if (discriminant == 0.0)
    {
        root1 = -b / (2.0 * a);
        root2 = root1;
        return 1; // 重根
    }

    // 数值稳定的求根公式：避免相消误差
    const double sqrtD = std::sqrt(discriminant);
    if (b > 0.0)
    {
        root1 = (-b - sqrtD) / (2.0 * a);
    }
    else
    {
        root1 = (-b + sqrtD) / (2.0 * a);
    }
    root2 = c / (a * root1); // 利用 Viète 定理：root1 * root2 = c/a

    // 确保 root1 是较小的根
    if (root1 > root2)
    {
        std::swap(root1, root2);
    }

    return 2;
}


void aSolveQuadraticComplex(double a, double b, double c,
                           std::complex<double>& root1,
                           std::complex<double>& root2)
{
    using Complex = std::complex<double>;

    // 退化为线性方程 bx + c = 0
    if (a == 0.0)
    {
        if (b == 0.0)
        {
            root1 = root2 = Complex{};
            return; // 无解
        }
        root1 = root2 = Complex(-c / b, 0.0);
        return;
    }

    const double discriminant = b * b - 4.0 * a * c;

    if (discriminant >= 0.0)
    {
        // 实根（数值稳定算法）
        const double sqrtD = std::sqrt(discriminant);
        double r1, r2;
        if (b > 0.0)
        {
            r1 = (-b - sqrtD) / (2.0 * a);
        }
        else
        {
            r1 = (-b + sqrtD) / (2.0 * a);
        }
        r2 = c / (a * r1);

        if (r1 > r2)
        {
            std::swap(r1, r2);
        }

        root1 = Complex(r1, 0.0);
        root2 = Complex(r2, 0.0);
    }
    else
    {
        // 共轭复根
        const double realPart = -b / (2.0 * a);
        const double imagPart = std::sqrt(-discriminant) / (2.0 * a);
        root1 = Complex(realPart, -std::abs(imagPart));
        root2 = Complex(realPart,  std::abs(imagPart));
    }
}


AST_NAMESPACE_END

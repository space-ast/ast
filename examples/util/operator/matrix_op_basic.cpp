#include "AstMath/MathOperator.hpp"
#include "AstMath/Matrix.hpp"
#include <vector>

int main()
{
    using namespace _AST math;
    AST_USING_NAMESPACE

    // 使用 MatrixMN 类
    {
        MatrixMN<double, 2, 3> A = {1,2,3,4,5,6};
        MatrixMN<double, 3, 2> B = {7,8,9,10,11,12};
        auto C = A * B;  // 2x2 矩阵
    }
    // 使用原生数组
    {
        double A[2][3] = {{1,2,3}, {4,5,6}};
        double B[3][2] = {{7,8}, {9,10}, {11,12}};
        auto C = mtimes(A, B);  // 2x2 矩阵
    }
}
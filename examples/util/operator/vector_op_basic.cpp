#include "AstCore/MathOperator.hpp"
#include <vector>

int main()
{
    using namespace _AST math;

    // 使用 std::vector
    std::vector<double> v1 = {1, 2, 3};
    std::vector<double> v2 = {4, 5, 6};

    double dot_product = dot(v1, v2);           // 点积
    auto cross_product = cross(v1, v2);         // 叉积
    double magnitude = norm(v1);                // 范数
    auto normalized_v = normalized(v1);         // 归一化

    // 使用原生数组
    double arr1[3] = {1, 2, 3};
    double arr2[3] = {4, 5, 6};

    dot_product = dot(arr1, arr2);
}
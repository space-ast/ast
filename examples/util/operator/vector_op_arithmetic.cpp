#include "AstCore/MathOperator.hpp"
#include <vector>

int main()
{
    using namespace _AST math;

    std::vector<double> v = {1, 2, 3};

    // 标量运算
    auto v2 = 2.0 * v;      // {2, 4, 6}
    auto v3 = v + v;        // {2, 4, 6}
    auto v4 = v - 1.0;      // {0, 1, 2}
}
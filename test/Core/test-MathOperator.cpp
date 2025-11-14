/// @file      test-MathOperator.cpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      15.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

/// ast项目（https://github.com/space-ast/ast）
/// 根据 Apache 许可证 2.0 版本（"许可证"）授权；
/// 除非符合许可证，否则不得使用此文件。
/// 您可以在以下网址获取许可证副本：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 除非适用法律要求或书面同意，按"原样"分发的软件
/// 没有任何明示或暗示的担保或条件。
/// 请参阅许可证了解特定语言的权限和限制。
 


#include "AstCore/MathOperator.hpp"
#include "AstCore/Vector.hpp"
#include "AstTest/AstTestMacro.h"
#include <vector>
#include <array>
#include <stdio.h>



int testMathFunction()
{
    AST_USING_NAMESPACE
    const double expect = sqrt(9+16+25);
    {
        std::vector<double> a{ 3, 4, 5 };
        double retval = norm(a);
        printf("norm: %lf\n", retval);
        ASSERT_NEAR(retval, expect, 1e-15);
    }
    {
        std::array<double, 3> a{3, 4, 5};
        double retval = norm(a);
        printf("norm: %lf\n", retval);
        ASSERT_NEAR(retval, expect, 1e-15);
    }
    {
        double a[3]{3,4,5};
        double retval = norm(a);
        printf("norm: %lf\n", retval);
        ASSERT_NEAR(retval, expect, 1e-15);
    }
    {
        std::vector<double> a{ 3, 4 };
        double retval = norm(a);
        printf("norm: %lf\n", retval);
        ASSERT_NEAR(retval, 5, 1e-15);
    }
    {
        Vector3d a{3,4,5};
        double retval = norm(a);
        printf("norm: %lf\n", retval);
        ASSERT_NEAR(retval, expect, 1e-15);
    }
    // cross
    {
        double arr1[3]{ 1,2,3 };
        double arr2[3]{ 4,5, 6};
        auto arr = cross(arr1, arr2);
        nothing();
    }
    {
        double arr1[3]{ 1,2,3 };
        double arr2[3]{ 4,5,6 };
        double arr3[3]{};
        double* a= arr1, *b = arr2;
        cross(arr3, a, b);
        nothing();
    }
    {
        std::vector<double> arr1{1,2, 3};
        std::vector<double> arr2{4,5,6};
        auto ret = cross(arr1, arr2);
        nothing();
    }
    {
        std::array<double,3> arr1{ 1,2,3 };
        std::array<double,3> arr2{ 4,5,6 };
        auto ret = cross(arr1, arr2);
        nothing();
    }
    {
        std::vector<double> arr1{ 1,2,3 };
        std::array<double, 3> arr2{ 4,5,6 };
        auto ret = cross(arr1, arr2);
        nothing();
    }
    {
        Vector3d a{3,4,5};
        double b[3]{1,2,3};
        auto ret = cross(a, b);
    }
    // dot
    {
        double a[3]{1,2,3};
        double b[3]{1,2,3};
        double r = dot(a, b);
    }
    {
        std::vector<double> a{1,2,3};
        std::vector<double> b{1,2,3};
        double r = dot(a, b);
    }
    return 0;
}


int testMathOperator()
{
    AST_USING_NAMESPACE
    {
        std::array<double,3> a{1,2,3};
        double b[3]{ 4,5,6 };
        auto ret = a + b;
        nothing();
    }
    {
        Vector3d vec1{ 1,2,3 };
        Vector3d vec2{ 1,2,-3 };
        auto ret = vec1 + vec2;
        nothing();
    }
    {
        Vector3d vec1{1,2,3};
        auto ret = vec1 * 2;
        nothing();
    }
    {
        Vector3d vec1{1,2,3};
        auto ret = vec1 / 2;
        nothing();
    }
    {
        std::array<int, 3> a{1,2,4},b{4,5,6};
        auto c = a - b;
        nothing();
    }
    return 0;
}

int main()
{
    int rc = 0;
    rc |= testMathFunction();
    rc |= testMathOperator();
    return rc;
}
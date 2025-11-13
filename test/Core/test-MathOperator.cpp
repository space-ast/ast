#include "AstCore/MathOperator.hpp"
#include "AstTest/AstTestMacro.h"
#include <vector>
#include <array>


int main()
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

    // cross
    {
        double arr1[3]{ 1,2,3 };
        double arr2[3]{ 4,5, 6};
        auto arr = cross(arr1, arr2);
        printf("");
    }
    {
        double arr1[3]{ 1,2,3 };
        double arr2[3]{ 4,5,6 };
        double arr3[3]{};
        double* a= arr1, *b = arr2;
        cross(arr3, a, b);
        printf("");
    }
    {
        std::vector<double> arr1{1,2, 3};
        std::vector<double> arr2{4,5,6};
        auto ret = cross(arr1, arr2);
        printf("");
    }
    {
        std::array<double,3> arr1{ 1,2,3 };
        std::array<double,3> arr2{ 4,5,6 };
        auto ret = cross(arr1, arr2);
        printf("");
    }
    {
        std::vector<double> arr1{ 1,2,3 };
        std::array<double, 3> arr2{ 4,5,6 };
        auto ret = cross(arr1, arr2);
        printf("");
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

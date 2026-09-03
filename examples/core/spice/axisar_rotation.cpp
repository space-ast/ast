#include "ast/SpiceZpr.hpp"
#include "ast/Vector.hpp"
#include "ast/Matrix.hpp"
#include "ast/Constants.h"
#include <cmath>
#include <cstdio>

AST_USING_NAMESPACE

int main()
{
    // 绕Z轴旋转45度
    Vector3d axis = {0, 0, 1};
    double angle = kPI / 4;  // 45度
    Matrix3d rotation;
    
    axisar(axis, angle, rotation);
    
    printf("axisar旋转矩阵示例:\n");
    printf("旋转轴: (%.1f, %.1f, %.1f), 角度: %.2f rad\n", axis[0], axis[1], axis[2], angle);
    printf("旋转矩阵:\n");
    for (int i = 0; i < 3; i++) {
        printf("  [%.6f %.6f %.6f]\n", rotation(i,0), rotation(i,1), rotation(i,2));
    }
    
    return 0;
}

#include "ast/SpiceZpr.hpp"
#include "ast/Vector.hpp"
#include "ast/Matrix.hpp"
#include "ast/Constants.hpp"

#include <cmath>
#include <cstdio>

AST_USING_NAMESPACE

int main()
{
    // 示例：绕指定轴旋转向量
    Vector3d v = {1, 0, 0};    // 初始向量沿X轴
    Vector3d axis = {0, 0, 1}; // 绕Z轴旋转
    double theta = kPI / 4; // 45度
    
    Vector3d result;
    vrotv(v, axis, theta, result);
    
    printf("向量旋转示例:\n");
    printf("输入向量: (%.4f, %.4f, %.4f)\n", v[0], v[1], v[2]);
    printf("旋转轴: (%.4f, %.4f, %.4f)\n", axis[0], axis[1], axis[2]);
    printf("旋转角度: %.2f 弧度 (%.2f 度)\n", theta, theta * 180 / kPI);
    printf("结果向量: (%.4f, %.4f, %.4f)\n", result[0], result[1], result[2]);
    
    // 示例：矩阵乘法
    Matrix3d m1, m2, mout;
    m1 = Matrix3d::Identity();
    m1(0, 1) = 0.5;
    m1(1, 0) = -0.5;
    
    m2 = Matrix3d::Identity();
    m2(0, 2) = 0.3;
    m2(2, 0) = -0.3;
    
    mxm(m1, m2, mout);
    
    printf("\n矩阵乘法示例:\n");
    printf("矩阵1:\n");
    for (int i = 0; i < 3; i++) {
        printf("  [%.2f %.2f %.2f]\n", m1(i,0), m1(i,1), m1(i,2));
    }
    printf("结果:\n");
    for (int i = 0; i < 3; i++) {
        printf("  [%.4f %.4f %.4f]\n", mout(i,0), mout(i,1), mout(i,2));
    }
    
    return 0;
}

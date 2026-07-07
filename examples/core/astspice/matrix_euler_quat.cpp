#include "ast/SpiceZpr.hpp"
#include "ast/Constants.h"
#include "ast/Matrix.hpp"
#include "ast/Quaternion.hpp"

#include <cmath>
#include <cstdio>

AST_USING_NAMESPACE

int main()
{
    // 示例：欧拉角转旋转矩阵
    double angle1 = kPI / 6;  // 30度
    double angle2 = kPI / 4;  // 45度
    double angle3 = kPI / 3;  // 60度
    
    Matrix3d rotation;
    eul2m(angle3, angle2, angle1, 3, 2, 1, rotation);
    
    printf("欧拉角 (3-2-1顺序): angle1=%.2f, angle2=%.2f, angle3=%.2f (弧度)\n", 
           angle1, angle2, angle3);
    printf("旋转矩阵:\n");
    for (int i = 0; i < 3; i++) {
        printf("  [%8.4f %8.4f %8.4f]\n", rotation(i,0), rotation(i,1), rotation(i,2));
    }
    
    // 示例：旋转矩阵转欧拉角
    double ang1_out, ang2_out, ang3_out;
    m2eul(rotation, 3, 2, 1, ang1_out, ang2_out, ang3_out);
    
    printf("\n反向欧拉角: %.6f, %.6f, %.6f (弧度)\n", ang1_out, ang2_out, ang3_out);
    
    // 示例：旋转矩阵转四元数
    Quaternion q;
    m2q(rotation, q);
    
    printf("\n四元数: w=%.4f, x=%.4f, y=%.4f, z=%.4f\n", q.w(), q.x(), q.y(), q.z());
    
    // 示例：四元数转旋转矩阵
    Matrix3d rotation2;
    q2m(q, rotation2);
    
    return 0;
}

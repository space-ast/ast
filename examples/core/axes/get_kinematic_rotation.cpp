#include "ast/BuiltinAxes.hpp"
#include "ast/TimePoint.hpp"
#include "ast/KinematicRotation.hpp"
#include "ast/Matrix.hpp"
#include <stdio.h>

AST_USING_NAMESPACE

int main()
{
    // 创建时间点
    TimePoint tp1 = TimePoint::FromUTC(2026, 1, 1, 12, 0, 0);
    TimePoint tp2 = TimePoint::FromUTC(2026, 1, 1, 12, 1, 0);  // 1分钟后
    
    printf("获取运动学旋转变换示例:\n\n");
    
    // 获取两个轴系
    Axes* j2000 = aAxesJ2000();
    Axes* ecf = aAxesECF();
    
    // 获取第一个时刻的运动学旋转
    KinematicRotation kinRot1;
    errc_t err = aAxesTransform(j2000, ecf, tp1, kinRot1);
    
    if (err == eNoError) {
        // 获取旋转矩阵
        Matrix3d rot = kinRot1.getRotation().getMatrix();
        printf("时刻1 - J2000到ECF旋转矩阵:\n");
        for (int i = 0; i < 3; i++) {
            printf("  [%.8f %.8f %.8f]\n", rot(i,0), rot(i,1), rot(i,2));
        }
        
        // 获取角速度向量
        Vector3d angVel = kinRot1.getRotationRate();
        printf("\n时刻1 - 角速度 (rad/s):\n");
        printf("  (%.10e, %.10e, %.10e)\n", angVel[0], angVel[1], angVel[2]);
    }
    
    // 获取第二个时刻的运动学旋转，用于计算平均角速度
    KinematicRotation kinRot2;
    aAxesTransform(j2000, ecf, tp2, kinRot2);
    
    printf("\n时刻2 - 角速度 (rad/s):\n");
    Vector3d angVel2 = kinRot2.getRotationRate();
    printf("  (%.10e, %.10e, %.10e)\n", angVel2[0], angVel2[1], angVel2[2]);
    
    // 估算地球自转角速度（简化计算）
    double dt = 60.0;  // 1分钟
    Vector3d avgAngVel = (angVel2 - kinRot1.getRotationRate()) / dt;
    printf("\n角速度变化率:\n");
    printf("  (%.10e, %.10e, %.10e) rad/s^2\n", avgAngVel[0], avgAngVel[1], avgAngVel[2]);
    
    return 0;
}

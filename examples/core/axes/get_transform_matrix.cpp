#include "ast/BuiltinAxes.hpp"
#include "ast/TimePoint.hpp"
#include "ast/Matrix.hpp"
#include <stdio.h>

AST_USING_NAMESPACE

int main()
{
    // 创建时间点
    TimePoint tp = TimePoint::FromUTC(2026, 1, 1, 12, 0, 0);
    
    printf("获取转换矩阵示例:\n\n");
    
    // 获取两个轴系
    Axes* j2000 = aAxesJ2000();
    Axes* tod = aAxesTOD();
    
    // 直接获取转换矩阵（不需要先获取Rotation对象）
    Matrix3d matrix;
    errc_t err = aAxesTransform(j2000, tod, tp, matrix);
    
    if (err == eNoError) {
        printf("J2000到TOD转换矩阵:\n");
        for (int i = 0; i < 3; i++) {
            printf("  [%.10f %.10f %.10f]\n", matrix(i,0), matrix(i,1), matrix(i,2));
        }
        
        // 直接使用矩阵进行坐标变换
        Vector3d vecJ2000{1000.0, 2000.0, 3000.0};
        Vector3d vecTOD = matrix * vecJ2000;
        printf("\n坐标变换:\n");
        printf("J2000坐标: (%.3f, %.3f, %.3f)\n", vecJ2000[0], vecJ2000[1], vecJ2000[2]);
        printf("TOD坐标: (%.3f, %.3f, %.3f)\n", vecTOD[0], vecTOD[1], vecTOD[2]);
    }
    
    return 0;
}

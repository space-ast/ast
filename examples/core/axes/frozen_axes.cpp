#include "ast/BuiltinAxes.hpp"
#include "ast/AxesFrozen.hpp"
#include "ast/TimePoint.hpp"
#include "ast/Rotation.hpp"
#include "ast/Matrix.hpp"
#include <stdio.h>

AST_USING_NAMESPACE

int main()
{
    // 创建时间点
    TimePoint freezeTime = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    TimePoint evalTime = TimePoint::FromUTC(2026, 6, 15, 12, 0, 0);
    
    printf("AxesFrozen冻结轴系示例:\n");
    
    // 获取ECF轴系
    Axes* ecf = aAxesECF();
    
    // 创建冻结轴系：在freezeTime时刻冻结ECF轴系
    Axes* frozenAxes = AxesFrozen::New(ecf, freezeTime, aAxesICRF());
    
    printf("冻结时间: 2026-01-01 00:00:00 UTC\n");
    printf("求值时间: 2026-06-15 12:00:00 UTC\n");
    
    // 获取冻结轴系到ICRF的变换（在冻结时刻的值）
    Rotation rotation;
    frozenAxes->getTransformTo(aAxesICRF(), freezeTime, rotation);
    
    printf("\n在冻结时刻的变换矩阵:\n");
    Matrix3d matrix1 = rotation.getMatrix();
    for (int i = 0; i < 3; i++) {
        printf("  [%.8f %.8f %.8f]\n", matrix1(i,0), matrix1(i,1), matrix1(i,2));
    }
    
    // 在其他时刻获取变换（仍然是冻结时的值）
    Rotation rotation2;
    frozenAxes->getTransformTo(aAxesICRF(), evalTime, rotation2);
    
    printf("\n在求值时刻的变换矩阵（应该是相同的）:\n");
    Matrix3d matrix2 = rotation2.getMatrix();
    for (int i = 0; i < 3; i++) {
        printf("  [%.8f %.8f %.8f]\n", matrix2(i,0), matrix2(i,1), matrix2(i,2));
    }
    
    return 0;
}

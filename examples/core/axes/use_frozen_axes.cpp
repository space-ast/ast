#include "ast/BuiltinAxes.hpp"
#include "ast/AxesFrozen.hpp"
#include "ast/TimePoint.hpp"
#include "ast/Rotation.hpp"
#include "ast/Matrix.hpp"
#include <stdio.h>

AST_USING_NAMESPACE

int main()
{
    // 创建冻结时间和求值时间
    TimePoint freezeTime = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    TimePoint evalTime1 = TimePoint::FromUTC(2026, 6, 15, 12, 0, 0);  // 6个月后
    TimePoint evalTime2 = TimePoint::FromUTC(2026, 12, 31, 23, 59, 59);  // 1年后
    
    printf("使用冻结轴系示例:\n\n");
    
    // 获取轴系
    HAxes ecf = aAxesICRF();
    HAxes reference = aAxesJ2000();
    
    // 创建冻结轴系（使用智能指针自动管理内存）
    HAxesFrozen frozenAxes = AxesFrozen::MakeShared(ecf.get(), freezeTime, reference.get());
    
    printf("冻结时间: 2026-01-01 00:00:00 UTC\n\n");
    
    // 在冻结时刻获取变换
    printf("--- 在冻结时刻获取 ---\n");
    Rotation rot1;
    errc_t err = frozenAxes->getTransformTo(reference.get(), freezeTime, rot1);
    if (err != eNoError) {
        printf("错误: 获取变换失败，错误码: %d\n", (int)err);
        return 1;
    }
    Matrix3d mat1 = rot1.getMatrix();
    printf("变换矩阵:\n");
    for (int i = 0; i < 3; i++) {
        printf("  [%.8f %.8f %.8f]\n", mat1(i,0), mat1(i,1), mat1(i,2));
    }
    
    // 在6个月后获取变换（应该返回冻结时刻的值）
    printf("\n--- 在6个月后获取（应该与冻结时刻相同） ---\n");
    Rotation rot2;
    err = frozenAxes->getTransformTo(reference.get(), evalTime1, rot2);
    if (err != eNoError) {
        printf("错误: 获取变换失败，错误码: %d\n", (int)err);
        return 1;
    }
    Matrix3d mat2 = rot2.getMatrix();
    printf("变换矩阵:\n");
    for (int i = 0; i < 3; i++) {
        printf("  [%.8f %.8f %.8f]\n", mat2(i,0), mat2(i,1), mat2(i,2));
    }
    
    // 在1年后获取变换（也应该与冻结时刻相同）
    printf("\n--- 在1年后获取（应该与冻结时刻相同） ---\n");
    Rotation rot3;
    err = frozenAxes->getTransformTo(reference.get(), evalTime2, rot3);
    if (err != eNoError) {
        printf("错误: 获取变换失败，错误码: %d\n", (int)err);
        return 1;
    }
    Matrix3d mat3 = rot3.getMatrix();
    printf("变换矩阵:\n");
    for (int i = 0; i < 3; i++) {
        printf("  [%.8f %.8f %.8f]\n", mat3(i,0), mat3(i,1), mat3(i,2));
    }
    

    return 0;
}

#include "ast/Axes.hpp"
#include "ast/AxesICRF.hpp"
#include "ast/AxesRoot.hpp"
#include "ast/TimePoint.hpp"
#include "ast/Rotation.hpp"
#include "ast/Matrix.hpp"

AST_USING_NAMESPACE

int main()
{
    // 创建时间点
    TimePoint tp = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    
    // 获取源轴系和目标轴系
    Axes* icrf = aAxesICRF();
    Axes* root = aAxesRoot();
    
    // 使用aAxesTransform函数计算两个轴系之间的旋转变换
    Rotation rotation;
    errc_t err = aAxesTransform(icrf, root, tp, rotation);
    
    if (err != eNoError) {
        printf("错误: aAxesTransform失败，错误码: %d\n", (int)err);
        return 1;
    }
    
    printf("Axes轴系转换示例:\n");
    printf("源轴系: ICRF\n");
    printf("目标轴系: Root\n");
    
    Matrix3d matrix = rotation.getMatrix();
    printf("转换矩阵:\n");
    for (int i = 0; i < 3; i++) {
        printf("  [%.6f %.6f %.6f]\n", matrix(i,0), matrix(i,1), matrix(i,2));
    }
    
    // 使用成员函数进行转换
    Rotation rotation2;
    err = icrf->getTransformTo(root, tp, rotation2);
    if (err != eNoError) {
        printf("错误: getTransformTo失败，错误码: %d\n", (int)err);
        return 1;
    }
    
    printf("\n使用成员函数getTransformTo获取转换:\n");
    Matrix3d matrix2 = rotation2.getMatrix();
    for (int i = 0; i < 3; i++) {
        printf("  [%.6f %.6f %.6f]\n", matrix2(i,0), matrix2(i,1), matrix2(i,2));
    }
    
    return 0;
}

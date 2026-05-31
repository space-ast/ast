#include "ast/BuiltinAxes.hpp"
#include "ast/TimePoint.hpp"
#include "ast/Rotation.hpp"
#include "ast/Matrix.hpp"
#include <stdio.h>

AST_USING_NAMESPACE

int main()
{
    // 创建时间点
    TimePoint tp = TimePoint::FromUTC(2026, 1, 1, 12, 0, 0);
    
    printf("BuiltinAxes内置轴系示例:\n\n");
    
    // 获取各种内置轴系
    // 1. ICRF/J2000 惯性系
    Axes* j2000 = aAxesJ2000();
    printf("J2000轴系获取成功, 深度: %d\n", j2000->getDepth());
    
    // 2. ECF (Earth Centered Fixed) 地球固连系
    Axes* ecf = aAxesECF();
    printf("ECF轴系获取成功, 深度: %d\n", ecf->getDepth());
    
    // 3. MOD (Mean Equator of Date) 瞬时平赤道系
    Axes* mod = aAxesMOD();
    printf("MOD轴系获取成功, 深度: %d\n", mod->getDepth());
    
    // 4. TOD (True Equator of Date) 瞬时真赤道系
    Axes* tod = aAxesTOD();
    printf("TOD轴系获取成功, 深度: %d\n", tod->getDepth());
    
    // 5. GTOD (Greenwich True Equator of Date) 格林尼治真赤道系
    Axes* gtod = aAxesGTOD();
    printf("GTOD轴系获取成功, 深度: %d\n", gtod->getDepth());
    
    // 6. 通过名称获取轴系
    Axes* galactic = aGetAxes("GALACTIC");
    if (galactic != nullptr) {
        printf("GALACTIC轴系获取成功\n");
    }
    
    // 获取J2000到ECF的转换
    Rotation rotation;
    errc_t err = j2000->getTransformTo(ecf, tp, rotation);
    if (err != eNoError) {
        printf("错误: getTransformTo失败，错误码: %d\n", (int)err);
        return 1;
    }
    
    Matrix3d matrix = rotation.getMatrix();
    printf("\nJ2000到ECF转换矩阵:\n");
    for (int i = 0; i < 3; i++) {
        printf("  [%.8f %.8f %.8f]\n", matrix(i,0), matrix(i,1), matrix(i,2));
    }
    
    return 0;
}

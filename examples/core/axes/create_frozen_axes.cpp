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
    
    printf("创建冻结轴系示例:\n\n");
    
    // 获取ECF轴系
    Axes* ecf = aAxesECF();
    Axes* icrf = aAxesICRF();
    
    // 创建冻结轴系：在freezeTime时刻冻结ECF轴系
    // referenceAxes指定参考轴系，用于确定变换方向
    AxesFrozen* frozenAxes = AxesFrozen::New(ecf, freezeTime, icrf);
    
    printf("冻结时间: 2026-01-01 00:00:00 UTC\n");
    printf("ECF轴系: %p\n", (void*)ecf);
    printf("参考轴系(ICRF): %p\n", (void*)icrf);
    printf("冻结轴系: %p\n", (void*)frozenAxes);
    
    // 获取冻结轴系的属性
    printf("\n冻结轴系属性:\n");
    printf("  冻结时间: %s\n", freezeTime.toString().c_str());
    printf("  冻结轴系指针: %p\n", (void*)frozenAxes->getAxes());
    printf("  参考轴系指针: %p\n", (void*)frozenAxes->getReferenceAxes());
    
    // 验证冻结轴系的父轴系是参考轴系
    Axes* parent = frozenAxes->getParent();
    printf("\n冻结轴系的父轴系: %p (应为ICRF: %p)\n", 
           (void*)parent, (void*)icrf);
    
    return 0;
}

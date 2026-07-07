#include "ast/BuiltinAxes.hpp"
#include "ast/TimePoint.hpp"
#include "ast/Rotation.hpp"
#include <stdio.h>

AST_USING_NAMESPACE

int main()
{
    printf("Axes实例获取示例:\n\n");
    
    // 1. 全局单例轴系
    printf("--- 全局单例轴系 ---\n");
    Axes* icrf = aAxesICRF();
    Axes* j2000 = aAxesJ2000();
    Axes* ecf = aAxesECF();
    Axes* mod = aAxesMOD();
    Axes* tod = aAxesTOD();
    Axes* gtod = aAxesGTOD();
    Axes* root = aAxesRoot();
    
    printf("ICRF: %p\n", (void*)icrf);
    printf("J2000: %p\n", (void*)j2000);
    printf("ECF: %p\n", (void*)ecf);
    printf("MOD: %p\n", (void*)mod);
    printf("TOD: %p\n", (void*)tod);
    printf("GTOD: %p\n", (void*)gtod);
    printf("Root: %p\n", (void*)root);
    
    // 2. 通过名称获取轴系
    printf("\n--- 通过名称获取轴系 ---\n");
    Axes* galactic = aGetAxes("GALACTIC");
    if (galactic != nullptr) {
        printf("GALACTIC轴系获取成功\n");
    }
    
    // 3. 检查轴系层级关系
    printf("\n--- 轴系层级关系 ---\n");
    printf("ICRF深度: %d\n", icrf->getDepth());
    printf("ECF深度: %d\n", ecf->getDepth());
    printf("TOD深度: %d\n", tod->getDepth());
    
    // 4. 验证父子关系
    printf("\n--- 验证父子关系 ---\n");
    printf("ECF的父轴系是否为ICRF: %s\n", 
           (ecf->getParent() == icrf) ? "是" : "否");
    printf("TOD的父轴系是否为MOD: %s\n", 
           (tod->getParent() == mod) ? "是" : "否");
    printf("GTOD的父轴系是否为TOD: %s\n", 
           (gtod->getParent() == tod) ? "是" : "否");
    
    return 0;
}

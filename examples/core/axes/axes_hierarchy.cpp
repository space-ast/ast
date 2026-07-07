#include "ast/BuiltinAxes.hpp"
#include "ast/TimePoint.hpp"
#include <stdio.h>

AST_USING_NAMESPACE

int main()
{
    printf("轴系层级操作示例:\n\n");
    
    // 获取各种轴系
    Axes* root = aAxesRoot();
    Axes* icrf = aAxesICRF();
    Axes* ecf = aAxesECF();
    Axes* j2000 = aAxesJ2000();
    Axes* mod = aAxesMOD();
    Axes* tod = aAxesTOD();
    Axes* gtod = aAxesGTOD();
    
    // 1. 获取父轴系
    printf("--- 获取父轴系 ---\n");
    printf("ICRF的父轴系: %p (应为Root: %p)\n", 
           (void*)icrf->getParent(), (void*)root);
    printf("ECF的父轴系: %p (应为ICRF: %p)\n", 
           (void*)ecf->getParent(), (void*)icrf);
    printf("J2000的父轴系: %p (应为ECF: %p)\n", 
           (void*)j2000->getParent(), (void*)ecf);
    printf("MOD的父轴系: %p (应为J2000: %p)\n", 
           (void*)mod->getParent(), (void*)j2000);
    printf("TOD的父轴系: %p (应为MOD: %p)\n", 
           (void*)tod->getParent(), (void*)mod);
    printf("GTOD的父轴系: %p (应为TOD: %p)\n", 
           (void*)gtod->getParent(), (void*)tod);
    
    // 2. 获取轴系深度
    printf("\n--- 轴系深度 ---\n");
    printf("Root深度: %d\n", root->getDepth());
    printf("ICRF深度: %d\n", icrf->getDepth());
    printf("ECF深度: %d\n", ecf->getDepth());
    printf("J2000深度: %d\n", j2000->getDepth());
    printf("MOD深度: %d\n", mod->getDepth());
    printf("TOD深度: %d\n", tod->getDepth());
    printf("GTOD深度: %d\n", gtod->getDepth());
    
    // 3. 获取祖先轴系
    printf("\n--- 获取祖先轴系 (以GTOD为例) ---\n");
    printf("GTOD的第0层祖先(自身): %p (应为GTOD: %p)\n", 
           (void*)gtod->getAncestor(0), (void*)gtod);
    printf("GTOD的第1层祖先: %p (应为TOD: %p)\n", 
           (void*)gtod->getAncestor(1), (void*)tod);
    printf("GTOD的第2层祖先: %p (应为MOD: %p)\n", 
           (void*)gtod->getAncestor(2), (void*)mod);
    printf("GTOD的第3层祖先: %p (应为J2000: %p)\n", 
           (void*)gtod->getAncestor(3), (void*)j2000);
    printf("GTOD的第4层祖先: %p (应为ECF: %p)\n", 
           (void*)gtod->getAncestor(4), (void*)ecf);
    printf("GTOD的第5层祖先: %p (应为ICRF: %p)\n", 
           (void*)gtod->getAncestor(5), (void*)icrf);
    printf("GTOD的第6层祖先: %p (应为Root: %p)\n", 
           (void*)gtod->getAncestor(6), (void*)root);
    
    // 4. 层级遍历
    printf("\n--- 从GTOD向上遍历到Root ---\n");
    Axes* current = gtod;
    int level = 0;
    while (current != nullptr) {
        printf("Level %d: %p\n", level, (void*)current);
        current = current->getParent();
        level++;
    }
    
    return 0;
}

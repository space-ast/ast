#include "AstCore/SOFA.hpp"
#include "AstCore/TimePoint.hpp"

AST_USING_NAMESPACE

int main()
{
    // 创建时间点
    TimePoint tp = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    
    // 计算章动角
    double dpsi, deps;
    aNutation(tp, dpsi, deps);
    
    // 输出结果
    printf("时间点: 2026-01-01 00:00:00 UTC\n");
    printf("黄经章动角 dpsi: %.9f 弧度\n", dpsi);
    printf("交角章动角 deps: %.9f 弧度\n", deps);
    
    return 0;
}
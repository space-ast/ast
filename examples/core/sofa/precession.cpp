#include "AstCore/SOFA.hpp"
#include "AstCore/TimePoint.hpp"

AST_USING_NAMESPACE

int main()
{
    // 创建时间点
    TimePoint tp = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    
    // 计算岁差角
    double zeta, z, theta;
    aPrecession_IAU1976(tp, zeta, z, theta);
    
    // 输出结果
    printf("时间点: 2026-01-01 00:00:00 UTC\n");
    printf("岁差角 zeta: %.9f 弧度\n", zeta);
    printf("岁差角 z: %.9f 弧度\n", z);
    printf("岁差角 theta: %.9f 弧度\n", theta);
    
    return 0;
}
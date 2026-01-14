#include "AstCore/SOFA.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/RunTime.hpp"

AST_USING_NAMESPACE

int main()
{
    // 加载基础数据并初始化，需要EOP数据中的 UT1 - UTC
    aInitialize();
    
    // 创建时间点
    TimePoint tp = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    
    // 计算格林尼治平恒星时
    double gmst = aGMST_IAU1982(tp);
    
    // 计算格林尼治视恒星时
    double gast = aGAST_IAU1994(tp);
    
    // 计算地球自转角度
    double era = aEarthRotationAngle_IAU2000(tp);
    
    // 输出结果
    printf("时间点: 2026-01-01 00:00:00 UTC\n");
    printf("格林尼治平恒星时 (GMST): %.9f 弧度\n", gmst);
    printf("格林尼治视恒星时 (GAST): %.9f 弧度\n", gast);
    printf("地球自转角度 (ERA): %.9f 弧度\n", era);
    
    // 清理资源
    aUninitialize();
    
    return 0;
}
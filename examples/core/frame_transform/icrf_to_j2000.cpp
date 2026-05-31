#include "ast/FrameTransform.hpp"
#include "ast/TimePoint.hpp"
#include "ast/Vector.hpp"
#include "ast/Literals.hpp"
#include "ast/RunTime.hpp"

AST_USING_NAMESPACE
using namespace _AST literals;

int main()
{
    // 加载基础数据并初始化，需要EOP数据
    // aInitialize();
    
    // 创建时间点
    TimePoint tp = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    
    // J2000坐标
    Vector3d vecJ2000{1000_km, 2000_km, 3000_km};
    
    // 转换到ICRF坐标
    Vector3d vecICRF;
    aJ2000ToICRF(tp, vecJ2000, vecICRF);
    
    // 输出结果
    printf("J2000坐标: %.3f m, %.3f m, %.3f m\n", vecJ2000[0], vecJ2000[1], vecJ2000[2]);
    printf("ICRF坐标: %.3f m, %.3f m, %.3f m\n", vecICRF[0], vecICRF[1], vecICRF[2]);
    
    // 清理资源
    // aUninitialize();
    
    return 0;
}
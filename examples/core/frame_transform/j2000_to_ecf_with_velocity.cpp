#include "AstCore/FrameTransform.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstMath/Vector.hpp"
#include "AstUtil/Literals.hpp"
#include "AstCore/RunTime.hpp"

AST_USING_NAMESPACE
using namespace _AST literals;

int main()
{
    // 加载基础数据并初始化，需要EOP数据
    aInitialize();
    
    // 创建时间点
    TimePoint tp = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    
    // J2000坐标和速度
    Vector3d vecJ2000{1000_km, 2000_km, 3000_km};
    Vector3d velJ2000{100_km/s, 200_km/s, 300_km/s};
    
    // 转换到ECF坐标和速度
    Vector3d vecECF;
    Vector3d velECF;
    aJ2000ToECF(tp, vecJ2000, velJ2000, vecECF, velECF);
    
    // 输出结果
    printf("J2000坐标: %.3f km, %.3f km, %.3f km\n", vecJ2000[0], vecJ2000[1], vecJ2000[2]);
    printf("J2000速度: %.3f km/s, %.3f km/s, %.3f km/s\n", velJ2000[0], velJ2000[1], velJ2000[2]);
    printf("ECF坐标: %.3f km, %.3f km, %.3f km\n", vecECF[0], vecECF[1], vecECF[2]);
    printf("ECF速度: %.3f km/s, %.3f km/s, %.3f km/s\n", velECF[0], velECF[1], velECF[2]);
    
    // 清理资源
    aUninitialize();
    
    return 0;
}
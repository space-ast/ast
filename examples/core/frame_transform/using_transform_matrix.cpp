#include "AstCore/FrameTransform.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstMath/Vector.hpp"
#include "AstMath/Matrix.hpp"
#include "AstUtil/Literals.hpp"
#include "AstCore/RunTime.hpp"

AST_USING_NAMESPACE
using namespace _AST literals;

int main()
{
    // 加载基础数据并初始化，需要EOP数据
    aInitialize();
    
    // 创建时间点
    TimePoint tp = TimePoint::FromUTC(2026, 1, 7, 12, 0, 0);
    
    // 获取J2000到MOD的转换矩阵
    Matrix3d matrix;
    aJ2000ToMODMatrix(tp, matrix);
    
    // J2000坐标
    Vector3d vecJ2000{1000_km, 2000_km, 3000_km};
    
    // 使用矩阵转换坐标
    Vector3d vecMOD = matrix * vecJ2000;
    
    // 输出结果
    printf("J2000坐标: %.3f m, %.3f m, %.3f m\n", vecJ2000[0], vecJ2000[1], vecJ2000[2]);
    printf("转换矩阵:\n");
    printf("%.9f %.9f %.9f\n", matrix(0,0), matrix(0,1), matrix(0,2));
    printf("%.9f %.9f %.9f\n", matrix(1,0), matrix(1,1), matrix(1,2));
    printf("%.9f %.9f %.9f\n", matrix(2,0), matrix(2,1), matrix(2,2));
    printf("MOD坐标: %.3f m, %.3f m, %.3f m\n", vecMOD[0], vecMOD[1], vecMOD[2]);
    
    // 清理资源
    aUninitialize();
    
    return 0;
}
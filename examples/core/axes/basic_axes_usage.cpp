#include "ast/Axes.hpp"
#include "ast/AxesICRF.hpp"
#include "ast/AxesRoot.hpp"
#include "ast/TimePoint.hpp"
#include "ast/Rotation.hpp"
#include "ast/Vector.hpp"

AST_USING_NAMESPACE

int main()
{
    // 创建时间点
    TimePoint tp = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    
    // 获取ICRF轴系（惯性系，全局单例）
    Axes* icrf = aAxesICRF();
    
    // 获取根轴系
    Axes* root = aAxesRoot();
    
    // 从ICRF获取旋转变换（ICRF相对父轴系的变换）
    Rotation rotation;
    errc_t err = icrf->getTransform(tp, rotation);
    if (err != eNoError) {
        printf("错误: getTransform失败，错误码: %d\n", (int)err);
        return 1;
    }
    
    // 获取旋转矩阵
    Matrix3d matrix = rotation.getMatrix();
    
    printf("Axes基本使用示例:\n");
    printf("ICRF轴系获取成功\n");
    printf("旋转变换矩阵:\n");
    for (int i = 0; i < 3; i++) {
        printf("  [%.6f %.6f %.6f]\n", matrix(i,0), matrix(i,1), matrix(i,2));
    }
    
    // 获取轴系层级深度
    printf("ICRF轴系深度: %d\n", icrf->getDepth());
    
    return 0;
}

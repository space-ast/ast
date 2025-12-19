#include "AstUtil/Literals.hpp"

int main(){
    AST_USING_NAMESPACE;
    // 角度转换示例
    double angle1 = 90.0_deg;      // 90度转换为弧度 (约1.5708 rad)
    double angle2 = 3.14159_rad;   // 直接使用弧度值
    double quarter = 0.25 * 360_deg; // 90度
}

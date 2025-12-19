#include "AstUtil/Literals.hpp"

int main(){
    AST_USING_NAMESPACE;
    // 角速度转换示例
    double angSpeed1 = 30_deg_s;   // 30度/秒转换为弧度/秒
    double angSpeed2 = 0.1_rad_s;  // 直接使用弧度/秒
}
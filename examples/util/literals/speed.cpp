#include "AstUtil/Literals.hpp"

int main(){
    AST_USING_NAMESPACE;
    // 速度转换示例
    double speed1 = 300000_km_s;   // 300000千米/秒转换为米/秒
    double speed2 = 100_km_h;      // 100千米/小时转换为米/秒 (约27.78 m/s)
    double speed3 = 25_m_s;        // 直接使用米/秒
}
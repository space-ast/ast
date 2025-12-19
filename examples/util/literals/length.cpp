#include "AstUtil/Literals.hpp"

int main(){
    AST_USING_NAMESPACE;
    // 长度转换示例
    double distance1 = 5.5_km;     // 5.5公里转换为米 (5500米)
    double distance2 = 1000_cm;    // 1000厘米转换为米 (10米)
    double distance3 = 1.0_au;     // 1天文单位转换为米
    double thickness = 0.5_mm;     // 0.5毫米转换为米
}
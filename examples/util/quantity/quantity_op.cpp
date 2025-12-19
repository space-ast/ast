#include "AstUtil/Quantity.hpp"

int main(){
    AST_USING_NAMESPACE
    // 加法：相同量纲的数量可以相加
    Quantity length1 = 5.0 * Unit::Meter();
    Quantity length2 = 100.0 * Unit::Centimeter();
    Quantity totalLength = length1 + length2;  // 自动转换为相同单位

    // 乘法：不同量纲的数量相乘得到新量纲
    Quantity area = length1 * length2;  // 结果为5.0 m²

    // 除法：数量相除得到新量纲
    Quantity velocity = length1 / (10.0 * Unit::Second());  // 结果为0.5 m/s
}

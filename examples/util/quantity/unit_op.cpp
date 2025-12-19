#include "AstUtil/Quantity.hpp"

int main(){
    AST_USING_NAMESPACE
    // 创建米每秒单位
    Unit mps = Unit::Meter() / Unit::Second();

    // 创建千米每小时单位
    Unit kmph = Unit::Kilometer() / Unit::Hour();

    // 创建平方米单位
    Unit sqMeter = Unit::Meter() * Unit::Meter();
}
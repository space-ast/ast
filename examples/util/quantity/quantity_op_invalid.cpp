#include "AstUtil/Quantity.hpp"

int main(){
    AST_USING_NAMESPACE
    Quantity length = 5.0 * Unit::Meter();
    Quantity time = 10.0 * Unit::Second();

    // 合理：长度 + 长度
    Quantity totalLength = length + length;

    // 不合理：长度 + 时间（编译时不会报错，但运行时可能出现问题）
    // Quantity invalid = length + time;
}
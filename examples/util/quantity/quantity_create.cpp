#include "AstUtil/Quantity.hpp"

int main(){
    AST_USING_NAMESPACE
    // 创建5米的长度
    Quantity length = 5.0 * Unit::Meter();

    // 创建10米每秒的速度
    Quantity velocity = 10.0 * (Unit::Meter() / Unit::Second());

    // 创建标量（无量纲）
    Quantity scalar = Quantity::Scalar(42.0);
}
#include "AstUtil/Quantity.hpp"

int main(){
    AST_USING_NAMESPACE
    // 创建速度量纲（长度/时间）
    Dimension velocityDim = Dimension::Length() / Dimension::Time();

    // 创建加速度量纲（长度/时间²）
    Dimension accelerationDim = velocityDim / Dimension::Time();

    // 创建面积量纲（长度²）
    Dimension areaDim = Dimension::Length().pow(2);
}
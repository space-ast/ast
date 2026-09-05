#include "ast/Literals.hpp"
#include "ast/Constants.hpp"

int main(){
    AST_USING_NAMESPACE;
    // 计算示例
    double speed = 100_km / 1_h;   // 计算速度 (约27.78 m/s)
    double frequency = 1.0 / 1_ms; // 计算频率 (1000 Hz)
    double circumference = 2.0 * kPI * 1.0_km; // 计算周长
}
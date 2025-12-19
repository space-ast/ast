#include "AstCore/OrbitElement.hpp"
#include <iostream>

using namespace AST_NAMESPACE;

int main() {
    // 定义经典轨道根数
    OrbElem coe;
    coe.a() = 7000000.0;  // 7000 km
    coe.e() = 0.1;
    coe.i() = 0.0;        // 0 radians
    coe.raan() = 0.0;
    coe.argper() = 0.0;
    coe.trueA() = 0.0;
    
    // 转换为春分点根数
    EquinElem ee;
    coe2ee(coe.data(), ee.data());
    
    // 转换为修正轨道根数
    ModOrbElem moe;
    coe2moe(coe.data(), moe.data());
    
    // 转换为改进春分点轨道根数
    ModEquinElem mee;
    coe2mee(coe.data(), mee.data());
    
    std::cout << "转换完成\n";
    std::cout << "春分点根数长半轴: " << ee.a() << " m\n";
    std::cout << "修正轨道根数近拱点半径: " << moe.rp() << " m\n";
}
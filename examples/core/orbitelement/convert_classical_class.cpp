#include "AstCore/OrbitElement.hpp"
#include "AstMath/Vector.hpp"
#include <iostream>


int main() {
    AST_USING_NAMESPACE 

    // 地球引力参数 (m^3/s^2)
    double gmEarth = 3.986004418e14;
    
    // 定义位置和速度向量
    Vector3d pos{6778137.0, 0.0, 0.0};  // 近地轨道位置
    Vector3d vel{0.0, 7726.0, 0.0};      // 近地轨道速度
    
    // 转换为经典轨道根数
    OrbElem elem;
    err_t err = aCartToOrbElem(pos, vel, gmEarth, elem);
    
    if (err == eNoError) {
        std::cout << "长半轴: " << elem.a() << " m\n";
        std::cout << "偏心率: " << elem.e() << "\n";
        std::cout << "轨道倾角: " << elem.i() << " rad\n";
        
        // 从经典轨道根数转回直角坐标
        Vector3d posBack, velBack;
        err = aOrbElemToCart(elem, gmEarth, posBack, velBack);
        
        if (err == eNoError) {
            std::cout << "位置向量: [" << posBack[0] << ", " << posBack[1] << ", " << posBack[2] << "]\n";
        }
    }
}
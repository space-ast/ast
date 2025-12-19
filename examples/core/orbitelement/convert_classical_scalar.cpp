#include "AstCore/OrbitElement.hpp"
#include <iostream>

int main() {
    AST_USING_NAMESPACE 
    // 地球引力参数 (m^3/s^2)
    double gmEarth = 3.986004418e14;
    
    // 定义经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
    double coe[6] = {7000000.0, 0.01, 0.0, 0.0, 0.0, 0.0};
    
    // 转换为直角坐标
    double pos[3], vel[3];
    err_t err = coe2rv(coe, gmEarth, pos, vel);
    
    if (err == eNoError) {
        std::cout << "位置向量: [" << pos[0] << ", " << pos[1] << ", " << pos[2] << "]\n";
        std::cout << "速度向量: [" << vel[0] << ", " << vel[1] << ", " << vel[2] << "]\n";
        
        // 从直角坐标转回经典轨道根数
        double coeBack[6];
        err = rv2coe(pos, vel, gmEarth, coeBack);
        
        if (err == eNoError) {
            std::cout << "长半轴: " << coeBack[0] << " m\n";
            std::cout << "偏心率: " << coeBack[1] << std::endl;
        }
    }
}
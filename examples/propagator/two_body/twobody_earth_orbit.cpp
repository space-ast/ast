#include "AstCore/TwoBody.hpp"
#include "AstMath/Vector.hpp"
#include "AstUtil/Literals.hpp"
#include "AstUtil/Constants.h"
#include <iostream>
#include <iomanip>
#include <cmath>

AST_USING_NAMESPACE
using namespace _AST literals;

int main()
{
    // 地球引力常数
    const double gm_earth = kEarthGrav;
    
    // 地球同步轨道示例 (高度约35786 km)
    double altitude = 35786_km;
    double radius = kEarthRadius + altitude;
    
    // 计算轨道速度 (圆轨道)
    double orbital_velocity = std::sqrt(gm_earth / radius);
    
    // 初始轨道状态 (地球同步轨道)
    Vector3d position{radius, 0.0, 0.0};
    Vector3d velocity{0.0, orbital_velocity, 0.0};
    
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "地球同步轨道传播示例" << std::endl;
    std::cout << "=====================" << std::endl;
    std::cout << "轨道高度: " << altitude / 1000.0 << " km" << std::endl;
    std::cout << "轨道半径: " << radius / 1000.0 << " km" << std::endl;
    std::cout << "轨道速度: " << orbital_velocity / 1000.0 << " km/s" << std::endl;
    std::cout << "轨道周期: " << (2 * kPI * radius / orbital_velocity) / 3600.0 << " 小时" << std::endl;
    std::cout << std::endl;
    
    // 传播多个时间点
    double orbital_period = 2 * kPI * radius / orbital_velocity; // 轨道周期 (秒)
    
    std::cout << "轨道传播结果:" << std::endl;
    std::cout << "时间(小时)\tX(km)\t\tY(km)\t\tZ(km)\t\t速度(km/s)" << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;
    
    for (int i = 0; i <= 4; i++) {
        double time = i * orbital_period / 4.0; // 四分之一轨道周期
        
        // 复制初始状态
        Vector3d current_pos = position;
        Vector3d current_vel = velocity;
        
        // 执行二体传播
        err_t result = aTwoBodyProp(time, gm_earth, current_pos, current_vel);
        
        if (result == eNoError) {
            double speed = current_vel.norm();
            std::cout << std::setw(8) << time/3600.0 << "\t"
                      << std::setw(10) << current_pos[0]/1000.0 << "\t"
                      << std::setw(10) << current_pos[1]/1000.0 << "\t"
                      << std::setw(10) << current_pos[2]/1000.0 << "\t"
                      << std::setw(10) << speed/1000.0 << std::endl;
        } else {
            std::cout << "传播失败，错误码: " << result << std::endl;
            break;
        }
    }
    
    return 0;
}
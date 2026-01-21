#include "AstCore/J2Analytical.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstMath/Vector.hpp"
#include "AstUtil/Literals.hpp"
#include "AstUtil/Constants.h"
#include <iostream>
#include <iomanip>

AST_USING_NAMESPACE
using namespace _AST literals;

int main()
{
    // 地球参数
    const double gm_earth = kEarthGrav;
    const double j2_earth = 1.08262668e-3;  // 地球J2系数
    const double re_earth = kEarthRadius;   // 地球半径
    
    // 初始轨道状态 (近地轨道，倾角45度)
    Vector3d position{7000_km, 0.0, 0.0};
    Vector3d velocity{0.0, 7.5_km_s, 1.0_km_s}; // 包含Z分量产生倾角
    
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "J2解析传播器 - 位置速度传播示例" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "初始轨道状态:" << std::endl;
    std::cout << "位置: (" << position[0]/1000.0 << ", " 
              << position[1]/1000.0 << ", " 
              << position[2]/1000.0 << ") km" << std::endl;
    std::cout << "速度: (" << velocity[0]/1000.0 << ", " 
              << velocity[1]/1000.0 << ", " 
              << velocity[2]/1000.0 << ") km/s" << std::endl;
    std::cout << std::endl;
    
    // 传播时间间隔 (1天)
    double duration = 24 * 3600.0; // 秒
    
    // 执行J2解析传播
    err_t result = aJ2AnalyticalProp(duration, gm_earth, j2_earth, re_earth, position, velocity);
    
    if (result == eNoError) {
        std::cout << "传播后轨道状态 (" << duration/3600.0 << " 小时后):" << std::endl;
        std::cout << "位置: (" << position[0]/1000.0 << ", " 
                  << position[1]/1000.0 << ", " 
                  << position[2]/1000.0 << ") km" << std::endl;
        std::cout << "速度: (" << velocity[0]/1000.0 << ", " 
                  << velocity[1]/1000.0 << ", " 
                  << velocity[2]/1000.0 << ") km/s" << std::endl;
        
        // 计算轨道参数变化
        double initial_radius = 7000.0; // km
        double final_radius = std::sqrt(position[0]*position[0] + position[1]*position[1] + position[2]*position[2]) / 1000.0;
        
        std::cout << std::endl;
        std::cout << "轨道参数变化:" << std::endl;
        std::cout << "初始轨道半径: " << initial_radius << " km" << std::endl;
        std::cout << "最终轨道半径: " << final_radius << " km" << std::endl;
        std::cout << "半径变化: " << (final_radius - initial_radius) << " km" << std::endl;
    } else {
        std::cout << "传播失败，错误码: " << result << std::endl;
    }
    
    return 0;
}
#include "AstCore/J4Analytical.hpp"
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
    const double j4_earth = -1.620e-6;      // 地球J4系数
    const double re_earth = kEarthRadius;   // 地球半径
    
    // 初始轨道状态 (近地轨道)
    Vector3d position{7000_km, 0.0, 0.0};
    Vector3d velocity{0.0, 7.5_km_s, 1.0_km_s};
    
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "J4解析传播器 - 位置速度传播示例" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "初始轨道状态:" << std::endl;
    std::cout << "位置: (" << position[0]/1000.0 << ", " 
              << position[1]/1000.0 << ", " 
              << position[2]/1000.0 << ") km" << std::endl;
    std::cout << "速度: (" << velocity[0]/1000.0 << ", " 
              << velocity[1]/1000.0 << ", " 
              << velocity[2]/1000.0 << ") km/s" << std::endl;
    std::cout << std::endl;
    
    // 传播时间间隔 (3天)
    double duration = 3 * 24 * 3600.0; // 秒
    
    // 执行J4解析传播
    err_t result = aJ4AnalyticalProp(duration, gm_earth, j2_earth, j4_earth, re_earth, position, velocity);
    
    if (result == eNoError) {
        std::cout << "传播后轨道状态 (" << duration/(24*3600.0) << " 天后):" << std::endl;
        std::cout << "位置: (" << position[0]/1000.0 << ", " 
                  << position[1]/1000.0 << ", " 
                  << position[2]/1000.0 << ") km" << std::endl;
        std::cout << "速度: (" << velocity[0]/1000.0 << ", " 
                  << velocity[1]/1000.0 << ", " 
                  << velocity[2]/1000.0 << ") km/s" << std::endl;
        
        // 计算轨道倾角变化
        double initial_inclination = std::atan2(1.0, 7.5) * kRadToDeg; // 近似计算
        double final_inclination = std::atan2(velocity[2], std::sqrt(velocity[0]*velocity[0] + velocity[1]*velocity[1])) * kRadToDeg;
        
        std::cout << std::endl;
        std::cout << "轨道参数:" << std::endl;
        std::cout << "初始倾角: " << initial_inclination << " 度" << std::endl;
        std::cout << "最终倾角: " << final_inclination << " 度" << std::endl;
        std::cout << "倾角变化: " << (final_inclination - initial_inclination) << " 度" << std::endl;
    } else {
        std::cout << "传播失败，错误码: " << result << std::endl;
    }
    
    return 0;
}
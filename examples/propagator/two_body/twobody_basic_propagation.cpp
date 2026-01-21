#include "AstCore/TwoBody.hpp"
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
    // 地球引力常数 (m³/s²)
    const double gm_earth = 3.986004418e14;
    
    // 初始轨道状态 (近地轨道)
    Vector3d position{7000_km, 0.0, 0.0};  // 位置向量 (米)
    Vector3d velocity{0.0, 7.5_km_s, 0.0}; // 速度向量 (米/秒)
    
    std::cout << "初始轨道状态:" << std::endl;
    std::cout << "位置: (" << position[0] << ", " << position[1] << ", " << position[2] << ") m" << std::endl;
    std::cout << "速度: (" << velocity[0] << ", " << velocity[1] << ", " << velocity[2] << ") m/s" << std::endl;
    std::cout << std::endl;
    
    // 传播时间间隔 (1小时)
    double duration = 3600.0; // 秒
    
    // 执行二体传播
    err_t result = aTwoBodyProp(duration, gm_earth, position, velocity);
    
    if (result == eNoError) {
        std::cout << "传播后轨道状态 (" << duration << " 秒后):" << std::endl;
        std::cout << "位置: (" << position[0] << ", " << position[1] << ", " << position[2] << ") m" << std::endl;
        std::cout << "速度: (" << velocity[0] << ", " << velocity[1] << ", " << velocity[2] << ") m/s" << std::endl;
        
        // 计算轨道半径
        double radius = position.norm();
        std::cout << "轨道半径: " << radius / 1000.0 << " km" << std::endl;
        
        // 计算轨道速度
        double speed = velocity.norm();
        std::cout << "轨道速度: " << speed / 1000.0 << " km/s" << std::endl;
    } else {
        std::cout << "传播失败，错误码: " << result << std::endl;
    }
    
    return 0;
}
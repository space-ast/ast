#include "AstCore/J4Analytical.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstMath/Vector.hpp"
#include "AstUtil/Literals.hpp"
#include "AstUtil/Constants.h"
#include <iostream>
#include <iomanip>

AST_USING_NAMESPACE
using namespace _AST literals;

int main()
{
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "J4解析传播器 - 类接口使用示例" << std::endl;
    std::cout << "================================" << std::endl;
    
    // 地球参数
    const double gm_earth = 3.986004418e14;      // 地球引力常数 (m³/s²)
    const double j2_earth = 1.08262668e-3;       // 地球J2系数
    const double j4_earth = -1.620e-6;           // 地球J4系数
    const double re_earth = 6378137.0;           // 地球半径 (m)
    
    // 示例1: 使用轨道根数构造J4传播器
    std::cout << "示例1: 使用轨道根数构造J4传播器" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    
    TimePoint epoch1 = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    
    // 创建改进轨道根数 (太阳同步轨道)
    ModOrbElem modOrbElem1;
    modOrbElem1.rp_ = 7178.137_km;               // 近拱点半径 (km)
    modOrbElem1.e_ = 0.001;                      // 偏心率
    modOrbElem1.i_ = 98.0_deg;                   // 轨道倾角
    modOrbElem1.argper_ = 0.0_deg;               // 近拱点角
    modOrbElem1.raan_ = 0.0_deg;                 // 升交点赤经
    modOrbElem1.trueA_ = 0.0_deg;                // 真近点角
    
    // 使用轨道根数构造J4传播器
    J4Analytical j4_prop1(modOrbElem1, epoch1, gm_earth, j2_earth, j4_earth, re_earth);
    
    std::cout << "轨道根数构造成功" << std::endl;
    std::cout << "半长轴: " << modOrbElem1.getA()/1000.0 << " km" << std::endl;
    std::cout << "偏心率: " << modOrbElem1.e_ << std::endl;
    std::cout << "轨道倾角: " << modOrbElem1.i_/kDegToRad << " 度" << std::endl;
    std::cout << std::endl;
    
    // 示例2: 使用位置速度构造J4传播器
    std::cout << "示例2: 使用位置速度构造J4传播器" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    
    TimePoint epoch2 = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    
    // 初始位置速度 (近圆轨道)
    Vector3d position{7000_km, 0.0, 0.0};
    Vector3d velocity{0.0, 7.5_km_s, 0.0};
    
    // 使用位置速度构造J4传播器
    J4Analytical j4_prop2(position, velocity, epoch2, gm_earth, j2_earth, j4_earth, re_earth);
    
    std::cout << "位置速度构造成功" << std::endl;
    std::cout << "位置: (" << position[0]/1000.0 << ", " 
              << position[1]/1000.0 << ", " 
              << position[2]/1000.0 << ") km" << std::endl;
    std::cout << "速度: (" << velocity[0]/1000.0 << ", " 
              << velocity[1]/1000.0 << ", " 
              << velocity[2]/1000.0 << ") km/s" << std::endl;
    std::cout << std::endl;
    
    // 示例3: 完整传播流程演示
    std::cout << "示例3: 完整传播流程演示" << std::endl;
    std::cout << "------------------------" << std::endl;
    
    // 创建时间点
    TimePoint start_time = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    TimePoint end_time = start_time + 24 * 3600.0; // 1天后
    
    // 初始轨道状态 (大椭圆轨道)
    Vector3d pos{26560_km, 0.0, 0.0};           // 约20000km高度
    Vector3d vel{0.0, 3.87_km_s, 0.0};          // 约12小时周期
    
    std::cout << "传播前状态:" << std::endl;
    std::cout << "起始时间: 2026-01-01 00:00:00 UTC" << std::endl;
    std::cout << "位置: (" << pos[0]/1000.0 << ", " 
              << pos[1]/1000.0 << ", " 
              << pos[2]/1000.0 << ") km" << std::endl;
    std::cout << "速度: (" << vel[0]/1000.0 << ", " 
              << vel[1]/1000.0 << ", " 
              << vel[2]/1000.0 << ") km/s" << std::endl;
    
    // 使用函数接口进行传播
    err_t result = aJ4AnalyticalProp(24 * 3600.0, gm_earth, j2_earth, j4_earth, re_earth, pos, vel);
    
    if (result == eNoError) {
        std::cout << std::endl;
        std::cout << "传播后状态:" << std::endl;
        std::cout << "结束时间: 2026-01-02 00:00:00 UTC" << std::endl;
        std::cout << "位置: (" << pos[0]/1000.0 << ", " 
                  << pos[1]/1000.0 << ", " 
                  << pos[2]/1000.0 << ") km" << std::endl;
        std::cout << "速度: (" << vel[0]/1000.0 << ", " 
                  << vel[1]/1000.0 << ", " 
                  << vel[2]/1000.0 << ") km/s" << std::endl;
        
        // 计算轨道参数变化
        double initial_radius = 26560.0; // km
        double final_radius = pos.norm() / 1000.0;
        double radius_change = final_radius - initial_radius;
        
        std::cout << std::endl;
        std::cout << "轨道参数变化:" << std::endl;
        std::cout << "初始轨道半径: " << initial_radius << " km" << std::endl;
        std::cout << "最终轨道半径: " << final_radius << " km" << std::endl;
        std::cout << "轨道半径变化: " << radius_change << " km" << std::endl;
        std::cout << "注: 变化主要由J2和J4摄动引起" << std::endl;
    } else {
        std::cout << "传播失败，错误码: " << result << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "J4解析传播器类接口使用总结:" << std::endl;
    std::cout << "1. 支持使用轨道根数或位置速度构造传播器" << std::endl;
    std::cout << "2. 提供函数接口和类接口两种使用方式" << std::endl;
    std::cout << "3. 考虑J2和J4摄动项，精度高于J2传播器" << std::endl;
    std::cout << "4. 适用于需要较高精度的近地轨道传播" << std::endl;
    
    return 0;
}
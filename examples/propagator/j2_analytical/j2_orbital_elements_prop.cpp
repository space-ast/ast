#include "AstCore/J2Analytical.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/OrbitElement.hpp"
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
    // 地球参数
    const double gm_earth = kEarthGrav;
    const double j2_earth = 1.08262668e-3;
    const double re_earth = kEarthRadius;
    
    // 创建改进轨道根数 (太阳同步轨道示例)
    ModOrbElem orb_elem;
    orb_elem.rp_ = 7000_km;         // 近拱点半径
    orb_elem.e_ = 0.001;            // 偏心率
    orb_elem.i_ = 98.0 * kDegToRad; // 倾角 (太阳同步轨道)
    orb_elem.raan_ = 0.0;           // 升交点赤经
    orb_elem.argper_ = 0.0;         // 近拱点角
    orb_elem.trueA_ = 0.0;          // 真近点角
    
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "J2解析传播器 - 轨道根数传播示例" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "初始轨道根数:" << std::endl;
    std::cout << "半长轴: " << orb_elem.getA() / 1000.0 << " km" << std::endl;
    std::cout << "偏心率: " << orb_elem.e_ << std::endl;
    std::cout << "倾角: " << orb_elem.i_ * kRadToDeg << " 度" << std::endl;
    std::cout << "升交点赤经: " << orb_elem.raan_ * kRadToDeg << " 度" << std::endl;
    std::cout << "近拱点角: " << orb_elem.argper_ * kRadToDeg << " 度" << std::endl;
    std::cout << "真近点角: " << orb_elem.trueA_ * kRadToDeg << " 度" << std::endl;
    std::cout << std::endl;
    
    // 传播多个时间点，观察J2摄动效应
    double duration_day = 1.0; // 天
    double duration_sec = duration_day * 24 * 3600.0;
    
    std::cout << "轨道根数随时间变化 (" << duration_day << " 天):" << std::endl;
    std::cout << "时间(天)\t升交点赤经(度)\t近地点幅角(度)" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
    
    for (int i = 0; i <= 10; i++) {
        double time = i * duration_sec / 10.0;
        
        // 复制初始轨道根数
        ModOrbElem current_orb = orb_elem;
        
        // 执行J2解析传播
        err_t result = aJ2AnalyticalProp(time, gm_earth, j2_earth, re_earth, current_orb);
        
        if (result == eNoError) {
            std::cout << std::setw(6) << time/(24*3600.0) << "\t"
                      << std::setw(12) << current_orb.raan_ * kRadToDeg << "\t"
                      << std::setw(12) << current_orb.argper_ * kRadToDeg << std::endl;
        } else {
            std::cout << "传播失败，错误码: " << result << std::endl;
            break;
        }
    }
    
    // 计算J2引起的升交点退行率
    double semi_major_axis = orb_elem.getA();
    double eccentricity = orb_elem.e_;
    double inclination = orb_elem.i_;
    
    // 升交点退行率公式: dΩ/dt = -3/2 * J2 * n * (R/p)^2 * cos(i)
    double n = std::sqrt(gm_earth / (semi_major_axis * semi_major_axis * semi_major_axis)); // 平均运动
    double p = semi_major_axis * (1 - eccentricity * eccentricity); // 半通径
    double raan_rate = -1.5 * j2_earth * n * std::pow(re_earth / p, 2) * std::cos(inclination);
    
    std::cout << std::endl;
    std::cout << "J2摄动理论值:" << std::endl;
    std::cout << "升交点退行率: " << raan_rate * kRadToDeg * 86400.0 << " 度/天" << std::endl;
    
    return 0;
}
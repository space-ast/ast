#include "AstCore/J4Analytical.hpp"
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
    const double j4_earth = -1.620e-6;
    const double re_earth = kEarthRadius;
    
    // 创建改进轨道根数 (大椭圆轨道示例)
    ModOrbElem orb_elem;
    orb_elem.rp_ = 26500_km;       // 近拱点半径 
    orb_elem.e_ = 0.73;           // 偏心率
    orb_elem.i_ = 28.5 * kDegToRad; // 倾角
    orb_elem.raan_ = 0.0;
    orb_elem.argper_ = 0.0;
    orb_elem.trueA_ = 0.0;
    
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "J4解析传播器 - 轨道根数传播示例" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "初始轨道根数 (大椭圆轨道):" << std::endl;
    std::cout << "半长轴: " << orb_elem.getA() / 1000.0 << " km" << std::endl;
    std::cout << "偏心率: " << orb_elem.e_ << std::endl;
    std::cout << "倾角: " << orb_elem.i_ * kRadToDeg << " 度" << std::endl;
    std::cout << "升交点赤经: " << orb_elem.raan_ * kRadToDeg << " 度" << std::endl;
    std::cout << "近地点幅角: " << orb_elem.argper_ * kRadToDeg << " 度" << std::endl;
    std::cout << std::endl;
    
    // 传播多个时间点，观察J4摄动效应
    double duration_day = 7.0; // 7天
    double duration_sec = duration_day * 24 * 3600.0;
    
    std::cout << "轨道根数随时间变化 (" << duration_day << " 天):" << std::endl;
    std::cout << "时间(天)\t升交点赤经(度)\t近地点幅角(度)\t偏心率" << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;
    
    for (int i = 0; i <= 7; i++) {
        double time = i * duration_sec / 7.0;
        
        // 复制初始轨道根数
        ModOrbElem current_orb = orb_elem;
        
        // 执行J4解析传播
        err_t result = aJ4AnalyticalProp(time, gm_earth, j2_earth, j4_earth, re_earth, current_orb);
        
        if (result == eNoError) {
            std::cout << std::setw(6) << time/(24*3600.0) << "\t"
                      << std::setw(12) << current_orb.raan_ * kRadToDeg << "\t"
                      << std::setw(12) << current_orb.argper_ * kRadToDeg << "\t"
                      << std::setw(10) << current_orb.e_ << std::endl;
        } else {
            std::cout << "传播失败，错误码: " << result << std::endl;
            break;
        }
    }
    
    // 计算J4对近地点进动的修正
    double semi_major_axis = orb_elem.getA();
    double eccentricity = orb_elem.e_;
    double inclination = orb_elem.i_;
    
    // J2引起的近地点进动率
    double n = std::sqrt(gm_earth / (semi_major_axis * semi_major_axis * semi_major_axis));
    double p = semi_major_axis * (1 - eccentricity * eccentricity);
    double aop_rate_j2 = 1.5 * j2_earth * n * std::pow(re_earth / p, 2) * (2 - 2.5 * std::sin(inclination) * std::sin(inclination));
    
    // J4引起的近地点进动率修正
    double aop_rate_j4 = 0.0; // 简化计算，实际需要复杂公式
    
    std::cout << std::endl;
    std::cout << "摄动理论值:" << std::endl;
    std::cout << "J2近地点进动率: " << aop_rate_j2 * kRadToDeg * 86400.0 << " 度/天" << std::endl;
    std::cout << "J4提供更高精度修正" << std::endl;
    
    return 0;
}
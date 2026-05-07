///
/// @file      orbit_prediction.cpp
/// @brief     轨道预报综合示例
/// @details   展示如何使用HPOP进行高精度轨道预报，包含多种力模型配置和轨道状态输出
/// @author    ast
/// @date      2026-05-01
///

#include "AstCore/HPOP.hpp"
#include "AstCore/HPOPForceModel.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/RunTime.hpp"
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
    // 初始化运行时环境
    aInitialize();
    setlocale(LC_ALL, ".UTF-8");
    std::cout << std::fixed << std::setprecision(6);
    
    std::cout << "==========================================" << std::endl;
    std::cout << "      HPOP高精度轨道预报器 - 综合示例       " << std::endl;
    std::cout << "==========================================" << std::endl;

    // ========== 1. 创建HPOP对象 ==========
    HPOP hpop;

    // ========== 2. 配置力模型 ==========
    HPOPForceModel force_model;
    
    // 配置地球重力场 (JGM3模型，10阶10次)
    force_model.gravity().model_ = "JGM3";
    force_model.gravity().maxDegree_ = 10;
    force_model.gravity().maxOrder_ = 10;
    
    // 启用大气阻力
    force_model.useDrag(true);
    
    // 启用太阳辐射压
    force_model.useSRP(true);
    
    // 启用月球引力
    force_model.useMoonGravity(true);
    
    // 启用相对论修正
    force_model.useRelativityCorrection(true);

    // 设置力模型到HPOP
    errc_t result = hpop.setForceModel(force_model);
    if (result != eNoError) {
        std::cout << "[错误] 设置力模型失败，错误码: " << result << std::endl;
        return -1;
    }

    // ========== 3. 初始化HPOP ==========
    result = hpop.initialize();
    if (result != eNoError) {
        std::cout << "[错误] HPOP初始化失败，错误码: " << result << std::endl;
        return -1;
    }

    std::cout << "\n【力模型配置】" << std::endl;
    std::cout << "- 重力场: " << force_model.gravity().model_ 
              << " (阶数=" << force_model.gravity().maxDegree_ 
              << ", 次数=" << force_model.gravity().maxOrder_ << ")" << std::endl;
    std::cout << "- 大气阻力: " << (force_model.useDrag() ? "启用" : "禁用") << std::endl;
    std::cout << "- 太阳辐射压: " << (force_model.useSRP() ? "启用" : "禁用") << std::endl;
    std::cout << "- 月球引力: " << (force_model.useMoonGravity() ? "启用" : "禁用") << std::endl;
    std::cout << "- 相对论修正: " << (force_model.useRelativityCorrection() ? "启用" : "禁用") << std::endl;

    // ========== 4. 设置初始轨道状态 ==========
    // 使用经典轨道元素设置初始状态 (太阳同步轨道示例)
    TimePoint start_time = TimePoint::FromUTC(2026, 6, 15, 12, 0, 0);
    
    // 太阳同步轨道参数
    double semi_major_axis = 7000_km;    // 半长轴
    double eccentricity = 0.001;         // 偏心率
    double inclination = 98.0_deg;       // 轨道倾角
    double raan = 0.0_deg;              // 升交点赤经
    double arg_perigee = 0.0_deg;       // 近地点幅角
    double true_anomaly = 0.0_deg;       // 真近点角

    // 计算初始位置和速度
    Vector3d position, velocity;
    OrbElem elem;
    elem.a_ = semi_major_axis;
    elem.e_ = eccentricity;
    elem.i_ = inclination;
    elem.raan_ = raan;
    elem.argper_ = arg_perigee;
    elem.trueA_ = true_anomaly;
    aOrbElemToCart(elem, kEarthGrav, position, velocity);

    std::cout << "\n【初始轨道状态】" << std::endl;
    std::cout << "- 起始时间: 2026-06-15 12:00:00 UTC" << std::endl;
    std::cout << "- 轨道类型: 太阳同步轨道" << std::endl;
    std::cout << "- 半长轴: " << semi_major_axis / 1000.0 << " km" << std::endl;
    std::cout << "- 偏心率: " << eccentricity << std::endl;
    std::cout << "- 轨道倾角: " << inclination / kDegToRad << "°" << std::endl;
    std::cout << "- 初始位置: (" << position[0]/1000.0 << ", " 
              << position[1]/1000.0 << ", " 
              << position[2]/1000.0 << ") km" << std::endl;
    std::cout << "- 初始速度: (" << velocity[0]/1000.0 << ", " 
              << velocity[1]/1000.0 << ", " 
              << velocity[2]/1000.0 << ") km/s" << std::endl;

    // ========== 5. 执行轨道预报 ==========
    std::cout << "\n【执行轨道预报】" << std::endl;
    
    // 预报时间点列表
    std::vector<double> forecast_durations = {1, 6, 12, 24, 48}; // 小时
    
    for (double hours : forecast_durations) {
        // 备份初始状态
        Vector3d pos = position;
        Vector3d vel = velocity;
        TimePoint end_time = start_time + hours * 3600.0;
        
        // 执行预报
        result = hpop.propagate(start_time, end_time, pos, vel);
        
        if (result == eNoError) {
            // 计算轨道要素
            double radius = pos.norm() / 1000.0;
            double speed = vel.norm() / 1000.0;
            
            std::cout << "- 预报 " << hours << " 小时后:" << std::endl;
            std::cout << "  位置: (" << pos[0]/1000.0 << ", " 
                      << pos[1]/1000.0 << ", " 
                      << pos[2]/1000.0 << ") km" << std::endl;
            std::cout << "  速度: (" << vel[0]/1000.0 << ", " 
                      << vel[1]/1000.0 << ", " 
                      << vel[2]/1000.0 << ") km/s" << std::endl;
            std::cout << "  轨道半径: " << radius << " km" << std::endl;
            std::cout << "  轨道速度: " << speed << " km/s" << std::endl;
        } else {
            std::cout << "- 预报 " << hours << " 小时失败，错误码: " << result << std::endl;
        }
        std::cout << std::endl;
    }

    // ========== 6. 长期轨道预报 ==========
    std::cout << "\n【长期轨道预报 (7天)】" << std::endl;
    
    Vector3d long_term_pos = position;
    Vector3d long_term_vel = velocity;
    TimePoint long_term_end = start_time + 7 * 24 * 3600.0;
    
    result = hpop.propagate(start_time, long_term_end, long_term_pos, long_term_vel);
    
    if (result == eNoError) {
        double initial_radius = position.norm() / 1000.0;
        double final_radius = long_term_pos.norm() / 1000.0;
        double radius_change = final_radius - initial_radius;
        
        std::cout << "预报成功!" << std::endl;
        std::cout << "初始轨道半径: " << initial_radius << " km" << std::endl;
        std::cout << "7天后轨道半径: " << final_radius << " km" << std::endl;
        std::cout << "轨道衰减: " << std::abs(radius_change) << " km" << std::endl;
        
        // 计算轨道周期变化
        double initial_period = 2.0 * kPI * std::sqrt(std::pow(initial_radius * 1000.0, 3) / kEarthGrav);
        double final_period = 2.0 * kPI * std::sqrt(std::pow(final_radius * 1000.0, 3) / kEarthGrav);
        std::cout << "初始轨道周期: " << initial_period / 60.0 << " 分钟" << std::endl;
        std::cout << "7天后轨道周期: " << final_period / 60.0 << " 分钟" << std::endl;
    } else {
        std::cout << "长期预报失败，错误码: " << result << std::endl;
    }

    std::cout << "\n==========================================" << std::endl;
    std::cout << "              轨道预报完成                   " << std::endl;
    std::cout << "==========================================" << std::endl;

    return 0;
}
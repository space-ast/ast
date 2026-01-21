#include "AstCore/HPOP.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/RunTime.hpp"
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
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "HPOP高精度轨道预报器 - 高级力模型配置示例" << std::endl;
    std::cout << "==========================================" << std::endl;
    
    // 创建HPOP对象
    HPOP hpop;
    
    // 配置高级力模型
    HPOPForceModel force_model;
    
    // 1. 配置地球重力场 (JGM3模型)
    force_model.gravity_.model_ = "JGM3";  // 使用JGM3重力场模型
    force_model.gravity_.degree_ = 4;     // 阶数
    force_model.gravity_.order_ = 0;     // 次数
    
    // 2. 配置大气阻力模型
    force_model.useDrag_ = true;         // 启用大气阻力
    
    // 3. 配置第三体引力 (太阳和月球)
    force_model.useMoonGravity_ = true;  // 启用月球引力
    
    // 设置力模型
    err_t result = hpop.setForceModel(force_model);
    if (result != eNoError) {
        std::cout << "设置力模型失败，错误码: " << result << std::endl;
        return -1;
    }
    
    // 初始化HPOP
    result = hpop.initialize();
    if (result != eNoError) {
        std::cout << "初始化失败，错误码: " << result << std::endl;
        return -1;
    }
    
    std::cout << "HPOP初始化成功" << std::endl;
    std::cout << "力模型配置:" << std::endl;
    std::cout << "- 重力场: " << force_model.gravity_.model_ 
              << " (阶数=" << force_model.gravity_.degree_ << ")" << std::endl;
    std::cout << "- 大气阻力: " << (force_model.useDrag_ ? "启用" : "禁用") << std::endl;
    std::cout << "- 月球引力: " << (force_model.useMoonGravity_ ? "启用" : "禁用") << std::endl;
    std::cout << std::endl;
    
    // 设置初始轨道状态 (低地球轨道)
    TimePoint start_time = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    Vector3d position{6878.14_km, 0.0, 0.0};  // 约400km高度
    Vector3d velocity{0.0, 7.668_km_s, 0.0};  // 圆轨道速度
    
    std::cout << "初始轨道状态:" << std::endl;
    std::cout << "起始时间: 2026-01-01 00:00:00 UTC" << std::endl;
    std::cout << "位置: (" << position[0]/1000.0 << ", " 
              << position[1]/1000.0 << ", " 
              << position[2]/1000.0 << ") km" << std::endl;
    std::cout << "速度: (" << velocity[0]/1000.0 << ", " 
              << velocity[1]/1000.0 << ", " 
              << velocity[2]/1000.0 << ") km/s" << std::endl;
    std::cout << std::endl;
    
    // 设置传播时间 (3天)
    TimePoint end_time = start_time + 3 * 24 * 3600.0; // 加3天
    
    // 执行轨道预报
    result = hpop.propagate(start_time, end_time, position, velocity);
    
    if (result == eNoError) {
        std::cout << "轨道预报成功" << std::endl;
        std::cout << "传播时长: 3天" << std::endl;
        std::cout << "结束时间: 2026-01-04 00:00:00 UTC" << std::endl;
        std::cout << "预报后位置: (" << position[0]/1000.0 << ", " 
                  << position[1]/1000.0 << ", " 
                  << position[2]/1000.0 << ") km" << std::endl;
        std::cout << "预报后速度: (" << velocity[0]/1000.0 << ", " 
                  << velocity[1]/1000.0 << ", " 
                  << velocity[2]/1000.0 << ") km/s" << std::endl;
        
        // 计算轨道参数变化
        double initial_radius = 6878.14; // km
        double final_radius = position.norm() / 1000.0; // km
        double radius_change = final_radius - initial_radius;
        
        std::cout << std::endl;
        std::cout << "轨道参数变化:" << std::endl;
        std::cout << "初始轨道半径: " << initial_radius << " km" << std::endl;
        std::cout << "最终轨道半径: " << final_radius << " km" << std::endl;
        std::cout << "轨道半径变化: " << radius_change << " km" << std::endl;
        
    } else {
        std::cout << "轨道预报失败，错误码: " << result << std::endl;
    }
    
    return 0;
}
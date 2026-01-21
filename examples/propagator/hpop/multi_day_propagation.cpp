#include "AstCore/HPOP.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/RunTime.hpp"
#include "AstMath/Vector.hpp"
#include "AstUtil/Literals.hpp"
#include "AstUtil/Constants.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>

AST_USING_NAMESPACE
using namespace _AST literals;

int main()
{
    // 初始化运行时环境
    aInitialize();
    setlocale(LC_ALL, ".UTF-8");
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "HPOP高精度轨道预报器 - 多天传播示例" << std::endl;
    std::cout << "====================================" << std::endl;
    
    // 创建HPOP对象
    HPOP hpop;
    
    // 配置高精度力模型
    HPOPForceModel force_model;
    
    // 配置地球重力场 (使用高精度模型)
    force_model.gravity_.model_ = "EGM2008"; // 使用高精度重力场模型
    force_model.gravity_.degree_ = 8;         // 阶数
    force_model.gravity_.order_ = 8;          // 次数
    
    // 配置大气阻力模型
    force_model.useDrag_ = true;              // 启用大气阻力
    
    // 配置第三体引力
    force_model.useMoonGravity_ = true;       // 启用月球引力
    
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
              << " (阶数=" << force_model.gravity_.degree_ 
              << ", 次数=" << force_model.gravity_.order_ << ")" << std::endl;
    std::cout << "- 大气阻力: " << (force_model.useDrag_ ? "启用" : "禁用") << std::endl;
    std::cout << "- 月球引力: " << (force_model.useMoonGravity_ ? "启用" : "禁用") << std::endl;
    std::cout << std::endl;
    
    // 设置初始轨道状态 (中地球轨道 - 导航卫星轨道)
    TimePoint start_time = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    Vector3d position{26560_km, 0.0, 0.0};   // 约20000km高度
    Vector3d velocity{0.0, 3.87_km_s, 0.0};  // 圆轨道速度
    
    std::cout << "初始轨道状态:" << std::endl;
    std::cout << "起始时间: 2026-01-01 00:00:00 UTC" << std::endl;
    std::cout << "位置: (" << position[0]/1000.0 << ", " 
              << position[1]/1000.0 << ", " 
              << position[2]/1000.0 << ") km" << std::endl;
    std::cout << "速度: (" << velocity[0]/1000.0 << ", " 
              << velocity[1]/1000.0 << ", " 
              << velocity[2]/1000.0 << ") km/s" << std::endl;
    std::cout << std::endl;
    
    // 定义多个传播时间点
    std::vector<double> propagation_days = {1.0, 7.0, 30.0, 90.0}; // 1天, 7天, 30天, 90天
    
    std::cout << "多时间点轨道预报结果:" << std::endl;
    std::cout << "=====================" << std::endl;
    
    // 保存初始状态用于比较
    Vector3d initial_position = position;
    Vector3d initial_velocity = velocity;
    
    for (double days : propagation_days) {
        // 重置到初始状态
        position = initial_position;
        velocity = initial_velocity;
        
        // 设置传播时间
        TimePoint end_time = start_time + days * 24 * 3600.0;
        
        // 执行轨道预报
        result = hpop.propagate(start_time, end_time, position, velocity);
        
        if (result == eNoError) {
            std::cout << "\n传播时长: " << days << " 天" << std::endl;
            // std::cout << "结束时间: " << end_time.toString() << std::endl;
            
            // 计算轨道参数变化
            double initial_radius = initial_position.norm() / 1000.0; // km
            double final_radius = position.norm() / 1000.0;           // km
            double radius_change = final_radius - initial_radius;
            
            double initial_speed = initial_velocity.norm() / 1000.0;  // km/s
            double final_speed = velocity.norm() / 1000.0;            // km/s
            double speed_change = final_speed - initial_speed;
            
            std::cout << "轨道半径变化: " << radius_change << " km" << std::endl;
            std::cout << "轨道速度变化: " << speed_change << " km/s" << std::endl;
            
        } else {
            std::cout << "传播 " << days << " 天失败，错误码: " << result << std::endl;
        }
    }
    
    return 0;
}
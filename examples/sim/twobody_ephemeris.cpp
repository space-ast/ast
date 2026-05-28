#include "ast/MotionTwoBody.hpp"
#include "ast/MotionProfile.hpp"
#include "ast/OrbitElement.hpp"
#include "ast/StateCartesian.hpp"
#include "ast/Frame.hpp"
#include "ast/EventInterval.hpp"
#include "ast/Ephemeris.hpp"
#include "ast/Literals.hpp"
#include "ast/Constants.h"
#include "ast/ScopedPtr.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>

AST_USING_NAMESPACE
using namespace _AST literals;

int main()
{
    setlocale(LC_ALL, ".UTF8");
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "二体预报生成星历示例 (使用 MotionProfile 接口)" << std::endl;
    std::cout << "==============================================" << std::endl;
    
    // 创建二体运动模型
    auto motion = MotionTwoBody::New();
    
    // 设置初始状态
    auto initialState = StateCartesian::New();
    
    // 设置参考坐标系 (J2000)
    initialState->setFrameByName("Earth ICRF");
    auto frame = initialState->getFrame();

    // 设置引力常数 (地球)
    initialState->setGM(kEarthGrav);
    
    // 设置初始历元时间 (J2000.0)
    TimePoint epoch = TimePoint::J2000TT();
    initialState->setStateEpoch(epoch);
    
    // 设置初始笛卡尔状态 (近地圆轨道，高度500km)
    double altitude = 500_km;
    double radius = kEarthRadius + altitude;
    double orbital_velocity = std::sqrt(kEarthGrav / radius);
    
    CartState cartState;
    cartState.pos() = Vector3d{radius, 0.0, 0.0};
    cartState.vel() = Vector3d{0.0, orbital_velocity, 0.0};
    initialState->setState(cartState);
    
    // 将初始状态设置到运动模型
    motion->setInitialState(initialState);
    motion->setPropagationFrame(frame);
    
    // 设置预报时间间隔 (从J2000开始，持续1小时)
    auto interval = TimeInterval(epoch, epoch + 3600);
    motion->setInterval(interval);
    
    // 设置预报步长 (1分钟)
    motion->setStepSize(60.0);
    
    std::cout << "初始轨道参数:" << std::endl;
    std::cout << "轨道高度: " << altitude / 1000.0 << " km" << std::endl;
    std::cout << "轨道半径: " << radius / 1000.0 << " km" << std::endl;
    std::cout << "轨道速度: " << orbital_velocity / 1000.0 << " km/s" << std::endl;
    std::cout << "轨道周期: " << (2 * kPI * radius / orbital_velocity) / 60.0 << " 分钟" << std::endl;
    std::cout << "预报时长: 1 小时" << std::endl;
    std::cout << "预报步长: 60 秒" << std::endl;
    std::cout << std::endl;
    
    // 使用 MotionProfile 接口生成简单星历
    ScopedPtr<Ephemeris> eph;
    errc_t result = motion->makeEphemerisSimple(eph);
    
    if (result == eNoError && eph) {
        std::cout << "星历生成成功!" << std::endl;
        std::cout << std::endl;
        
        // 查询星历数据
        std::cout << "星历数据:" << std::endl;
        std::cout << "时间(秒)\tX(km)\t\tY(km)\t\tZ(km)\t\tVx(km/s)\tVy(km/s)\tVz(km/s)" << std::endl;
        std::cout << "---------------------------------------------------------------------------------------" << std::endl;
        
        for (int i = 0; i <= 60; i += 10) {
            double time = i * 60.0; // 每10分钟取一个点
            TimePoint tp = epoch + time;
            
            Vector3d pos, vel;
            result = eph->getPosVel(tp, pos, vel);
            
            if (result == eNoError) {
                std::cout << std::setw(8) << time << "\t"
                          << std::setw(10) << pos[0]/1000.0 << "\t"
                          << std::setw(10) << pos[1]/1000.0 << "\t"
                          << std::setw(10) << pos[2]/1000.0 << "\t"
                          << std::setw(12) << vel[0]/1000.0 << "\t"
                          << std::setw(12) << vel[1]/1000.0 << "\t"
                          << std::setw(12) << vel[2]/1000.0 << std::endl;
            } else {
                std::cout << "获取星历数据失败，时间: " << time << " 秒，错误码: " << result << std::endl;
            }
        }
        
        // 获取星历时间间隔
        TimeInterval ephInterval;
        result = eph->getInterval(ephInterval);
        if (result == eNoError) {
            std::cout << std::endl;
            std::cout << "星历时间范围:" << std::endl;
            std::cout << "开始时间: " << ephInterval.getStart().toString() << std::endl;
            std::cout << "结束时间: " << ephInterval.getStop().toString()  << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "星历生成完成!" << std::endl;
    } else {
        std::cout << "星历生成失败，错误码: " << result << std::endl;
        return 1;
    }
    
    return 0;
}
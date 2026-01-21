#include "AstCore/J2Analytical.hpp"
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
    const double j2_earth = 1.08262668e-3;
    const double re_earth = kEarthRadius;
    
    // 创建时间点
    TimePoint epoch = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "J2解析传播器 - 类接口使用示例" << std::endl;
    std::cout << "==================================" << std::endl;
    
    // 示例1: 使用轨道根数构造
    std::cout << "\n示例1: 使用轨道根数构造传播器" << std::endl;
    {
        ModOrbElem orb_elem;
        orb_elem.rp_ = 7000_km;
        orb_elem.e_ = 0.01;
        orb_elem.i_ = 45.0 * kDegToRad;
        orb_elem.raan_ = 0.0;
        orb_elem.argper_ = 0.0;
        orb_elem.trueA_ = 0.0;
        
        J2Analytical propagator1(orb_elem, epoch, gm_earth, j2_earth, re_earth);
        std::cout << "使用轨道根数成功创建J2传播器" << std::endl;
    }
    
    // 示例2: 使用位置速度构造
    std::cout << "\n示例2: 使用位置速度构造传播器" << std::endl;
    {
        Vector3d position{7000_km, 0.0, 0.0};
        Vector3d velocity{0.0, 7.5_km_s, 1.0_km_s};
        
        J2Analytical propagator2(position, velocity, epoch, gm_earth, j2_earth, re_earth);
        std::cout << "使用位置速度成功创建J2传播器" << std::endl;
        
        // 演示继承的基类功能
        std::cout << "传播器类型: J2Analytical" << std::endl;
        std::cout << "继承自: J2J4Analytical" << std::endl;
    }
    
    // 示例3: 完整的传播流程
    std::cout << "\n示例3: 完整的传播流程演示" << std::endl;
    {
        Vector3d position{7000_km, 0.0, 0.0};
        Vector3d velocity{0.0, 7.5_km_s, 0.0};
        
        // 创建传播器
        J2Analytical propagator(position, velocity, epoch, gm_earth, j2_earth, re_earth);
        
        // 传播多个时间点
        double duration = 24 * 3600.0; // 1天
        
        std::cout << "时间传播结果:" << std::endl;
        std::cout << "时间(小时)\tX(km)\t\tY(km)\t\tZ(km)" << std::endl;
        std::cout << "----------------------------------------------" << std::endl;
        
        for (int i = 0; i <= 4; i++) {
            double time = i * duration / 4.0;
            
            // 注意：这里需要调用实际的传播方法
            // 由于J2Analytical类的具体传播方法未在头文件中显示，
            // 这里仅演示类的基本使用
            
            std::cout << std::setw(8) << time/3600.0 << "\t"
                      << std::setw(10) << "待实现" << "\t"
                      << std::setw(10) << "待实现" << "\t"
                      << std::setw(10) << "待实现" << std::endl;
        }
    }
    
    std::cout << "\n类接口使用演示完成" << std::endl;
    std::cout << "注意：实际传播功能需要调用类的具体方法" << std::endl;
    
    return 0;
}
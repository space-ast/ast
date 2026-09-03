///
/// @file      coefficient_update.cpp
/// @brief     HPOP 动态更新阻力/光压系数示例
/// @details   演示无需重新初始化 HPOP，即可通过 aHPOPEquation_SetDragCoefficient /
///            aHPOPEquation_SetSRPCoefficient 动态修改阻力系数(Cd)与太阳光压系数(Cr)。
///            依次对默认系数与修改后的系数各做一次预报，对比轨道衰减(高度下降)，
///            以说明系数在积分过程中即时生效。
/// @author    axel
/// @date      2026-09-04
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
///
///    http://www.apache.org/licenses/LICENSE-2.0
///
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "ast/HPOP.hpp"
#include "ast/HPOPForceModel.hpp"
#include "ast/HPOPUtil.hpp"
#include "ast/SpacecraftParam.hpp"
#include "ast/TimePoint.hpp"
#include "ast/RunTime.hpp"
#include "ast/Vector.hpp"
#include "ast/Literals.hpp"
#include "ast/Constants.h"
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
    std::cout << "HPOP高精度轨道预报器 - 动态更新阻力/光压系数示例" << std::endl;
    std::cout << "================================================" << std::endl;

    // ========== 1. 创建HPOP并配置力模型 ==========
    HPOP hpop;

    HPOPForceModel forcemodel;
    forcemodel.gravity().model_ = "JGM3";   // 地球重力场
    forcemodel.gravity().maxDegree_ = 4;
    forcemodel.gravity().maxOrder_ = 0;

    forcemodel.useDrag(true);               // 启用大气阻力
    forcemodel.useSRP(true);                // 启用太阳辐射压

    errc_t result = hpop.setForceModel(forcemodel);
    if (result != eNoError) {
        std::cout << "[错误] 设置力模型失败，错误码: " << result << std::endl;
        return -1;
    }

    // ========== 2. 设置航天器参数（初始 Cd=2.2, Cr=1.0） ==========
    SpacecraftParam scParam;
    scParam.setDryMass(1000.0);             // 干质量 kg
    scParam.setFuelMass(0.0);               // 燃料质量 kg
    scParam.setDragArea(20.0);              // 阻力面积 m^2
    scParam.setCd(2.2);                     // 阻力系数
    scParam.setSrpArea(20.0);               // 光压面积 m^2
    scParam.setCr(1.0);                     // 光压系数
    hpop.setSpacecraftParam(scParam);

    // ========== 3. 初始化 HPOP（此处才会真正构建力模型/函数块） ==========
    result = hpop.initialize();
    if (result != eNoError) {
        std::cout << "[错误] HPOP初始化失败，错误码: " << result << std::endl;
        return -1;
    }

    // 获取内部方程实例（初始化后方程已建立，dragBlock/srpBlock 均已挂载）
    HPOPEquation* equation = hpop.equation();
    if (!equation) {
        std::cout << "[错误] 未能获取 HPOPEquation 实例" << std::endl;
        return -1;
    }

    std::cout << "\n【初始配置】" << std::endl;
    std::cout << "- 阻力系数 Cd: " << scParam.cd() << std::endl;
    std::cout << "- 光压系数 Cr: " << scParam.cr() << std::endl;

    // ========== 4. 设置初始轨道状态（约500km高度近地圆轨道） ==========
    TimePoint start_time = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    Vector3d position{6878_km, 0.0, 0.0};   // 约500km高度
    Vector3d velocity{0.0, 7.668_km_s, 0.0};

    std::cout << "\n【初始轨道状态】" << std::endl;
    std::cout << "- 起始时间: 2026-01-01 00:00:00 UTC" << std::endl;
    std::cout << "- 位置: (" << position[0]/1000.0 << ", "
              << position[1]/1000.0 << ", " << position[2]/1000.0 << ") km" << std::endl;
    std::cout << "- 速度: (" << velocity[0]/1000.0 << ", "
              << velocity[1]/1000.0 << ", " << velocity[2]/1000.0 << ") km/s" << std::endl;
    std::cout << std::endl;

    const double days = 2.0;                // 预报时长：2天
    TimePoint end_time = start_time + days * 24 * 3600.0;

    // ========== 5. 第一次预报：使用默认系数 ==========
    Vector3d pos_default = position;
    Vector3d vel_default = velocity;
    TimePoint end1 = end_time;
    result = hpop.propagate(start_time, end1, pos_default, vel_default);
    if (result != eNoError) {
        std::cout << "[错误] 第一次预报失败，错误码: " << result << std::endl;
        return -1;
    }
    double radius_default = pos_default.norm() / 1000.0;   // km

    std::cout << "【第一次预报（默认 Cd=2.2, Cr=1.0）】" << std::endl;
    std::cout << "- 预报时长: " << days << " 天" << std::endl;
    std::cout << "- 最终高度: " << radius_default - kEarthRadius / 1000.0 << " km" << std::endl;
    std::cout << std::endl;

    // ========== 6. 不重新初始化，直接更新系数 ==========
    // 注意：这里没有再调用 hpop.initialize() / hpop.setForceModel()，
    // 直接修改方程内部函数块的系数，改动在后续积分过程中即时生效。
    result = aHPOPEquation_SetDragCoefficient(equation, 2.8);
    if (result != eNoError) {
        std::cout << "[错误] 设置阻力系数失败，错误码: " << result << std::endl;
        return -1;
    }
    result = aHPOPEquation_SetSRPCoefficient(equation, 1.5);
    if (result != eNoError) {
        std::cout << "[错误] 设置光压系数失败，错误码: " << result << std::endl;
        return -1;
    }

    std::cout << "【动态更新系数（未重新初始化HPOP）】" << std::endl;
    std::cout << "- 阻力系数 Cd: 2.2 -> 2.8" << std::endl;
    std::cout << "- 光压系数 Cr: 1.0 -> 1.5" << std::endl;
    std::cout << std::endl;

    // ========== 7. 第二次预报：从相同初值出发，使用新系数 ==========
    Vector3d pos_modified = position;
    Vector3d vel_modified = velocity;
    TimePoint end2 = end_time;
    result = hpop.propagate(start_time, end2, pos_modified, vel_modified);
    if (result != eNoError) {
        std::cout << "[错误] 第二次预报失败，错误码: " << result << std::endl;
        return -1;
    }
    double radius_modified = pos_modified.norm() / 1000.0;  // km

    std::cout << "【第二次预报（更新后 Cd=2.8, Cr=1.5）】" << std::endl;
    std::cout << "- 预报时长: " << days << " 天" << std::endl;
    std::cout << "- 最终高度: " << radius_modified - kEarthRadius / 1000.0 << " km" << std::endl;
    std::cout << std::endl;

    // ========== 8. 结果对比 ==========
    double altitude_default  = radius_default  - kEarthRadius / 1000.0;
    double altitude_modified = radius_modified - kEarthRadius / 1000.0;

    std::cout << "【对比】" << std::endl;
    std::cout << "- 默认系数最终高度: " << altitude_default << " km" << std::endl;
    std::cout << "- 新系数最终高度:   " << altitude_modified << " km" << std::endl;
    std::cout << "- 高度差异:         " << (altitude_modified - altitude_default) << " km" << std::endl;

    // 说明系数越大，阻力衰减越明显（高度下降更多）
    if (altitude_modified < altitude_default) {
        std::cout << "  增大Cd后轨道高度下降更多，说明新系数已在预报中生效。" << std::endl;
    }
    else
    {
        std::cout << "  增大Cd后轨道高度未下降更多，说明新系数未生效。" << std::endl;
        return -1;
    }

    std::cout << std::endl;
    std::cout << "示例运行完毕。" << std::endl;

    return 0;
}

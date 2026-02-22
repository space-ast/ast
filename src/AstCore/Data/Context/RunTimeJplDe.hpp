///
/// @file      RunTimeJplDe.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-22
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#pragma once

#include "AstGlobal.h"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Data
    @{
*/


/// @brief 获取JPL DE星历数据的位置和速度（ICRF）
/// @param time 
/// @param target 
/// @param referenceBody 
/// @param pos 
/// @param vel 
/// @return 
AST_CORE_CAPI err_t aJplDeGetPosVelICRF(
    const TimePoint& time, 
    int target, 
    int referenceBody, 
    Vector3d& pos,
    Vector3d& vel
);

/// @brief 获取JPL DE星历数据的位置（ICRF）
/// @param time 
/// @param target 
/// @param referenceBody 
/// @param pos 
/// @return 
AST_CORE_CAPI err_t aJplDeGetPosICRF(
    const TimePoint& time,
    int target,
    int referenceBody,
    Vector3d& pos
);


/// @brief 获取JPL DE星历数据的中章动角数据
/// @param time 
/// @param dpsi 
/// @param deps 
/// @return 
AST_CORE_CAPI err_t aJplDeGetNutation(
    const TimePoint& time,
    double& dpsi,
    double& deps
);

/// @brief 获取JPL DE星历数据的月面天平动相关角度
/// @param time 
/// @param ang  - 从ICRF到月球惯性主轴坐标系的转角(rad)
///             - [omega,i,u],[进动角,章动角,自转角],按照313顺序旋转
/// @return 
AST_CORE_API err_t aJplDeGetLibration(
    const TimePoint& time,
    Vector3d& ang
);


/// @brief 获取JPL DE星历数据的月面天平动相关角度（Euler）
/// @param time 
/// @param ang  - 从ICRF到月球惯性主轴坐标系的转角(rad)
///             - [omega,i,u],[进动角,章动角,自转角],按照313顺序旋转
/// @return 
AST_CORE_API err_t aJplDeGetLibration(
    const TimePoint& time,
    Euler& ang
);


/// @brief 获取JPL DE星历数据的版本号
/// @return 星历版本号
AST_CORE_CAPI int aJplDeNum();


/// @brief 打开JPL DE星历数据文件
/// @param filepath 
/// @return 
AST_CORE_CAPI err_t aJplDeOpen(const char* filepath);



/// @brief 关闭JPL DE星历数据文件
AST_CORE_CAPI void aJplDeClose();


/*! @} */

AST_NAMESPACE_END

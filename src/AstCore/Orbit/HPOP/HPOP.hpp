///
/// @file      HPOP.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-16
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
#include <string>

AST_NAMESPACE_BEGIN

/// @brief 高精度轨道预报力模型
class HPOPForceModel
{
public:
    struct Gravity
    {
        std::string gravityModel_ = "";     ///< 中心天体重力场模型(模型名称或者引力场文件路径)
        int degree_ = 2;                    ///< 中心天体重力场计算阶数
        int order_ = 0;                     ///< 中心天体重力场计算次数
    };
public:
    Gravity gravity_;                       ///< 中心天体重力场
    bool useMoonGravity_ = false;           ///< 是否使用月球引力
};

/// @brief 高精度轨道预报接口类
class HPOP
{
public:
    HPOP() = default;
    ~HPOP() = default;
public:
    void setForceModel(const HPOPForceModel& forcemodel){forcemodel_ = forcemodel;}

protected:
    HPOPForceModel forcemodel_;     ///< 高精度轨道预报力模型
};

AST_NAMESPACE_END

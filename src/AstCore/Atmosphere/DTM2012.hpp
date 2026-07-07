///
/// @file      DTM2012.hpp
/// @brief     DTM-2012 大气模型
/// @details   DTM（Drag Temperature Model）2012 大气密度模型封装。
///            模型仅适用于高度 > 120 km 的高层大气。
/// @author    axel
/// @date      2026-06-27
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

#pragma once

#include "AstGlobal.h"
#include "AstCore/AtmosphereBase.hpp"
#include <memory>
#include <string>

// 前向声明：DTM_12 的系数类型（全局作用域，定义在 AstWeather/Atmosphere/DTM_12/DTM_12.h 中）
struct pardtmtype;

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/

/// @brief 从文件加载DTM2012系数
/// @param path 系数文件路径
/// @param pardtm 输出：温度/密度系数
/// @return eNoError 表示加载成功，否则返回错误码
AST_CORE_API errc_t aLoadDTM2012Coefficients(const std::string& path, pardtmtype& pardtm);

/// @brief DTM-2012大气模型
/// @details DTM-2012 (Drag Temperature Model) 是CNES发布的高层大气密度模型。
///          使用 F10.7 太阳射电通量和 Kp 地磁指数驱动。
///          模型仅适用于高度 > 120 km。
class AST_CORE_API DTM2012 final: public AtmosphereBase
{
public:
    DTM2012(Frame* frame, BodyShape* bodyShape,
            double f107Daily, double f107Average, double kp,
            const std::string& coeffFilePath = "");
    ~DTM2012() override;

    /// @brief 计算给定时间和位置的大气密度
    /// @param tp 时间点
    /// @param posInBodyFixed 地固系下的位置 [m]
    /// @return 大气密度 [kg/m³]
    double getDensity(const TimePoint& tp, const Vector3d& posInBodyFixed) const override;

    /// @brief 检查系数文件是否加载成功
    bool isInitialized() const { return initialized_; }

private:
    double F107Daily_{0.0};
    double F107Average_{0.0};
    double kp_{0.0};
    mutable bool initialized_{false};

    /// @brief 不透明指针，存储 DTM_12 的状态结构体
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

/*! @} */

AST_NAMESPACE_END

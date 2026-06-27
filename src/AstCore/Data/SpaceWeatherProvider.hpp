///
/// @file      SpaceWeatherProvider.hpp
/// @brief     空间天气数据提供者抽象接口
/// @details   定义了空间天气指数查询的抽象接口，遵循依赖注入的Provider模式。
///            具体实现可以从不同数据源（文件、网络、模拟数据等）提供空间天气数据。
/// @author    axel
/// @date      2026-06-27
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

/// @brief 空间天气数据提供者抽象接口
/// @details
/// 提供空间天气指数（Ap、Kp、F10.7等）的查询接口。
///
/// 关于接口设计：
/// 核心接口只包含所有空间天气数据源的公共查询方法。
/// 具体的数据加载方式（文件、网络、内嵌数据等）由子类实现，
/// 不出现在抽象接口中，以保持接口的简洁和可替换性。
/// @see SpaceWeather 基于Celestrak文件的具体实现
class AST_CORE_API SpaceWeatherProvider
{
public:
    virtual ~SpaceWeatherProvider() = default;

    /// @brief 获取指定时间的Ap日平均值
    /// @param tp 时间点
    /// @return 对应时间点的Ap值（0-400），无数据时返回0.0
    virtual double getApDaily(const TimePoint& tp) const = 0;

    /// @brief 获取指定时间的Kp日平均值
    /// @param tp 时间点
    /// @return 对应时间点的Kp值（0-9），无数据时返回0.0
    virtual double getKpDaily(const TimePoint& tp) const = 0;

    /// @brief 获取指定时间的F10.7单日观测值
    /// @param tp 时间点
    /// @return 对应时间点的F10.7观测值（10⁻²² W·m⁻²·Hz⁻¹），无数据时返回0.0
    virtual double getF10p7Daily(const TimePoint& tp) const = 0;

    /// @brief 获取指定时间的F10.7平均观测值
    /// @param tp 时间点
    /// @return 以输入时间为中心的81天平均F10.7观测值（调整到1 AU），无数据时返回0.0
    virtual double getF10p7Average(const TimePoint& tp) const = 0;
};

/*! @} */

AST_NAMESPACE_END

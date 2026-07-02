///
/// @file      ConstantSpaceWeather.hpp
/// @brief     恒定空间天气数据提供者
/// @details   返回用户指定的常数值，适用于以下场景：
///            - 测试：提供确定性的输入，无需依赖外部数据文件
///            - 默认/回退：在无真实空间天气数据时提供合理的默认值
///            - 简化仿真：当空间天气效应对结果影响不显著时
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

#include "SpaceWeatherProvider.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Data
    @{
*/

/// @brief 恒定空间天气数据提供者
/// @details
/// SpaceWeatherProvider 的具体实现，所有查询方法返回用户预设的常数值。
/// 默认值代表中等太阳活动水平下的典型空间天气条件。
///
/// 使用示例：
/// @code
/// // 使用默认值
/// ConstantSpaceWeather sw;
/// double ap = sw.getApDaily(tp);     // 返回 15.0
///
/// // 自定义值
/// ConstantSpaceWeather sw(150.0, 150.0, 15.0, 3.0);
/// @endcode
class AST_CORE_API ConstantSpaceWeather: public SpaceWeatherProvider
{
public:
    /// @brief 默认中等太阳活动水平
    static constexpr double kDefaultAp         = 15.0;   ///< 默认Ap日平均值
    static constexpr double kDefaultKp         = 3.0;    ///< 默认Kp日平均值
    static constexpr double kDefaultF10p7Daily = 150.0;  ///< 默认F10.7单日观测值
    static constexpr double kDefaultF10p7Avg   = 150.0;  ///< 默认F10.7平均观测值

    /// @brief 构造，使用默认中等太阳活动水平值
    ConstantSpaceWeather() = default;

    /// @brief 构造，指定所有空间天气指数
    /// @param f10p7Daily F10.7单日观测值（10⁻²² W·m⁻²·Hz⁻¹）
    /// @param f10p7Average F10.7平均观测值（10⁻²² W·m⁻²·Hz⁻¹）
    /// @param ap Ap日平均值（0-400）
    /// @param kp Kp日平均值（0-9）
    ConstantSpaceWeather(double f10p7Daily, double f10p7Average, double ap, double kp)
        : f10p7Daily_(f10p7Daily)
        , f10p7Average_(f10p7Average)
        , ap_(ap)
        , kp_(kp)
    {}

    ~ConstantSpaceWeather() override = default;

    // --- SpaceWeatherProvider 接口实现 ---

    double getApDaily(const TimePoint&) const override       { return ap_; }
    double getKpDaily(const TimePoint&) const override       { return kp_; }
    double getF10p7Daily(const TimePoint&) const override    { return f10p7Daily_; }
    double getF10p7Average(const TimePoint&) const override  { return f10p7Average_; }

    // --- 配置方法 ---

    /// @brief 设置Ap日平均值
    void setAp(double ap)                   { ap_ = ap; }
    /// @brief 获取Ap日平均值
    double getAp() const                    { return ap_; }

    /// @brief 设置Kp日平均值
    void setKp(double kp)                   { kp_ = kp; }
    /// @brief 获取Kp日平均值
    double getKp() const                    { return kp_; }

    /// @brief 设置F10.7单日值
    void setF10p7Daily(double f10p7)        { f10p7Daily_ = f10p7; }
    /// @brief 获取F10p7单日值
    double getF10p7Daily() const            { return f10p7Daily_; }

    /// @brief 设置F10.7平均值
    void setF10p7Average(double f10p7Avg)   { f10p7Average_ = f10p7Avg; }
    /// @brief 获取F10p7平均值
    double getF10p7Average() const          { return f10p7Average_; }

protected:
    double f10p7Daily_   = kDefaultF10p7Daily;  ///< F10.7单日值
    double f10p7Average_ = kDefaultF10p7Avg;    ///< F10.7平均值
    double ap_           = kDefaultAp;          ///< Ap日平均值
    double kp_           = kDefaultKp;          ///< Kp日平均值
};

/*! @} */

AST_NAMESPACE_END

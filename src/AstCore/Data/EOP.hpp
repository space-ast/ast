///
/// @file      EOP.hpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      30.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。
 
#pragma once
 
#include "AstGlobal.h"
#include <vector>

AST_NAMESPACE_BEGIN


/// @brief 地球指向参数（Earth Orientation Parameters）
class AST_CORE_API EOP
{
public:
    struct Entry
    {
        int mjd;                ///< 简约儒略日
        int dat;                ///< 闰秒
        double x, y;            ///< 极移
        double ut1_utc;         ///< ut1 - utc
        double lod;             ///< length of day
        double dpsi, deps;      ///< 章动修正
        double dx, dy;          ///< XYS系数修正
    };
public:
    EOP() = default;
    ~EOP() = default;

    /// @brief 加载默认EOP数据
    /// @details 加载默认EOP数据，路径为 $(AST_DATA_DIR)/SolarSystem/Earth/EOP-All.txt。
    /// @return 错误码
    err_t loadDefault();

    /// @brief 加载EOP数据
    /// @param filepath 文件路径
    /// @return 错误码
    err_t load(StringView filepath);

    /// @brief 加载EOP数据
    /// @param filepath 文件路径
    /// @param data 数据容器
    /// @return 错误码
    static err_t load(StringView filepath, std::vector<Entry>& data);

    /// @brief 获取数据大小
    size_t size() const { return m_data.size(); }

    /// @brief 获取ut1-utc值
    /// @details 线性插值获取指定时间点的ut1-utc值。
    /// @param tp 时间点
    /// @return ut1-utc值
    double ut1MinusUTC(const TimePoint& tp) const;

    /// @brief 获取ut1-utc值
    /// @details 线性插值获取指定时间点的ut1-utc值。
    /// @param jdUTC 时间点（UTC）
    /// @return ut1-utc值
    double ut1MinusUTC_UTC(const JulianDate& jdUTC) const;

    /// @brief 获取ut1-utc值
    /// @details 线性插值获取指定时间点的ut1-utc值。
    /// @param mjdUTC 时间点（UTC）的MJD值
    /// @return ut1-utc值
    double ut1MinusUTC_UTCMJD(double mjdUTC) const;

protected:
    /// @brief 查找时间点对应的索引和插值系数
    /// @details 查找时间点对应的索引和插值系数，用于线性插值。
    /// @param mjdUTC 时间点（UTC）的MJD值
    /// @param index 索引
    /// @param frac 插值系数
    void findEntryIndex(double mjdUTC, size_t& index, double& frac) const;
protected:
    std::vector<Entry> m_data;
    double m_startMJD = 0.0;
    double m_endMJD = 0.0;
};

AST_NAMESPACE_END
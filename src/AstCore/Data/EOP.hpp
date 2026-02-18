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


/*!
    @ingroup   Core
    @defgroup Data 数据读取
    @brief 提供地球指向参数（EOP）、闰秒、行星星历等数据的加载和查询功能。
*/


/*!
    @addtogroup Data
    @{
*/



/// @brief 地球指向参数（Earth Orientation Parameters）
/// @details 
/// 支持加载从Celestrak网站下载的EOP数据文件。
/// 根据当前时间查询MJD、闰秒、极移、ut1-utc、length of day、章动修正、XYS系数修正等参数。
/// 在查询时，会根据当前时间点的MJD值，线性插值获取最近的两个条目
/// @see https://celestrak.org/SpaceData/EOP-format.php
class AST_CORE_API EOP
{
public:
    /// @brief 地球指向参数条目
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

    /// @brief 卸载EOP数据
    void unload();

    /// @brief 加载EOP数据
    /// @param filepath 文件路径
    /// @param data 数据容器
    /// @return 错误码
    static err_t load(StringView filepath, std::vector<Entry>& data);

    /// @brief 获取数据大小
    size_t size() const { return m_data.size(); }

    /// @brief 获取指定MJD的EOP条目
    /// @param mjd 简约儒略日
    /// @return EOP条目
    const Entry* getEntry(int mjd) const;


    /// @brief 设置指定MJD的EOP条目
    /// @param mjd 简约儒略日
    /// @param entry EOP条目
    /// @return 错误码
    err_t setEntry(int mjd, const Entry& entry);

    //----------------
    // ut1 - utc
    //----------------

    /// @brief 获取ut1-utc值
    /// @details 线性插值获取指定时间点的ut1-utc值。
    /// @param tp 时间点
    /// @return ut1-utc值
    double getUT1MinusUTC(const TimePoint& tp) const;

    /// @brief 获取ut1-utc值
    /// @details 线性插值获取指定时间点的ut1-utc值。
    /// @param jdUTC 时间点（UTC）
    /// @return ut1-utc值
    double getUT1MinusUTC_UTC(const JulianDate& jdUTC) const;

    /// @brief 获取ut1-utc值
    /// @details 线性插值获取指定时间点的ut1-utc值。
    /// @param mjdUTC 时间点（UTC）的MJD值
    /// @return ut1-utc值
    double getUT1MinusUTC_UTCMJD(double mjdUTC) const;


    //----------------
    // pole motion
    //----------------


    /// @brief 获取极移
    /// @details 线性插值获取指定时间点的极移。
    /// @param tp 时间点
    /// @param pm 极移
    void getPoleMotion(const TimePoint& tp, double& x, double& y) const;

    /// @brief 获取极移（UTC）
    /// @details 线性插值获取指定时间点的极移（UTC）。
    /// @param jdUTC 时间点（UTC）
    /// @param pm 极移
    void getPoleMotionUTC(const JulianDate& jdUTC, double& x, double& y) const;


    /// @brief 获取极移（UTC）
    /// @details 线性插值获取指定时间点的极移（UTC）。
    /// @param mjdUTC 时间点（UTC）的MJD值
    /// @param pm 极移
    void getPoleMotionUTCMJD(double mjdUTC, double& x, double& y) const;
    

    //----------------
    // lod
    //----------------

    /// @brief 获取LOD值
    /// @details 线性插值获取指定时间点的LOD值。
    /// @param tp 时间点
    /// @return LOD值
    double getLOD(const TimePoint& tp) const;

    /// @brief 获取LOD值（UTC）
    /// @details 线性插值获取指定时间点的LOD值（UTC）。
    /// @param jdUTC 时间点（UTC）
    /// @return LOD值
    double getLOD_UTC(const JulianDate& jdUTC) const;

    /// @brief 获取LOD值（UTC）
    /// @details 线性插值获取指定时间点的LOD值（UTC）。
    /// @param mjdUTC 时间点（UTC）的MJD值
    /// @return LOD值
    double getLOD_UTCMJD(double mjdUTC) const;


    //----------------
    // xy correction
    //----------------

    /// @brief 获取IAU XY修正项
    /// @details 线性插值获取指定时间点的IAU XY修正项。
    /// @param tp 时间点
    /// @param xyCorrection 修正项
    void getXYCorrection(const TimePoint& tp, array2d& xyCorrection) const;

    /// @brief 获取IAU XY修正项（UTC）
    /// @details 线性插值获取指定时间点的IAU XY修正项（UTC）。
    /// @param jdUTC 时间点（UTC）
    /// @param xyCorrection 修正项
    void getXYCorrectionUTC(const JulianDate& jdUTC, array2d& xyCorrection) const;

    /// @brief 获取IAU XY修正项（UTC）
    /// @details 线性插值获取指定时间点的IAU XY修正项（UTC）。
    /// @param mjdUTC 时间点（UTC）的MJD值
    /// @param xyCorrection 修正项
    void getXYCorrectionUTCMJD(double mjdUTC, array2d& xyCorrection) const;
    
protected:
    /// @brief 获取指定索引和插值系数的成员值
    /// @details 获取指定索引和插值系数的成员值，用于线性插值。
    /// @param index 索引
    /// @param frac 插值系数
    /// @return 成员值
    template<double Entry::* Member>
    A_ALWAYS_INLINE double getValue(size_t index, double frac) const
    {
        return m_data[index].*Member + frac * (m_data[index+1].*Member - m_data[index].*Member);
    }

    /// @brief 查找时间点对应的索引和插值系数
    /// @details 查找时间点对应的索引和插值系数，用于线性插值。
    /// @param mjdUTC 时间点（UTC）的MJD值
    /// @param index 索引
    /// @param frac 插值系数
    void findEntryIndex(double mjdUTC, int& index, double& frac) const;
protected:
    std::vector<Entry> m_data;
    int m_startMJD = 0;
    int m_endMJD = 0;
};


/*! @} */

AST_NAMESPACE_END